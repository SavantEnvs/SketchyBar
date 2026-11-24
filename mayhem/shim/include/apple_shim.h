/*
 * apple_shim.h — minimal Linux stand-ins for the macOS frameworks SketchyBar includes
 * (CoreFoundation, CoreGraphics, CoreText, CoreVideo, CoreAudio, IOKit, mach, bootstrap).
 *
 * SketchyBar is a macOS-only daemon. Its untrusted input is the IPC command message that
 * the `sketchybar` CLI sends to the daemon (handle_message_mach in src/message.c). That
 * parser and everything it dispatches to (bar/item/text/font/color/graph/... property
 * parsers, the item model, the query JSON writer) is plain C; only drawing and system
 * integration call into Apple frameworks. This header supplies the types and prototypes so
 * that plain C compiles on Linux; the functions themselves are no-op stubs defined in
 * mayhem/shim/apple_stubs.c (generated from the PROTOTYPES below by mayhem/shim/genstubs.sh).
 *
 * Only the prototype lines between the BEGIN/END PROTOTYPES markers are fed to the
 * generator: one declaration per line, `RET NAME(ARGS);`.
 */
#pragma once
#define _GNU_SOURCE 1
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <assert.h>
#include <stdarg.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <dirent.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define __builtin_available(...) 1
#define API_AVAILABLE(...)
#define CF_ENUM(t, n) t n; enum
#define CF_RETURNS_RETAINED

/* ---- CoreFoundation ---- */
typedef const void* CFTypeRef;
typedef const struct __CFString* CFStringRef;
typedef struct __CFString* CFMutableStringRef;
typedef const struct __CFArray* CFArrayRef;
typedef struct __CFArray* CFMutableArrayRef;
typedef const struct __CFDictionary* CFDictionaryRef;
typedef struct __CFDictionary* CFMutableDictionaryRef;
typedef const struct __CFNumber* CFNumberRef;
typedef const struct __CFBoolean* CFBooleanRef;
typedef const struct __CFData* CFDataRef;
typedef const struct __CFUUID* CFUUIDRef;
typedef const struct __CFURL* CFURLRef;
typedef const struct __CFAttributedString* CFAttributedStringRef;
typedef const struct __CFAllocator* CFAllocatorRef;
typedef struct __CFRunLoop* CFRunLoopRef;
typedef struct __CFRunLoopSource* CFRunLoopSourceRef;
typedef struct __CFRunLoopTimer* CFRunLoopTimerRef;
typedef struct __CFMachPort* CFMachPortRef;
typedef CFStringRef CFRunLoopMode;
typedef long CFIndex;
typedef unsigned long CFOptionFlags;
typedef unsigned long CFHashCode;
typedef unsigned int CFStringEncoding;
typedef double CFTimeInterval;
typedef double CFAbsoluteTime;
typedef unsigned char Boolean;
typedef int32_t OSStatus;
typedef CFIndex CFComparisonResult;
typedef CFIndex CFNumberType;
typedef void (*CFRunLoopTimerCallBack)(CFRunLoopTimerRef timer, void* info);
typedef void (*CFMachPortCallBack)(CFMachPortRef port, void* msg, CFIndex size, void* info);
typedef struct { CFIndex version; void* info; const void* (*retain)(const void*);
                 void (*release)(const void*); CFStringRef (*copyDescription)(const void*); } CFRunLoopTimerContext;
typedef CFRunLoopTimerContext CFMachPortContext;
typedef struct { CFIndex version; void* a; void* b; void* c; void* d; } CFArrayCallBacks;
typedef struct { CFIndex version; void* a; void* b; void* c; void* d; void* e; } CFDictionaryKeyCallBacks;
typedef struct { CFIndex version; void* a; void* b; void* c; void* d; } CFDictionaryValueCallBacks;

enum { kCFStringEncodingUTF8 = 0x08000100, kCFStringEncodingASCII = 0x0600 };
enum { kCFNumberSInt32Type = 3, kCFNumberSInt64Type = 4, kCFNumberFloat32Type = 5,
       kCFNumberFloat64Type = 6, kCFNumberIntType = 9, kCFNumberCFIndexType = 14,
       kCFNumberDoubleType = 13, kCFNumberFloatType = 12 };
