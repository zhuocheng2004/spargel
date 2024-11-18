#include "spargel/ui/appkit/platform_ns.h"

@implementation SpargelApplicationDelegate
- (void)applicationDidFinishLaunching:(NSNotification*)notification {
  // do nothing
}
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender {
  // todo: remove this
  return YES;
}
@end

namespace spargel::ui {

Platform* createAppKitPlatform() { return new PlatformNS; }

PlatformNS::PlatformNS() : Platform(PlatformBackend::appkit) {}
PlatformNS::~PlatformNS() = default;

void PlatformNS::init() {
  [NSApplication sharedApplication];
  [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

  auto delegate = [[SpargelApplicationDelegate alloc] init];
  [NSApp setDelegate:delegate];

  auto menu_bar = [[NSMenu alloc] init];

  auto app_menu = [[NSMenu alloc] initWithTitle:@"Spargel"];
  [app_menu addItemWithTitle:@"About Spargel" action:nil keyEquivalent:@""];
  [app_menu addItemWithTitle:@"Check for Updates" action:nil keyEquivalent:@""];
  [app_menu addItemWithTitle:@"Quit Spargel" action:@selector(terminate:) keyEquivalent:@"q"];

  auto app_menu_item = [[NSMenuItem alloc] init];
  [app_menu_item setSubmenu:app_menu];
  [menu_bar addItem:app_menu_item];

  NSApp.mainMenu = menu_bar;
}
void PlatformNS::run() {
  // just start the NSApplication
  [NSApp run];
}

}  // namespace spargel::ui
