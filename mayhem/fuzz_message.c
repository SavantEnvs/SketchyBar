/*
 * fuzz_message — SketchyBar's daemon IPC command handler, handle_message_mach() (src/message.c).
 *
 * The `sketchybar --add/--set/--bar/--default/--query/--trigger/...` CLI packs its argv into one
 * buffer (each argument NUL-terminated, the whole message ending in an extra NUL) and sends it to
 * the daemon over a mach port; the daemon hands the out-of-line buffer to handle_message_mach().
 * Any local process can send that message, so it is the daemon's untrusted input. The handler
 * dispatches to every domain and property parser (bar, item, text, font, color, background, image,
 * graph, slider, popup, bracket/group, events, animation, regex item selection) and writes its
 * reply (e.g. --query JSON) back over the port.
 *
 * Input: the raw message bytes. The harness appends the "\0\0" terminator the CLI always writes
 * (client_send_message in src/sketchybar.c), so the wire format is exactly what the daemon sees.
 *
 * State: the daemon's model (g_bar_manager and the subscription flags) is rebuilt for every input
 * — bar_manager_init + bar_manager_begin as main() does, then bar_manager_destroy — so each
 * result depends only on its input.
 *
 * Commands not run: `--exit` (handle_message_mach calls exit(0); a daemon shutdown is the
 * documented behaviour of that command, not a defect). Inputs carrying that token are rejected.
 * Scripts named by the input are never executed (vfork is mocked in mayhem/shim/sb_env.c).
 */
#include "bar_manager.h"
#include "mach.h"
#include "message.h"

extern char* sb_last_response;
extern bool g_volume_events;
extern bool g_brightness_events;
extern bool g_space_window_events;
extern int64_t g_disable_capture;

static bool has_token(const uint8_t* data, size_t size, const char* tok) {
  size_t tl = strlen(tok);
  size_t start = 0;
  for (size_t i = 0; i <= size; i++) {
    if (i == size || data[i] == '\0') {
      if (i - start == tl && memcmp(data + start, tok, tl) == 0) return true;
      start = i + 1;
    }
  }
  return false;
}

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (has_token(data, size, "--exit")) return -1;

  char* message = malloc(size + 2);
  memcpy(message, data, size);
  message[size] = '\0';
  message[size + 1] = '\0';

  g_volume_events = false;
  g_brightness_events = false;
  g_space_window_events = false;
  g_disable_capture = 0;
  memset(&g_bar_manager, 0, sizeof(g_bar_manager));
  bar_manager_init(&g_bar_manager);
  bar_manager_begin(&g_bar_manager);

  struct mach_buffer buffer;
  memset(&buffer, 0, sizeof(buffer));
  buffer.message.header.msgh_remote_port = 1;
  buffer.message.descriptor.address = message;
  buffer.message.descriptor.size = (mach_msg_size_t)(size + 2);
  handle_message_mach(&buffer);

  bar_manager_destroy(&g_bar_manager);
  free(sb_last_response);
  sb_last_response = NULL;
  free(message);
  return 0;
}