enum { kCFCompareEqualTo = 0, kCFCompareLessThan = -1, kCFCompareGreaterThan = 1 };
enum { kCFCompareCaseInsensitive = 1 };

extern const CFAllocatorRef kCFAllocatorDefault;
extern const CFArrayCallBacks kCFTypeArrayCallBacks;
extern const CFDictionaryKeyCallBacks kCFTypeDictionaryKeyCallBacks;
extern const CFDictionaryValueCallBacks kCFTypeDictionaryValueCallBacks;
extern const CFBooleanRef kCFBooleanTrue;
extern const CFBooleanRef kCFBooleanFalse;
extern const CFRunLoopMode kCFRunLoopDefaultMode;
extern const CFRunLoopMode kCFRunLoopCommonModes;

CFStringRef shim_cfstr(const char* s);
#define CFSTR(s) shim_cfstr(s)

/* ---- CoreGraphics ---- */
typedef double CGFloat;
typedef struct CGPoint { CGFloat x, y; } CGPoint;
typedef struct CGSize { CGFloat width, height; } CGSize;
typedef struct CGRect { CGPoint origin; CGSize size; } CGRect;
typedef struct CGAffineTransform { CGFloat a, b, c, d, tx, ty; } CGAffineTransform;
typedef int32_t CGError;
typedef uint32_t CGDirectDisplayID;
typedef uint32_t CGWindowID;
typedef int32_t CGWindowLevel;
typedef uint32_t CGDisplayChangeSummaryFlags;
typedef uint32_t CGEventType;
typedef uint64_t CGEventFlags;
typedef uint32_t CGEventField;
typedef uint32_t CGMouseButton;
typedef uint32_t CGBitmapInfo;
typedef uint32_t CGWindowListOption;
typedef uint32_t CGWindowImageOption;
typedef int32_t CGInterpolationQuality;
typedef int32_t CGBlendMode;
typedef int32_t CGPathDrawingMode;
typedef int32_t CGColorRenderingIntent;
typedef struct CGContext* CGContextRef;
typedef struct CGImage* CGImageRef;
typedef struct CGPath* CGMutablePathRef;
typedef const struct CGPath* CGPathRef;
typedef struct CGColorSpace* CGColorSpaceRef;
typedef struct CGColor* CGColorRef;
typedef struct CGDataProvider* CGDataProviderRef;
typedef struct __CGEvent* CGEventRef;
typedef struct __CGEventSource* CGEventSourceRef;
typedef void (*CGDisplayReconfigurationCallBack)(CGDirectDisplayID display, CGDisplayChangeSummaryFlags flags, void* userInfo);

extern const CGRect CGRectNull;
extern const CGRect CGRectZero;
extern const CGPoint CGPointZero;
extern const CGSize CGSizeZero;

enum { kCGErrorSuccess = 0, kCGErrorFailure = 1000 };
enum { kCGNullWindowID = 0 };
enum { kCGPathFill = 0, kCGPathEOFill, kCGPathStroke, kCGPathFillStroke, kCGPathEOFillStroke };
enum { kCGInterpolationDefault = 0, kCGInterpolationNone = 1, kCGInterpolationLow = 2,
       kCGInterpolationMedium = 4, kCGInterpolationHigh = 3 };
enum { kCGBlendModeNormal = 0, kCGBlendModeDestinationOut = 23 };
enum { kCGRenderingIntentDefault = 0 };
enum { kCGImageAlphaPremultipliedFirst = 2, kCGBitmapByteOrder32Host = 0 };
enum { kCGWindowListOptionAll = 0, kCGWindowListOptionOnScreenOnly = 1 };
enum { kCGWindowImageDefault = 0 };
enum { kCGBackingStoreBuffered = 2 };
enum { kCGDisplayMovedFlag = 1 << 1, kCGDisplayAddFlag = 1 << 4, kCGDisplayRemoveFlag = 1 << 5,
       kCGDisplayDesktopShapeChangedFlag = 1 << 12 };
