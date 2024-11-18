#pragma once

namespace spargel::ui {

class Renderer;
class Window;

class WindowDelegate {
 public:
  virtual ~WindowDelegate();

  virtual void render(Renderer* r);

  virtual void onMouseMove(double x, double y);

  virtual void onMouseDown(double x, double y);

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

  virtual void setRenderer(Renderer* r) = 0;

  void setDelegate(WindowDelegate* delegate);
  WindowDelegate* delegate();

 private:
  WindowDelegate* delegate_;
};

}  // namespace spargel::ui
