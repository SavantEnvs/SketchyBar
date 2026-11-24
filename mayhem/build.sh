#!/usr/bin/env bash
#
# mayhem/build.sh — build this repo's fuzz harness(es). EDIT per repo.
#
# Runs inside the commit image (mayhem/Dockerfile) as `mayhem` in /mayhem. The base image
# (ghcr.io/savantenvs/base) already exports the build contract — use these, don't redefine:
#   CC, CXX             stock clang / clang++
#   LIB_FUZZING_ENGINE  -fsanitize=fuzzer   (link into each harness that has a LLVMFuzzer entry)
#   SANITIZER_FLAGS     -fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer
#                       (ASan + UBSan, both set to HALT — so Mayhem catches memory AND UB defects)
#   DEBUG_FLAGS         -g -gdwarf-3   (DWARF debug info — always on for fuzz/standalone builds,
#                       independent of the sanitizer off-switch; DWARF version must be < 4)
#   RUST_DEBUG_FLAGS    -C debuginfo=2 -C force-frame-pointers=yes -C llvm-args=-gdwarf-3
#                       (thread through RUSTFLAGS on every cargo-fuzz build)
#   GO_DEBUG_FLAGS      -gcflags=all=-N -l
#                       (thread through go build / go-fuzz-build so the linked ELF keeps symbols)
#   SRC                 /mayhem (the repo source)
#
# Contract: build EVERYTHING here — one runnable binary per fuzz harness, AND the project's test
# suite (so mayhem/test.sh only has to RUN it, never compile). Keep it ADDITIVE (build upstream as
# upstream documents; don't edit upstream files). IMPORTANT: build the PROJECT ITSELF with
# $SANITIZER_FLAGS and $DEBUG_FLAGS (not just the harness) so the fuzzed code is instrumented
# AND carries DWARF < 4 symbols — otherwise ASan/UBSan only see the harness, not the library
# you're trying to find bugs in, and backtraces won't resolve project source lines. Build the TEST
# suite with the project's NORMAL flags (a clean, independent build) so test.sh stays an honest
# functional oracle and won't false-fail on benign UB. Leave the test binary/runner where test.sh
# expects it.
set -euo pipefail

# clang rejects SOURCE_DATE_EPOCH='' (empty) — it must be unset or a valid integer.
[ -n "${SOURCE_DATE_EPOCH:-}" ] || unset SOURCE_DATE_EPOCH

# Build knobs come from the ENVIRONMENT (overridable), with sane defaults — no if-statements,
# just parameter-expansion fallbacks. Default sanitizers are the base's ASan+UBSan (halting); override
# per build via the Dockerfile's `--build-arg SANITIZER_FLAGS="..."`.
# NB: SANITIZER_FLAGS uses `=` (no colon) on purpose — `=` only fills when the var is UNSET, so an
# explicit EMPTY value (`--build-arg SANITIZER_FLAGS=`) is honored and builds with NO sanitizers
# (useful when you want the program's natural crash / full backtrace, not an ASan report). The other
# knobs use `:=` (default on empty too). MAYHEM_JOBS sets build parallelism (falls back to nproc).
: "${SANITIZER_FLAGS=-fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer}"
# DEBUG_FLAGS carries DWARF debug info INDEPENDENTLY of the sanitizer off-switch (so an empty
# SANITIZER_FLAGS still yields DWARF symbols). DWARF MUST be < 4 (Mayhem triage can't read >=4); clang-19's
# plain `-g` emits DWARF-5, so `-gdwarf-3` is explicit. Apply $DEBUG_FLAGS to the fuzz/harness/standalone
# builds (NOT the test/oracle build). Rust/Go carry the same intent via their language flags below.
: "${DEBUG_FLAGS:=-g -gdwarf-3}"
: "${RUST_DEBUG_FLAGS:=-C debuginfo=2 -C force-frame-pointers=yes -C llvm-args=-gdwarf-3}"
: "${GO_DEBUG_FLAGS:=-gcflags=all=-N -l}"
: "${CC:=clang}" ; : "${CXX:=clang++}" ; : "${LIB_FUZZING_ENGINE:=-fsanitize=fuzzer}"
: "${MAYHEM_JOBS:=$(nproc)}"
# COVERAGE_FLAGS: empty by default → no effect on the normal oracle build. Set it via the Dockerfile's
# `--build-arg COVERAGE_FLAGS="-fprofile-instr-generate -fcoverage-mapping"` to instrument the TEST
# build for source-coverage measurement (how much of the project the test suite actually exercises —
# a quality signal for the oracle; complements the anti-reward-hack sabotage check). APPEND it to the
# test build's compile+link flags in step 3 (NOT the fuzz build); after `test.sh` runs, merge with
# `llvm-profdata` and report with `llvm-cov`. Empty value is honored (`=`, not `:=`).
: "${COVERAGE_FLAGS=}"
export SANITIZER_FLAGS DEBUG_FLAGS RUST_DEBUG_FLAGS GO_DEBUG_FLAGS CC CXX LIB_FUZZING_ENGINE MAYHEM_JOBS COVERAGE_FLAGS

