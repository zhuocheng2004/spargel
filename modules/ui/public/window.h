#pragma once

namespace spargel::ui {

class renderer;
class window;

class window_delegate {
 public:
  virtual ~window_delegate();

  virtual void render(renderer* r);

  virtual void on_mouse_move(double x, double y);

  virtual void on_mouse_down(double x, double y);

  virtual void set_window(window* window);
};

class window {
 public:
  window();
  virtual ~window();

  virtual void init(int width, int height) = 0;

  virtual void set_title(char const* str) = 0;
  virtual void set_width(int width) = 0;
  virtual void set_height(int height) = 0;

  virtual int width() = 0;
  virtual int height() = 0;

  virtual void bind_renderer(renderer* r) = 0;

  void set_delegate(window_delegate* delegate);
  window_delegate* delegate();

 private:
  window_delegate* delegate_;
};

}  // namespace spargel::ui
