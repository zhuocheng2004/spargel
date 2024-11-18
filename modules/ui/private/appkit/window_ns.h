#pragma once

#include "modules/ui/public/types.h"
#include "modules/ui/public/window.h"

#import <AppKit/AppKit.h>

namespace spargel::ui {
class WindowNS;
}

@interface SpargelWindowDelegate : NSObject <NSWindowDelegate> {
  spargel::ui::WindowNS* _window;
}
@end

namespace spargel::ui {

class RenderTargetMTL;

class WindowNS final : public Window {
 public:
  WindowNS();
  ~WindowNS() override;

  void init(int width, int height) override;

  void setTitle(char const* str) override;
  void setWidth(int width) override;
  void setHeight(int height) override;

  int width() override;
  int height() override;

  RenderTarget* renderTarget() override;

  // called by SpargelMetalView
  void render();
  void mouseMoved(float x, float y);
  void mouseDown(float x, float y);
  void setDrawableSize(RectSize size);
  void willClose();

  // called by ??
  Rect toBacking(Rect rect);

 private:
  int width_;
  int height_;

  NSWindow* window_;
  SpargelWindowDelegate* _ns_delegate;
  RenderTargetMTL* _render_target;
};

Window* createAppKitWindow();

}  // namespace spargel::ui