cd "$SRC"

# SketchyBar is a macOS-only daemon (Apple frameworks + Objective-C). Its IPC command handler and
# everything it dispatches to are plain C, so the harness builds that C core on Linux against a
# small framework shim (mayhem/shim/include, types + prototypes) whose functions are generated
# no-ops (mayhem/shim/genstubs.py) plus a hand-written environment (mayhem/shim/sb_env.c).
# Not compiled: src/sketchybar.c (main + globals), src/mach.c (mach transport), src/mouse.c
# (Carbon events), src/hotload.c (FSEvents) and the .m files — sb_env.c stands in for them.
OUT="$SRC/mayhem/build"
mkdir -p "$OUT/fuzz" "$OUT/test"
python3 "$SRC/mayhem/shim/genstubs.py" "$SRC" "$OUT/apple_stubs.c"

CORE=()
for f in "$SRC"/src/*.c; do
  case "$(basename "$f")" in
    sketchybar.c|mach.c|mouse.c|hotload.c) ;;
    *) CORE+=("$f") ;;
  esac
done
CORE+=("$OUT/apple_stubs.c" "$SRC/mayhem/shim/sb_env.c")
BASE_CFLAGS=(-std=gnu99 -fblocks -I"$SRC/mayhem/shim/include" -I"$SRC/src" -w)

# compile <objdir> <flags...> : every core source into <objdir>
compile_core() {
  local dir="$1"; shift
  local pids=() f
  for f in "${CORE[@]}"; do
    $CC "${BASE_CFLAGS[@]}" "$@" -c "$f" -o "$dir/$(basename "${f%.c}").o" &
    pids+=($!)
    if [ "${#pids[@]}" -ge "$MAYHEM_JOBS" ]; then wait "${pids[0]}"; pids=("${pids[@]:1}"); fi
  done
  for p in "${pids[@]}"; do wait "$p"; done
}

# 1) the daemon core, sanitized + DWARF 3 (the fuzzed code).
#    -fno-sanitize=vla-bound: animator_cancel() (src/animation.c) declares
#    `struct animation* remove[animator->animation_count]`, a zero-length VLA whenever no animation
#    is running, i.e. on nearly every --set; that benign pattern would end every run. Every other
#    ASan/UBSan check stays on and halting. Only applied when sanitizers are on.
#    -fno-sanitize=nonnull-attribute: pack_key_value_pair() (src/misc/helpers.h) does
#    memcpy(cursor, value, 0) with value == NULL for every `KEY=` env var with an empty value;
#    a zero-byte copy that ended two thirds of all fuzz runs before they reached anything else.
NO_VLA=""
case "$SANITIZER_FLAGS" in *undefined*) NO_VLA="-fno-sanitize=vla-bound,nonnull-attribute" ;; esac
# shellcheck disable=SC2086
compile_core "$OUT/fuzz" $SANITIZER_FLAGS $NO_VLA -fsanitize=fuzzer-no-link $DEBUG_FLAGS -O1
# shellcheck disable=SC2086
$CC $SANITIZER_FLAGS $DEBUG_FLAGS -c "$SRC/mayhem/lsan_off.c" -o "$OUT/lsan_off.o"

# 2) the harness: libFuzzer binary + standalone reproducer
# shellcheck disable=SC2086
$CC "${BASE_CFLAGS[@]}" $SANITIZER_FLAGS $DEBUG_FLAGS $LIB_FUZZING_ENGINE \
    "$SRC/mayhem/fuzz_message.c" "$OUT"/fuzz/*.o "$OUT/lsan_off.o" -lm -lpthread \
    -o "$SRC/fuzz_message"
# shellcheck disable=SC2086
$CC "${BASE_CFLAGS[@]}" $SANITIZER_FLAGS $DEBUG_FLAGS \
    "$STANDALONE_FUZZ_MAIN" "$SRC/mayhem/fuzz_message.c" "$OUT"/fuzz/*.o "$OUT/lsan_off.o" -lm -lpthread \
    -o "$SRC/fuzz_message-standalone"

# 3) the test driver, with normal flags (independent of the sanitized build)
# shellcheck disable=SC2086
compile_core "$OUT/test" -O2 $COVERAGE_FLAGS
# shellcheck disable=SC2086
$CC "${BASE_CFLAGS[@]}" -O2 $COVERAGE_FLAGS "$SRC/mayhem/sb_cli.c" "$OUT"/test/*.o -lm -lpthread \
    -o "$OUT/sb_cli"
echo "built: $SRC/fuzz_message $SRC/fuzz_message-standalone $OUT/sb_cli"
