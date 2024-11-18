#pragma once

#include "spargel/ui/platform.h"

#import <AppKit/NSApplication.h>

@interface SpargelApplicationDelegate : NSObject <NSApplicationDelegate> {
}
- (void)applicationDidFinishLaunching:(NSNotification*)notification;
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender;
@end

namespace spargel::ui {

class PlatformNS final : public Platform {
 public:
  PlatformNS();
  ~PlatformNS() override;

  void init() override;
  void run() override;
};

Platform* createAppKitPlatform();

}  // namespace spargel::ui
