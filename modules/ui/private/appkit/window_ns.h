#pragma once

#include "modules/base/public/types.h"
#include "modules/ui/public/window.h"

#import <AppKit/NSWindow.h>

@interface SpargelWindowDelegate : NSObject <NSWindowDelegate>
@end

@interface SpargelContentView : NSView
@end

namespace spargel::ui {

class window_ns final : public window {
 public:
  window_ns();
  ~window_ns() override;

  void init(int width, int height) override;

  void set_title(char const* str) override;
  void set_width(int width) override;
  void set_height(int height) override;

 private:
  int width_;
  int height_;

  NSWindow* window_;
};

window* create_appkit_window();

}  // namespace spargel::ui
