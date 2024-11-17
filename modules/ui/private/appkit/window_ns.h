#pragma once

#include "modules/base/public/types.h"
#include "modules/ui/public/window.h"

#import <AppKit/AppKit.h>

@interface SpargelWindowDelegate : NSObject <NSWindowDelegate>
@end

@interface SpargelContentView : NSView
@end

namespace spargel::ui {

class renderer_mtl;

class window_ns final : public window {
 public:
  window_ns();
  ~window_ns() override;

  void init(int width, int height) override;

  void set_title(char const* str) override;
  void set_width(int width) override;
  void set_height(int height) override;

  int width() override;
  int height() override;

  void bind_renderer(renderer* r) override;

  void render();

  renderer_mtl* get_renderer();

  void mouse_moved(double x, double y);
  void mouse_down(double x, double y);

  NSRect to_backing(double x, double y, double width, double height);

 private:
  int width_;
  int height_;

  NSWindow* window_;
  renderer_mtl* renderer_;
};

window* create_appkit_window();

}  // namespace spargel::ui
