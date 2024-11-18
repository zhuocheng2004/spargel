#pragma once

#include "modules/base/public/types.h"
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

  void mouseMoved(double x, double y);
  void mouseDown(double x, double y);

  NSRect toBacking(double x, double y, double width, double height);

 private:
  int width_;
  int height_;

  NSWindow* window_;
  RendererMTL* renderer_;
};

Window* createAppKitWindow();

}  // namespace spargel::ui
