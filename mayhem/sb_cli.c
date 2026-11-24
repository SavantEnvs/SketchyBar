/*
 * sb_cli — runs one `sketchybar <args...>` command against the daemon core in-process and prints
 * the daemon's reply. The arguments are packed exactly as the real CLI packs them
 * (client_send_message in src/sketchybar.c: each argument NUL-terminated, plus a final NUL), handed
 * to handle_message_mach() (src/message.c), and the reply the daemon sends back is written to
 * stdout. Used by mayhem/test.sh as a known-input -> known-output oracle.
 */
#include "bar_manager.h"
#include "mach.h"
#include "message.h"

extern char* sb_last_response;

int main(int argc, char** argv) {
  size_t len = 1;
  for (int i = 1; i < argc; i++) len += strlen(argv[i]) + 1;
  char* message = calloc(len + 1, 1);
  char* p = message;
  for (int i = 1; i < argc; i++) {
    size_t l = strlen(argv[i]);
    memcpy(p, argv[i], l);
    p += l + 1;
  }

  bar_manager_init(&g_bar_manager);
  bar_manager_begin(&g_bar_manager);

  struct mach_buffer buffer;
  memset(&buffer, 0, sizeof(buffer));
  buffer.message.header.msgh_remote_port = 1;
  buffer.message.descriptor.address = message;
  buffer.message.descriptor.size = (mach_msg_size_t)len;
  handle_message_mach(&buffer);

  if (sb_last_response) fputs(sb_last_response, stdout);
  bar_manager_destroy(&g_bar_manager);
  free(message);
  return 0;
}
