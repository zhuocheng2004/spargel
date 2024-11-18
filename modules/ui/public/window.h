#pragma once

namespace spargel::ui {

class RenderTarget;
class Window;

class WindowDelegate {
 public:
  virtual ~WindowDelegate();

  virtual void render();

  virtual void onMouseMove(float x, float y);

  virtual void onMouseDown(float x, float y);

  virtual void setWindow(Window* window);
};

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
