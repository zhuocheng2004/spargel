#include "modules/window/private/cocoa/window_cocoa.h"

#import <AppKit/NSWindow.h>

@implementation SpargelWindowDelegate
@end

@implementation SpargelContentView
@end

namespace spargel::window {

window_cocoa::window_cocoa() = default;
window_cocoa::~window_cocoa() = default;

void window_cocoa::init(window_descriptor const& desc) {
  width_ = desc.width;
  height_ = desc.height;

  auto style = NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable |
               NSWindowStyleMaskResizable | NSWindowStyleMaskTitled;
  auto rect = NSMakeRect(0, 0, width_, height_);

  window_ = [[NSWindow alloc] initWithContentRect:rect
                                        styleMask:style
                                          backing:NSBackingStoreBuffered
                                            defer:NO];

  delegate_ = [[SpargelWindowDelegate alloc] init];
  // window_.delegate = delegate_;

  view_ = [[SpargelContentView alloc] init];
  // window_.contentView = view_;
  // [window_ makeFirstResponder:view_];

  [window_ makeKeyAndOrderFront:nil];

  closed_ = false;
}

void window_cocoa::deinit() {
  if (closed_) return;
  close();
  [delegate_ release];
  [view_ release];
}

void window_cocoa::set_title(char const* str) {
  [window_ setTitle:[NSString stringWithUTF8String:str]];
}

void window_cocoa::close() {
  if (closed_) return;
  // default behaviour: the window is automatically released after it is closed
  [window_ close];
  closed_ = true;
}

window* create_cocoa_window(window_descriptor const& desc) {
  auto w = new window_cocoa;
  w->init(desc);
  return w;
}

}  // namespace spargel::window
