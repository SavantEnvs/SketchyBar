#!/usr/bin/env bash
#
# mayhem/test.sh — RUN this repo's OWN functional test suite (already built by mayhem/build.sh).
# exit 0 = pass. EDIT per repo. PATCH-grade oracle: after an agent patches the source, the grader
# rebuilds (build.sh) then runs this. DELETE this file if the repo has no meaningful tests.
#
# IMPORTANT:
#  * Must assert BEHAVIOR/OUTPUT, not just exit status. The oracle has to check asserted values /
#    golden-output diffs / known-answer results — so a PATCH that "fixes" a bug by making the program
#    exit(0) (or any no-op) FAILS here. Running inputs and checking only "exit 0 / didn't crash" is
#    NOT a functional test (it's trivially reward-hackable) — use the project's real assertion suite.
#  * Do NOT build here — mayhem/build.sh already compiled the test suite (with the project's normal
#    flags). This script only RUNS the pre-built tests and reports counts. If the test runner is
#    missing, that's a build.sh bug — fail loudly rather than silently rebuilding.
#  * REQUIRED OUTPUT — a CTRF (https://ctrf.io) summary so Mayhem/the PATCH grader reads the counts:
#      - writes a CTRF JSON report to ${CTRF_REPORT:-$SRC/ctrf-report.json}, and
#      - prints a one-line `CTRF {...}` marker to stdout (same JSON, compact).
#    Only `results.summary` (with tests/passed/failed/pending/skipped/other) is required.
#    Use the emit_ctrf helper below; it computes tests = passed+failed+skipped and sets the exit
#    code (0 iff failed==0). Map your framework's output to passed/failed/skipped.
set -uo pipefail
[ -n "${SOURCE_DATE_EPOCH:-}" ] || unset SOURCE_DATE_EPOCH
: "${MAYHEM_JOBS:=$(nproc)}"   # build parallelism; env-overridable, falls back to nproc (use -j"$MAYHEM_JOBS")
cd "$SRC"

# emit_ctrf <tool> <passed> <failed> [skipped] [pending] [other]
# Writes a CTRF report (file + stdout `CTRF {...}` marker) and returns non-zero iff failed>0.
emit_ctrf() {
  local tool="$1" passed="$2" failed="$3" skipped="${4:-0}" pending="${5:-0}" other="${6:-0}"
  local tests=$(( passed + failed + skipped + pending + other ))
  cat > "${CTRF_REPORT:-$SRC/ctrf-report.json}" <<JSON
{
  "results": {
    "tool": { "name": "$tool" },
    "summary": {
      "tests": $tests,
      "passed": $passed,
      "failed": $failed,
      "pending": $pending,
      "skipped": $skipped,
      "other": $other
    }
  }
}
JSON
  printf 'CTRF {"results":{"tool":{"name":"%s"},"summary":{"tests":%d,"passed":%d,"failed":%d,"pending":%d,"skipped":%d,"other":%d}}}\n' \
    "$tool" "$tests" "$passed" "$failed" "$pending" "$skipped" "$other"
  [ "$failed" -eq 0 ]
}

# Upstream ships no test suite, so this is a known-answer suite over the daemon's own command
# interface: mayhem/build/sb_cli (built by build.sh with normal flags) packs a `sketchybar ...`
# command line exactly like the real CLI, runs it through handle_message_mach() and prints the
# daemon's reply. Each case asserts values in that reply (mostly --query JSON), so it checks the
# property parsers, the item model and the JSON writer.
CLI="$SRC/mayhem/build/sb_cli"
if [ ! -x "$CLI" ]; then
  echo "missing $CLI — run mayhem/build.sh first" >&2
  emit_ctrf "sketchybar-known-answer" 0 1 0
  exit 1
fi

passed=0; failed=0
# check <name> <expected line (fixed string, after trimming)>... -- <sketchybar args...>
check() {
  local name="$1"; shift
  local expect=()
  while [ "$1" != "--" ]; do expect+=("$1"); shift; done
  shift
  local out
  out="$("$CLI" "$@" 2>&1 | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//')"
  local e ok=1
  for e in "${expect[@]}"; do
    if ! grep -qxF -- "$e" <<<"$out"; then
      echo "FAIL $name: expected line: $e"; ok=0
    fi
  done
  if [ "$ok" = 1 ]; then echo "ok   $name"; passed=$((passed+1)); else
    echo "---- output:"; head -40 <<<"$out"; failed=$((failed+1)); fi
}

check "item label + icon color" '"name": "foo",' '"value": "hi",' '"color": "0xff00ff00",' '"position": "left",' -- \
  --add item foo left --set foo label=hi icon.color=0xff00ff00 --query foo
check "bar properties" '"position": "bottom",' '"height": 32,' '"color": "0x40000000",' '"blur_radius": 20,' '"margin": 4,' '"corner_radius": 9,' -- \
  --bar height=32 position=bottom color=0x40000000 blur_radius=20 margin=4 corner_radius=9 --query bar
check "defaults apply to new items" '"padding_left": 7,' '"font": "Menlo:Regular:17.00",' -- \
  --default padding_left=7 label.font="Menlo:Regular:17.0" --add item a right --query a
check "item list in --query bar" '"items": [' '"one",' '"two"' -- \
  --add item one left --add item two right --query bar
check "unknown item is reported" "[!] Query: Invalid query, or item 'nope' not found" -- --query nope
check "bracket groups members" '"type": "bracket",' '"bracket": [' -- \
  --add item a left --add item b left --add bracket br a b --query br
check "graph width" '"type": "graph",' -- --add graph g right 50 --query g
check "slider percentage" '"type": "slider",' '"percentage": "40",' '"width": "100",' -- \
  --add slider s right 100 --set s slider.percentage=40 --query s
check "regex set" '"value": "R",' -- --add item xa left --add item xb left --set '/x[ab]/' label=R --query xb
check "rename" '"name": "newname",' -- --add item old left --rename old newname --query newname
check "remove" "[!] Query: Invalid query, or item 'gone' not found" -- --add item gone left --remove gone --query gone
check "custom event" '"my_event": {' -- --add event my_event --query events
check "toggle drawing" '"drawing": "off",' -- --add item t left --set t drawing=toggle --query t
check "label property parsing" '"y_offset": 3,' '"padding_right": 11,' -- \
  --add item l left --set l label.y_offset=3 label.padding_right=11 --query l

emit_ctrf "sketchybar-known-answer" "$passed" "$failed" 0
