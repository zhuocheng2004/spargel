#pragma once

#include "modules/ui/public/types.h"
#include "modules/ui/public/window.h"

#import <AppKit/AppKit.h>

@interface SpargelWindowDelegate : NSObject <NSWindowDelegate>
@end

@interface SpargelContentView : NSView
@end

namespace spargel::ui {

class RendererMTL;

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

  void setRenderer(Renderer* r) override;

  void render();

  RendererMTL* getRenderer();

  void mouseMoved(float x, float y);
  void mouseDown(float x, float y);

  Rect toBacking(Rect rect);

 private:
  int width_;
  int height_;

  NSWindow* window_;
  RendererMTL* renderer_;
};

Window* createAppKitWindow();

}  // namespace spargel::ui
