#pragma once

#include "modules/window/public/window.h"
#include "modules/base/public/types.h"

#import <AppKit/NSWindow.h>

@interface SpargelWindowDelegate : NSObject <NSWindowDelegate>
@end

@interface SpargelContentView : NSView
@end

namespace spargel::window {

class window_cocoa : public window {
 public:
  window_cocoa();
  ~window_cocoa() override;

  void init(window_descriptor const& desc);
  void deinit();

 private:
  int width_;
  int height_;

  NSWindow* window_;
  SpargelWindowDelegate* delegate_;
  SpargelContentView* view_;
};

window* create_cocoa_window(window_descriptor const& desc);

}  // namespace spargel::window
