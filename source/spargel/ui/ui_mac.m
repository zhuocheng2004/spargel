#import <AppKit/AppKit.h>
#include <spargel/ui/ui.h>

@interface SpargelApplicationDelegate : NSObject <NSApplicationDelegate>
@end
@implementation SpargelApplicationDelegate
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender {
  return YES;
}
@end

static void init_global_menu() {
  NSMenu* menu_bar = [[NSMenu alloc] init];

  NSMenu* app_menu = [[NSMenu alloc] initWithTitle:@"Spargel"];
  [app_menu addItemWithTitle:@"About Spargel" action:nil keyEquivalent:@""];
  [app_menu addItemWithTitle:@"Check for Updates" action:nil keyEquivalent:@""];
  [app_menu addItemWithTitle:@"Quit Spargel" action:@selector(terminate:) keyEquivalent:@"q"];

  NSMenuItem* app_menu_item = [[NSMenuItem alloc] init];
  [app_menu_item setSubmenu:app_menu];
  [menu_bar addItem:app_menu_item];

  /* mainMenu is a strong reference */
  NSApp.mainMenu = menu_bar;
}

void spargel_ui_init_platform() {
  [NSApplication sharedApplication];
  [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

  SpargelApplicationDelegate* delegate = [[SpargelApplicationDelegate alloc] init];
  /* NSApp.delegate is a weak reference */
  NSApp.delegate = delegate;

  init_global_menu();
}

void spargel_ui_platform_run() {
  /* run until stop: or terminate: */
  [NSApp run];
}

@interface SpargelWindowDelegate : NSObject <NSWindowDelegate> {
  struct spargel_ui_window* _swindow;
}
@end
@implementation SpargelWindowDelegate
- (instancetype)initWithSpargelUIWindow:(struct spargel_ui_window*)window {
  [super init];
  _swindow = window;
  return self;
}
@end

struct spargel_ui_window {
  NSWindow* window;
  SpargelWindowDelegate* delegate;
};

spargel_ui_window_id spargel_ui_create_window(int width, int height) {
  spargel_ui_window_id window = malloc(sizeof(struct spargel_ui_window));
  if (!window) return NULL;

  NSScreen* screen = [NSScreen mainScreen];

  NSWindowStyleMask style = NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable |
                            NSWindowStyleMaskResizable | NSWindowStyleMaskTitled;

  NSRect rect = NSMakeRect(0, 0, width, height);
  /* center the window */
  rect.origin.x = (screen.frame.size.width - width) / 2;
  rect.origin.y = (screen.frame.size.height - height) / 2;

  window->delegate = [[SpargelWindowDelegate alloc] initWithSpargelUIWindow:window];

  window->window = [[NSWindow alloc] initWithContentRect:rect
                                               styleMask:style
                                                 backing:NSBackingStoreBuffered
                                                   defer:NO
                                                  screen:screen];
  /* weak reference */
  window->window.delegate = window->delegate;
  window->window.releasedWhenClosed = NO;
  window->window.minSize = NSMakeSize(200, 200);

  [window->window makeKeyAndOrderFront:nil];

  return window;
}

void spargel_ui_destroy_window(spargel_ui_window_id window) {
  [window->window release];
  [window->delegate release];
}

void spargel_ui_window_set_title(spargel_ui_window_id window, char const* title) {
  window->window.title = [NSString stringWithUTF8String:title];
}
