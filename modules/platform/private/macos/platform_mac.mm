#include "modules/platform/private/macos/platform_mac.h"

#import <AppKit/NSApplication.h>

@implementation SpargelAppDelegate
- (instancetype)initWithSpargelPlatform:(spargel::platform::platform_mac*)platform {
  [super init];
  platform_ = platform;
  return self;
}
- (void)applicationDidFinishLaunching:(NSNotification*)notification {
}
@end

namespace spargel::platform {

platform_mac::platform_mac() {  }
platform_mac::~platform_mac() {}

void platform_mac::init() {
  if (initialized()) return;
  [NSApplication sharedApplication];
  delegate_ = [[SpargelAppDelegate alloc] init];
  [NSApp setDelegate:delegate_];
  [NSApp run];
  initialized_ = true;
}

void platform_mac::deinit() { [delegate_ release]; }

bool platform_mac::initialized() { return initialized_; }

platform* create_macos_platform() { return new platform_mac; }

}  // namespace spargel::platform
