#pragma once

namespace spargel::ui {

class RenderTarget;
class WindowDelegate;

class Window {
 public:
  Window();
  virtual ~Window();

  virtual void init(int width, int height) = 0;

  virtual void setTitle(char const* str) = 0;
  virtual void setWidth(int width) = 0;
  virtual void setHeight(int height) = 0;

  virtual int width() = 0;
  virtual int height() = 0;

  void setDelegate(WindowDelegate* delegate);
  WindowDelegate* delegate();

  virtual RenderTarget* renderTarget() = 0;

 private:
  WindowDelegate* _delegate;
};

}  // namespace spargel::ui