enum { kCGEventLeftMouseDown = 1, kCGEventLeftMouseUp = 2, kCGEventRightMouseDown = 3,
       kCGEventRightMouseUp = 4, kCGEventMouseMoved = 5, kCGEventLeftMouseDragged = 6,
       kCGEventScrollWheel = 22, kCGEventOtherMouseDown = 25, kCGEventOtherMouseUp = 26 };
enum { kCGEventFlagMaskShift = 0x20000, kCGEventFlagMaskControl = 0x40000,
       kCGEventFlagMaskAlternate = 0x80000, kCGEventFlagMaskCommand = 0x100000,
       kCGEventFlagMaskSecondaryFn = 0x800000 };
enum { kCGMouseEventButtonNumber = 3, kCGScrollWheelEventDeltaAxis1 = 11 };
enum { kCGMouseButtonLeft = 0, kCGMouseButtonRight = 1, kCGMouseButtonCenter = 2 };
enum { kCGBackstopMenuLevel = -20, kCGNormalWindowLevel = 0, kCGFloatingWindowLevel = 3,
       kCGStatusWindowLevel = 25, kCGPopUpMenuWindowLevel = 101 };
#define kCGSPreventsActivationTagBit (1ULL << 16)
#define kCGSExposeFadeTagBit (1ULL << 1)
extern const CFStringRef kCGWindowOwnerPID;
extern const CFStringRef kCGWindowOwnerName;
extern const CFStringRef kCGWindowNumber;
extern const CFStringRef kCGWindowName;
extern const CFStringRef kCGWindowLayer;
extern const CFStringRef kCGWindowBounds;

/* ---- CoreText ---- */
typedef const struct __CTFont* CTFontRef;
typedef const struct __CTFontDescriptor* CTFontDescriptorRef;
typedef const struct __CTLine* CTLineRef;
typedef uint32_t CTLineBoundsOptions;
typedef uint32_t CTFontManagerScope;
enum { kCTLineBoundsUseGlyphPathBounds = 1 << 3 };
enum { kCTFontManagerScopeProcess = 1 };
extern const CFStringRef kCTForegroundColorFromContextAttributeName;
extern const CFStringRef kCTFontStyleNameAttribute;
extern const CFStringRef kCTFontSizeAttribute;
extern const CFStringRef kCTFontOpenTypeFeatureValue;
extern const CFStringRef kCTFontOpenTypeFeatureTag;
extern const CFStringRef kCTFontFeatureTypeIdentifierKey;
extern const CFStringRef kCTFontFeatureSettingsAttribute;
extern const CFStringRef kCTFontFeatureSelectorIdentifierKey;
extern const CFStringRef kCTFontFamilyNameAttribute;
extern const CFStringRef kCTFontAttributeName;

/* ---- CoreVideo ---- */
typedef struct __CVDisplayLink* CVDisplayLinkRef;
typedef int32_t CVReturn;
typedef uint64_t CVOptionFlags;
typedef struct { uint32_t version; int32_t videoTimeScale; int64_t videoTime; uint64_t hostTime;
                 double rateScalar; int64_t videoRefreshPeriod; char smpte[24]; uint64_t flags;
                 uint64_t reserved; } CVTimeStamp;
typedef CVReturn (*CVDisplayLinkOutputCallback)(CVDisplayLinkRef, const CVTimeStamp*, const CVTimeStamp*,
                                                CVOptionFlags, CVOptionFlags*, void*);

/* ---- CoreAudio ---- */
typedef uint32_t AudioObjectID;
typedef uint32_t AudioObjectPropertySelector;
typedef uint32_t AudioObjectPropertyScope;
typedef uint32_t AudioObjectPropertyElement;
typedef struct { AudioObjectPropertySelector mSelector; AudioObjectPropertyScope mScope;
                 AudioObjectPropertyElement mElement; } AudioObjectPropertyAddress;
