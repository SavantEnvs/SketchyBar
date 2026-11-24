/*
 * sb_env.c — the hand-written part of the Linux environment SketchyBar's daemon core runs in
 * under the harness. Everything here replaces something that, on macOS, comes from the OS or
 * from a file the harness does not compile:
 *   - the daemon globals main() owns (src/sketchybar.c is not compiled: it holds main()),
 *   - the mach IPC transport (src/mach.c): mach_send_message captures the daemon's reply,
 *   - the Objective-C files (workspace.m, media.m, wifi.m, layer.m, display_nsscreen.m),
 *   - src/mouse.c (Carbon event handlers) and src/hotload.c (FSEvents file watcher),
 *   - a display model: one 1920x1080 display, so bars get created and laid out,
 *   - the clang blocks runtime symbols (dispatch_* blocks run synchronously),
 *   - vfork(): item scripts (script=/click_script=, --trigger, --update, --reload) are
 *     fork/exec'd via helpers.h fork_exec(); the mock reports "parent, child started" so no
 *     shell command from the input is ever run.
 * The remaining framework functions are generated no-ops (genstubs.py).
 */
#include "bar_manager.h"
#include "mach.h"
#include "workspace.h"
#include "media.h"
#include "wifi.h"
#include "layer.h"
#include "display_nsscreen.h"
#include "hotload.h"
#include "mouse.h"

/* ---- globals defined by src/sketchybar.c ---- */
CGError (*SBSLSTransactionAddPostDecodeAction)(CFTypeRef, void (^)()) = NULL;
int g_connection = 1;
CFTypeRef g_transaction;
int g_space_management_mode;
struct bar_manager g_bar_manager;
struct mach_server g_mach_server;
void* g_workspace_context;
char g_name[256] = "sketchybar";
char g_config_file[4096];
char g_lock_file[MAXLEN];
bool g_volume_events;
bool g_brightness_events;
int64_t g_disable_capture = 0;
pid_t g_pid = 0;

/* ---- framework constants ---- */
const CFAllocatorRef kCFAllocatorDefault = NULL;
const CFArrayCallBacks kCFTypeArrayCallBacks;
const CFDictionaryKeyCallBacks kCFTypeDictionaryKeyCallBacks;
const CFDictionaryValueCallBacks kCFTypeDictionaryValueCallBacks;
const CFBooleanRef kCFBooleanTrue = (CFBooleanRef)"true";
const CFBooleanRef kCFBooleanFalse = (CFBooleanRef)"false";
const CFRunLoopMode kCFRunLoopDefaultMode = (CFRunLoopMode)"kCFRunLoopDefaultMode";
const CFRunLoopMode kCFRunLoopCommonModes = (CFRunLoopMode)"kCFRunLoopCommonModes";
const CGRect CGRectNull = {{INFINITY, INFINITY}, {0, 0}};
const CGRect CGRectZero = {{0, 0}, {0, 0}};
const CGPoint CGPointZero = {0, 0};
const CGSize CGSizeZero = {0, 0};
const CFStringRef kCGWindowOwnerPID = (CFStringRef)"kCGWindowOwnerPID";
const CFStringRef kCGWindowOwnerName = (CFStringRef)"kCGWindowOwnerName";
const CFStringRef kCGWindowNumber = (CFStringRef)"kCGWindowNumber";
const CFStringRef kCGWindowName = (CFStringRef)"kCGWindowName";
const CFStringRef kCGWindowLayer = (CFStringRef)"kCGWindowLayer";
const CFStringRef kCGWindowBounds = (CFStringRef)"kCGWindowBounds";
const CFStringRef kCTForegroundColorFromContextAttributeName = (CFStringRef)"kCTForegroundColorFromContext";
const CFStringRef kCTFontStyleNameAttribute = (CFStringRef)"kCTFontStyleName";
const CFStringRef kCTFontSizeAttribute = (CFStringRef)"kCTFontSize";
const CFStringRef kCTFontOpenTypeFeatureValue = (CFStringRef)"kCTFontOpenTypeFeatureValue";
const CFStringRef kCTFontOpenTypeFeatureTag = (CFStringRef)"kCTFontOpenTypeFeatureTag";
const CFStringRef kCTFontFeatureTypeIdentifierKey = (CFStringRef)"kCTFontFeatureTypeIdentifier";
const CFStringRef kCTFontFeatureSettingsAttribute = (CFStringRef)"kCTFontFeatureSettings";
const CFStringRef kCTFontFeatureSelectorIdentifierKey = (CFStringRef)"kCTFontFeatureSelectorIdentifier";
const CFStringRef kCTFontFamilyNameAttribute = (CFStringRef)"kCTFontFamilyName";
const CFStringRef kCTFontAttributeName = (CFStringRef)"kCTFont";

CFStringRef shim_cfstr(const char* s) { return (CFStringRef)s; }

/* ---- clang blocks runtime (blocks are only ever invoked synchronously here) ---- */
void* _NSConcreteStackBlock[32];
void* _NSConcreteGlobalBlock[32];

/* ---- process environment ---- */
int pthread_main_np(void) { return 1; }
pid_t vfork(void) { return 1; }

