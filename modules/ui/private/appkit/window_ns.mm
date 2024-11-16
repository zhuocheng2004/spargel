#include "modules/ui/private/appkit/window_ns.h"

#import <AppKit/NSScreen.h>
#import <AppKit/NSWindow.h>

namespace spargel::ui {

window_ns::window_ns() : width_{300}, height_{300} {}
window_ns::~window_ns() = default;

void window_ns::init(int width, int height) {
  @autoreleasepool {
    auto screen = [NSScreen mainScreen];

    auto style = NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable |
                 NSWindowStyleMaskResizable | NSWindowStyleMaskTitled;
    auto rect = NSMakeRect(0, 0, width_, height_);
    // center the window
    rect.origin.x = (screen.frame.size.width - width_) / 2;
    rect.origin.y = (screen.frame.size.height - height_) / 2;

    window_ = [[NSWindow alloc] initWithContentRect:rect
                                          styleMask:style
                                            backing:NSBackingStoreBuffered
                                              defer:NO
                                             screen:screen];
    window_.releasedWhenClosed = NO;
    window_.minSize = NSMakeSize(200, 200);

    // auto delegate = [[SpargelWindowDelegate alloc] init];
    // window_.delegate = delegate_;

    // auto view = [[SpargelContentView alloc] init];
    // window_.contentView = view_;
    // [window_ makeFirstResponder:view_];

    [window_ makeKeyAndOrderFront:nil];
  }
}

void window_ns::set_title(char const* title) {
  [window_ setTitle:[NSString stringWithUTF8String:title]];
}

void window_ns::set_width(int width) { width_ = width; }
void window_ns::set_height(int height) { height_ = height; }

window* create_appkit_window() { return new window_ns; }

}  // namespace spargel::ui