typedef OSStatus (*AudioObjectPropertyListenerProc)(AudioObjectID, uint32_t, const AudioObjectPropertyAddress*, void*);
enum { kAudioObjectSystemObject = 1, kAudioObjectPropertyScopeGlobal = 0x676c6f62,
       kAudioObjectPropertyScopeOutput = 0x6f757470, kAudioObjectPropertyElementMain = 0,
       kAudioObjectPropertyElementMaster = 0, kAudioHardwarePropertyDefaultOutputDevice = 0x644f7574,
       kAudioDevicePropertyVolumeScalar = 0x766f6c6d, kAudioDevicePropertyMute = 0x6d757465 };

/* ---- IOKit power sources ---- */
#define kIOPMUPSPowerKey "UPS Power"
#define kIOPMBatteryPowerKey "Battery Power"
#define kIOPMACPowerKey "AC Power"
typedef void (*IOPowerSourceCallbackType)(void* context);

/* ---- mach / bootstrap ---- */
typedef uint32_t mach_port_t;
typedef uint32_t mach_port_name_t;
typedef int kern_return_t;
typedef int mach_msg_return_t;
typedef uint32_t mach_msg_size_t;
typedef uint32_t mach_msg_bits_t;
typedef int32_t mach_msg_id_t;
typedef uint32_t mach_msg_option_t;
typedef uint32_t mach_msg_timeout_t;
typedef uint32_t mach_port_right_t;
typedef int mach_port_flavor_t;
typedef int* mach_port_info_t;
typedef uint32_t mach_msg_type_number_t;
typedef mach_port_t task_t;
typedef struct { mach_msg_bits_t msgh_bits; mach_msg_size_t msgh_size; mach_port_t msgh_remote_port;
                 mach_port_t msgh_local_port; mach_port_name_t msgh_voucher_port; mach_msg_id_t msgh_id; } mach_msg_header_t;
typedef struct { void* address; uint32_t deallocate : 8, copy : 8, pad1 : 8, type : 8;
                 mach_msg_size_t size; } mach_msg_ool_descriptor_t;
typedef struct { uint32_t msgh_trailer_type; mach_msg_size_t msgh_trailer_size; } mach_msg_trailer_t;
typedef struct mach_port_limits { uint32_t mpl_qlimit; } mach_port_limits_t;
#define KERN_SUCCESS 0
#define MACH_MSG_SUCCESS 0
#define MACH_PORT_NULL 0
#define MACH_PORT_RIGHT_RECEIVE 1
#define MACH_MSG_TYPE_MAKE_SEND 20
#define MACH_MSG_TYPE_COPY_SEND 19
#define MACH_MSGH_BITS_COMPLEX 0x80000000U
#define MACH_MSGH_BITS_REMOTE_MASK 0x1fU
#define MACH_MSGH_BITS_SET(r, l, v, o) ((r) | ((l) << 8) | ((v) << 16) | (o))
#define MACH_MSG_OOL_DESCRIPTOR 1
#define MACH_MSG_VIRTUAL_COPY 1
#define MACH_MSG_TIMEOUT_NONE 0
#define MACH_SEND_MSG 1
#define MACH_RCV_MSG 2
#define MACH_SEND_TIMEOUT 0x10
#define MACH_RCV_TIMEOUT 0x100
#define MACH_PORT_LIMITS_INFO 1
#define MACH_PORT_LIMITS_INFO_COUNT 1
#define MACH_PORT_QLIMIT_LARGE 1024
#define TASK_BOOTSTRAP_PORT 4

/* ---- libdispatch (only the bits SketchyBar touches; blocks run synchronously) ---- */
typedef void* dispatch_queue_t;
#define dispatch_get_main_queue() ((dispatch_queue_t)0)
#define dispatch_get_global_queue(a, b) ((dispatch_queue_t)0)
#define DISPATCH_QUEUE_PRIORITY_DEFAULT 0
#define DISPATCH_QUEUE_PRIORITY_HIGH 2
static inline void dispatch_async(dispatch_queue_t q, void (^block)(void)) { (void)q; block(); }
static inline void dispatch_sync(dispatch_queue_t q, void (^block)(void)) { (void)q; block(); }

