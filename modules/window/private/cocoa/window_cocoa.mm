#include "modules/window/private/cocoa/window_cocoa.h"

#import <AppKit/NSWindow.h>

@implementation SpargelWindowDelegate
@end

namespace spargel::window {

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
  window_.delegate = delegate_;

  view_ = [[SpargelContentView alloc] init];
  window_.contentView = view_;
}

void window_cocoa::deinit() {
  [window_ release];
  [delegate_ release];
  [view_ release];
}

window* create_cocoa_window(window_descriptor const& desc) {
  auto w = new window_cocoa;
  w->init(desc);
  return w;
}

}  // namespace spargel::window
