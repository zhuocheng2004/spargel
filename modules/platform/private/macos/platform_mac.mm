#include "modules/platform/private/macos/platform_mac.h"

#import <AppKit/NSApplication.h>

@implementation SpargelAppDelegate
- (instancetype)initWithSpargelPlatform:(spargel::platform::platform_mac*)platform {
  [super init];
  platform_ = platform;
  return self;
}
- (void)applicationDidFinishLaunching:(NSNotification*)notification {
  // send an empty event so that the event loop can stop properly
  NSEvent* event = [NSEvent otherEventWithType:NSEventTypeApplicationDefined
                                      location:NSMakePoint(0, 0)
                                 modifierFlags:0
                                     timestamp:0
                                  windowNumber:0
                                       context:nil
                                       subtype:0
                                         data1:0
                                         data2:0];
  [NSApp postEvent:event atStart:YES];

  [NSApp stop:nil];
}
@end

namespace spargel::platform {

platform_mac::platform_mac() {}
platform_mac::~platform_mac() {}

void platform_mac::init() {
  if (initialized()) return;
  [NSApplication sharedApplication];
  delegate_ = [[SpargelAppDelegate alloc] init];
  [NSApp setDelegate:delegate_];
  [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
  [NSApp run];
  initialized_ = true;
}

void platform_mac::deinit() { [delegate_ release]; }

bool platform_mac::initialized() { return initialized_; }

void platform_mac::poll_event() {
  while (true) {
    @autoreleasepool {
      NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                          untilDate:[NSDate distantPast]
                                             inMode:NSDefaultRunLoopMode
                                            dequeue:YES];
      if (event == nil) break;
      [NSApp sendEvent:event];
    }
  }
}

platform* create_macos_platform() { return new platform_mac; }

}  // namespace spargel::platform