/* ---- one display ---- */
CGError CGGetActiveDisplayList(uint32_t max, CGDirectDisplayID* displays, uint32_t* count) {
  if (displays && max > 0) displays[0] = 1;
  if (count) *count = (displays && max == 0) ? 0 : 1;
  return kCGErrorSuccess;
}
CGRect CGDisplayBounds(CGDirectDisplayID did) { return CGRectMake(0, 0, 1920, 1080); }
CGDirectDisplayID CGMainDisplayID(void) { return 1; }
size_t CGDisplayPixelsWide(CGDirectDisplayID did) { return 1920; }
size_t CGDisplayPixelsHigh(CGDirectDisplayID did) { return 1080; }

/* ---- mach transport: the daemon's reply to the message being handled ---- */
#define SB_REPLY_PORT 1
char* sb_last_response = NULL;
char* mach_send_message(mach_port_t port, char* message, uint32_t len, bool await_response) {
  if (port == SB_REPLY_PORT && message) {
    free(sb_last_response);
    sb_last_response = malloc(len + 1);
    memcpy(sb_last_response, message, len);
    sb_last_response[len] = '\0';
  }
  return NULL;
}
mach_port_t mach_get_bs_port(char* bs_name) { return 0; }
bool mach_server_begin(struct mach_server* mach_server, mach_handler handler) { return true; }

/* ---- Objective-C sources (not compiled) ---- */
void workspace_create_custom_observer(void** context, char* notification) { }
void workspace_event_handler_init(void** context) { }
void workspace_event_handler_begin(void** context) { }
void workspace_event_handler_end(void* context) { }
int workspace_display_notch_height(uint32_t did) { return 0; }
float workspace_get_scale() { return 1.0f; }
CGImageRef workspace_icon_for_app(char* app) { return NULL; }
char* workspace_copy_app_name_for_pid(pid_t pid) { return NULL; }
void forced_front_app_event() { }
void initialize_media_events() { }
void begin_receiving_media_events() { }
void forced_media_change_event() { }
void forced_network_event() { }
void begin_receiving_network_events() { }
struct layer* layer_create(uint32_t cid, CGRect bounds) { return NULL; }
void layer_destroy(struct layer* layer) { }
uint32_t layer_get_context_id(struct layer* layer) { return 0; }
void layer_set_bounds(struct layer* layer, CGRect bounds) { }
void layer_set_contents(struct layer* layer, CGImageRef image) { }
int display_nsscreen_top_inset(uint32_t did) { return 0; }

/* ---- src/mouse.c and src/hotload.c (not compiled) ---- */
void mouse_begin(void) { }
static int g_hotload_state;
void exec_config_file() { }
void begin_receiving_config_change_events() { }
void hotload_set_state(int state) { g_hotload_state = state; }
int hotload_get_state() { return g_hotload_state; }
bool set_config_file_path(char* file) { return false; }

/* ---- SkyLight windows/surfaces: creation succeeds and hands out non-zero ids, as on macOS ---- */
static uint32_t g_next_id = 100;
static char g_dummy_object[64];
CGError SLSNewWindowWithOpaqueShapeAndContext(int cid, int type, CFTypeRef region, CFTypeRef opaque_shape, int options, uint64_t* tags, float x, float y, int tag_size, uint32_t* wid, void* context) {
  if (wid) *wid = g_next_id++;
  return kCGErrorSuccess;
}
CGError SLSNewWindow(int cid, int type, float x, float y, CFTypeRef region, uint64_t* wid) {
  if (wid) *wid = g_next_id++;
  return kCGErrorSuccess;
}
CGError SLSAddSurface(int cid, uint32_t wid, uint32_t* outSID) {
  if (outSID) *outSID = g_next_id++;
  return kCGErrorSuccess;
}
CGError CGSNewRegionWithRect(CGRect* rect, CFTypeRef* outRegion) {
  if (outRegion) *outRegion = g_dummy_object;
  return kCGErrorSuccess;
}
CFTypeRef CGRegionCreateEmptyRegion(void) { return g_dummy_object; }
CGContextRef SLWindowContextCreate(int cid, uint32_t wid, CFDictionaryRef options) { return (CGContextRef)g_dummy_object; }
CFTypeRef SLSTransactionCreate(int cid) { return g_dummy_object; }
CGMutablePathRef CGPathCreateMutable(void) { return (CGMutablePathRef)g_dummy_object; }
CGError SLSGetWindowOwner(int cid, uint32_t wid, int* out_cid) { if (out_cid) *out_cid = 0; return kCGErrorSuccess; }
CGError SLSConnectionGetPID(int cid, pid_t* pid) { if (pid) *pid = 0; return kCGErrorSuccess; }
CGError SLSGetCurrentCursorLocation(int cid, CGPoint* point) { if (point) *point = CGPointZero; return kCGErrorSuccess; }
CGError SLSGetMenuBarAutohideEnabled(int cid, int* enabled) { if (enabled) *enabled = 0; return kCGErrorSuccess; }
CGError SLSGetRevealedMenuBarBounds(CGRect* rect, int cid, uint64_t sid) { if (rect) *rect = CGRectZero; return kCGErrorSuccess; }
CGError SLSGetDisplayMenubarHeight(uint32_t did, uint32_t* height) { if (height) *height = 24; return kCGErrorSuccess; }
int SLSGetScreenRectForWindow(uint32_t cid, uint32_t wid, CGRect* out) { if (out) *out = CGRectZero; return 0; }
void SLSCaptureWindowsContentsToRectWithOptions(uint32_t cid, uint64_t* wid, bool meh, CGRect bounds, uint32_t flags, CGImageRef* image) { if (image) *image = NULL; }
