#pragma once

#include "modules/ui/public/platform.h"

#import <AppKit/NSApplication.h>

@interface SpargelApplicationDelegate : NSObject <NSApplicationDelegate> {
}
- (void)applicationDidFinishLaunching:(NSNotification*)notification;
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender;
@end

namespace spargel::ui {

class platform_ns final : public platform {
 public:
  platform_ns();
  ~platform_ns() override;

  void init() override;
  void run() override;
};

platform* create_appkit_platform();

}  // namespace spargel::ui