static inline uint64_t clock_gettime_nsec_np(int clk) {
  struct timespec ts; clock_gettime(clk, &ts);
  return (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec;
}
#define CLOCK_MONOTONIC_RAW_APPROX CLOCK_MONOTONIC
int pthread_main_np(void);
enum { kCVReturnSuccess = 0 };
#ifndef CLOCK_UPTIME_RAW
#define CLOCK_UPTIME_RAW CLOCK_MONOTONIC
#endif

static inline CGRect CGRectMake(CGFloat x, CGFloat y, CGFloat w, CGFloat h) {
  CGRect r = {{x, y}, {w, h}}; return r;
}
static inline CGPoint CGPointMake(CGFloat x, CGFloat y) { CGPoint p = {x, y}; return p; }
static inline CGSize CGSizeMake(CGFloat w, CGFloat h) { CGSize s = {w, h}; return s; }
static inline bool CGPointEqualToPoint(CGPoint a, CGPoint b) { return a.x == b.x && a.y == b.y; }
static inline bool CGSizeEqualToSize(CGSize a, CGSize b) { return a.width == b.width && a.height == b.height; }
static inline bool CGRectEqualToRect(CGRect a, CGRect b) {
  return CGPointEqualToPoint(a.origin, b.origin) && CGSizeEqualToSize(a.size, b.size);
}
static inline CGFloat CGRectGetMaxY(CGRect r) { return r.origin.y + r.size.height; }
static inline CGFloat CGRectGetMaxX(CGRect r) { return r.origin.x + r.size.width; }
static inline CGFloat CGRectGetMinX(CGRect r) { return r.origin.x; }
static inline CGFloat CGRectGetMinY(CGRect r) { return r.origin.y; }
static inline CGFloat CGRectGetWidth(CGRect r) { return r.size.width; }
static inline CGFloat CGRectGetHeight(CGRect r) { return r.size.height; }
static inline bool CGRectContainsPoint(CGRect r, CGPoint p) {
  return p.x >= r.origin.x && p.x < r.origin.x + r.size.width
      && p.y >= r.origin.y && p.y < r.origin.y + r.size.height;
}
static inline CGRect CGRectInset(CGRect r, CGFloat dx, CGFloat dy) {
  return CGRectMake(r.origin.x + dx, r.origin.y + dy, r.size.width - 2 * dx, r.size.height - 2 * dy);
}

/* BEGIN PROTOTYPES */
void CFRelease(CFTypeRef cf);
CFTypeRef CFRetain(CFTypeRef cf);
Boolean CFEqual(CFTypeRef a, CFTypeRef b);
CFIndex CFArrayGetCount(CFArrayRef a);
const void* CFArrayGetValueAtIndex(CFArrayRef a, CFIndex i);
CFArrayRef CFArrayCreate(CFAllocatorRef al, const void** values, CFIndex n, const CFArrayCallBacks* cb);
CFMutableArrayRef CFArrayCreateMutable(CFAllocatorRef al, CFIndex cap, const CFArrayCallBacks* cb);
void CFArrayAppendValue(CFMutableArrayRef a, const void* v);
const void* CFDictionaryGetValue(CFDictionaryRef d, const void* key);
CFDictionaryRef CFDictionaryCreate(CFAllocatorRef al, const void** keys, const void** values, CFIndex n, const CFDictionaryKeyCallBacks* kcb, const CFDictionaryValueCallBacks* vcb);
CFNumberRef CFNumberCreate(CFAllocatorRef al, CFNumberType t, const void* value);
Boolean CFNumberGetValue(CFNumberRef n, CFNumberType t, void* value);
CFNumberType CFNumberGetType(CFNumberRef n);
CFStringRef CFStringCreateWithCString(CFAllocatorRef al, const char* s, CFStringEncoding enc);
CFIndex CFStringGetLength(CFStringRef s);
CFIndex CFStringGetMaximumSizeForEncoding(CFIndex len, CFStringEncoding enc);
Boolean CFStringGetCString(CFStringRef s, char* buf, CFIndex size, CFStringEncoding enc);
CFComparisonResult CFStringCompare(CFStringRef a, CFStringRef b, CFOptionFlags opts);
CFAttributedStringRef CFAttributedStringCreate(CFAllocatorRef al, CFStringRef s, CFDictionaryRef attrs);
CFIndex CFDataGetLength(CFDataRef d);
const uint8_t* CFDataGetBytePtr(CFDataRef d);
CFUUIDRef CFUUIDCreateFromString(CFAllocatorRef al, CFStringRef s);
CFStringRef CFUUIDCreateString(CFAllocatorRef al, CFUUIDRef u);
CFURLRef CFURLCreateWithString(CFAllocatorRef al, CFStringRef s, CFURLRef base);
CFRunLoopRef CFRunLoopGetMain(void);
CFRunLoopRef CFRunLoopGetCurrent(void);
void CFRunLoopAddSource(CFRunLoopRef rl, CFRunLoopSourceRef src, CFRunLoopMode mode);
void CFRunLoopAddTimer(CFRunLoopRef rl, CFRunLoopTimerRef t, CFRunLoopMode mode);
void CFRunLoopRemoveTimer(CFRunLoopRef rl, CFRunLoopTimerRef t, CFRunLoopMode mode);
CFRunLoopTimerRef CFRunLoopTimerCreate(CFAllocatorRef al, CFAbsoluteTime fire, CFTimeInterval interval, CFOptionFlags flags, CFIndex order, CFRunLoopTimerCallBack cb, CFRunLoopTimerContext* ctx);
void CFRunLoopTimerInvalidate(CFRunLoopTimerRef t);
CFAbsoluteTime CFAbsoluteTimeGetCurrent(void);
CFMachPortRef CFMachPortCreateWithPort(CFAllocatorRef al, mach_port_t port, CFMachPortCallBack cb, CFMachPortContext* ctx, Boolean* shouldFree);
CFRunLoopSourceRef CFMachPortCreateRunLoopSource(CFAllocatorRef al, CFMachPortRef port, CFIndex order);
void CGContextSetRGBFillColor(CGContextRef c, CGFloat r, CGFloat g, CGFloat b, CGFloat a);
void CGContextSetRGBStrokeColor(CGContextRef c, CGFloat r, CGFloat g, CGFloat b, CGFloat a);
void CGContextAddPath(CGContextRef c, CGPathRef p);
void CGContextSaveGState(CGContextRef c);
void CGContextRestoreGState(CGContextRef c);
void CGContextSetInterpolationQuality(CGContextRef c, CGInterpolationQuality q);
void CGContextFlush(CGContextRef c);
void CGContextDrawPath(CGContextRef c, CGPathDrawingMode m);
void CGContextClearRect(CGContextRef c, CGRect r);
void CGContextSetLineWidth(CGContextRef c, CGFloat w);
void CGContextRelease(CGContextRef c);
void CGContextSetTextPosition(CGContextRef c, CGFloat x, CGFloat y);
void CGContextSetBlendMode(CGContextRef c, CGBlendMode m);
void CGContextClip(CGContextRef c);
void CGContextStrokePath(CGContextRef c);
void CGContextSetAllowsFontSmoothing(CGContextRef c, bool b);
void CGContextScaleCTM(CGContextRef c, CGFloat sx, CGFloat sy);
void CGContextFillRect(CGContextRef c, CGRect r);
void CGContextFillPath(CGContextRef c);
void CGContextDrawImage(CGContextRef c, CGRect r, CGImageRef img);
void CGContextClipToMask(CGContextRef c, CGRect r, CGImageRef mask);
void CGContextTranslateCTM(CGContextRef c, CGFloat tx, CGFloat ty);
void CGContextRotateCTM(CGContextRef c, CGFloat angle);
void CGContextSetShouldAntialias(CGContextRef c, bool b);
void CGContextMoveToPoint(CGContextRef c, CGFloat x, CGFloat y);
void CGContextAddLineToPoint(CGContextRef c, CGFloat x, CGFloat y);
void CGContextBeginPath(CGContextRef c);
void CGContextClosePath(CGContextRef c);
void CGContextSetLineCap(CGContextRef c, int cap);
void CGContextSetLineJoin(CGContextRef c, int join);
CGImageRef CGBitmapContextCreateImage(CGContextRef c);
CGContextRef CGBitmapContextCreate(void* data, size_t w, size_t h, size_t bpc, size_t bpr, CGColorSpaceRef cs, uint32_t info);
CGMutablePathRef CGPathCreateMutable(void);
void CGPathAddRoundedRect(CGMutablePathRef p, const CGAffineTransform* m, CGRect r, CGFloat cw, CGFloat ch);
void CGPathAddLineToPoint(CGMutablePathRef p, const CGAffineTransform* m, CGFloat x, CGFloat y);
void CGPathMoveToPoint(CGMutablePathRef p, const CGAffineTransform* m, CGFloat x, CGFloat y);
void CGPathCloseSubpath(CGMutablePathRef p);
void CGPathAddRect(CGMutablePathRef p, const CGAffineTransform* m, CGRect r);
void CGPathRelease(CGPathRef p);
void CGImageRelease(CGImageRef img);
size_t CGImageGetWidth(CGImageRef img);
size_t CGImageGetHeight(CGImageRef img);
CGDataProviderRef CGImageGetDataProvider(CGImageRef img);
CGImageRef CGImageCreateWithPNGDataProvider(CGDataProviderRef p, const CGFloat* decode, bool interp, CGColorRenderingIntent intent);
CGImageRef CGImageCreateWithJPEGDataProvider(CGDataProviderRef p, const CGFloat* decode, bool interp, CGColorRenderingIntent intent);
CGImageRef CGImageCreateCopy(CGImageRef img);
CGDataProviderRef CGDataProviderCreateWithFilename(const char* filename);
CFDataRef CGDataProviderCopyData(CGDataProviderRef p);
void CGDataProviderRelease(CGDataProviderRef p);
CGColorSpaceRef CGColorSpaceCreateDeviceRGB(void);
void CGColorSpaceRelease(CGColorSpaceRef cs);
CGPoint CGEventGetLocation(CGEventRef e);
int64_t CGEventGetIntegerValueField(CGEventRef e, CGEventField f);
CGEventFlags CGEventGetFlags(CGEventRef e);
CGEventType CGEventGetType(CGEventRef e);
CGEventRef CGEventCreate(CGEventSourceRef src);
CGError CGGetActiveDisplayList(uint32_t max, CGDirectDisplayID* displays, uint32_t* count);
CGDirectDisplayID CGDisplayGetDisplayIDFromUUID(CFUUIDRef uuid);
CGRect CGDisplayBounds(CGDirectDisplayID did);
size_t CGDisplayPixelsWide(CGDirectDisplayID did);
size_t CGDisplayPixelsHigh(CGDirectDisplayID did);
CGDirectDisplayID CGMainDisplayID(void);
CGError CGDisplayRegisterReconfigurationCallback(CGDisplayReconfigurationCallBack cb, void* ctx);
CGError CGDisplayRemoveReconfigurationCallback(CGDisplayReconfigurationCallBack cb, void* ctx);
bool CGRequestScreenCaptureAccess(void);
bool CGPreflightScreenCaptureAccess(void);
CGImageRef CGWindowListCreateImage(CGRect r, CGWindowListOption o, CGWindowID w, CGWindowImageOption io);
CFArrayRef CGWindowListCopyWindowInfo(CGWindowListOption o, CGWindowID w);
CGError CGSetLocalEventsSuppressionInterval(CFTimeInterval s);
CGError CGEnableEventStateCombining(bool b);
bool CGRectMakeWithDictionaryRepresentation(CFDictionaryRef d, CGRect* r);
CTLineRef CTLineCreateWithAttributedString(CFAttributedStringRef s);
CGRect CTLineGetBoundsWithOptions(CTLineRef l, CTLineBoundsOptions o);
double CTLineGetTypographicBounds(CTLineRef l, CGFloat* ascent, CGFloat* descent, CGFloat* leading);
void CTLineDraw(CTLineRef l, CGContextRef c);
CTFontDescriptorRef CTFontDescriptorCreateWithAttributes(CFDictionaryRef attrs);
CTFontDescriptorRef CTFontDescriptorCreateCopyWithAttributes(CTFontDescriptorRef d, CFDictionaryRef attrs);
CTFontRef CTFontCreateWithFontDescriptor(CTFontDescriptorRef d, CGFloat size, const CGAffineTransform* m);
bool CTFontManagerRegisterFontsForURL(CFURLRef url, CTFontManagerScope scope, CFTypeRef* err);
CVReturn CVDisplayLinkCreateWithActiveCGDisplays(CVDisplayLinkRef* link);
CVReturn CVDisplayLinkSetOutputCallback(CVDisplayLinkRef link, CVDisplayLinkOutputCallback cb, void* ctx);
CVReturn CVDisplayLinkStart(CVDisplayLinkRef link);
CVReturn CVDisplayLinkStop(CVDisplayLinkRef link);
void CVDisplayLinkRelease(CVDisplayLinkRef link);
double CVGetHostClockFrequency(void);
OSStatus AudioObjectAddPropertyListener(AudioObjectID id, const AudioObjectPropertyAddress* a, AudioObjectPropertyListenerProc p, void* ctx);
OSStatus AudioObjectRemovePropertyListener(AudioObjectID id, const AudioObjectPropertyAddress* a, AudioObjectPropertyListenerProc p, void* ctx);
OSStatus AudioObjectGetPropertyData(AudioObjectID id, const AudioObjectPropertyAddress* a, uint32_t qs, const void* q, uint32_t* size, void* data);
OSStatus AudioObjectSetPropertyData(AudioObjectID id, const AudioObjectPropertyAddress* a, uint32_t qs, const void* q, uint32_t size, const void* data);
CFRunLoopSourceRef IOPSNotificationCreateRunLoopSource(IOPowerSourceCallbackType cb, void* ctx);
CFStringRef IOPSGetProvidingPowerSourceType(CFTypeRef snapshot);
CFTypeRef IOPSCopyPowerSourcesInfo(void);
CFArrayRef IOPSCopyPowerSourcesList(CFTypeRef blob);
CFDictionaryRef IOPSGetPowerSourceDescription(CFTypeRef blob, CFTypeRef ps);
mach_port_t mach_task_self(void);
kern_return_t task_get_special_port(task_t task, int which, mach_port_t* port);
kern_return_t pid_for_task(mach_port_name_t t, int* pid);
mach_msg_return_t mach_msg(mach_msg_header_t* msg, mach_msg_option_t opt, mach_msg_size_t ss, mach_msg_size_t rs, mach_port_name_t rn, mach_msg_timeout_t to, mach_port_name_t notify);
void mach_msg_destroy(mach_msg_header_t* msg);
kern_return_t mach_port_allocate(task_t t, mach_port_right_t r, mach_port_name_t* name);
kern_return_t mach_port_deallocate(task_t t, mach_port_name_t name);
kern_return_t mach_port_mod_refs(task_t t, mach_port_name_t name, mach_port_right_t r, int delta);
kern_return_t mach_port_insert_right(task_t t, mach_port_name_t name, mach_port_t poly, uint32_t polyPoly);
kern_return_t mach_port_set_attributes(task_t t, mach_port_name_t name, mach_port_flavor_t f, mach_port_info_t info, mach_msg_type_number_t count);
kern_return_t bootstrap_register(mach_port_t bp, char* name, mach_port_t sp);
kern_return_t bootstrap_look_up(mach_port_t bp, const char* name, mach_port_t* sp);
/* END PROTOTYPES */
