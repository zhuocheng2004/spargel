#pragma once

#include "modules/platform/public/platform.h"

#import <AppKit/NSApplication.h>

namespace spargel::platform {
class platform_mac;
}

// Notes:
//
// The default `terminate:` implementation ends the process directly, never leaving the event loop.
//
@interface SpargelAppDelegate : NSObject <NSApplicationDelegate> {
  spargel::platform::platform_mac* platform_;
}
- (instancetype)initWithSpargelPlatform:(spargel::platform::platform_mac*)platform;
- (void)applicationDidFinishLaunching:(NSNotification*)notification;
@end

namespace spargel::platform {

class platform_mac final : public platform {
 public:
  platform_mac();
  ~platform_mac() override;

  void init() override;
  void deinit() override;

  bool initialized() override;

 private:
  bool initialized_ = false;
  SpargelAppDelegate* delegate_;
};

platform* create_macos_platform();

}  // namespace spargel::platform
