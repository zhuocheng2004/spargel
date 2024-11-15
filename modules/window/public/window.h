#pragma once

namespace spargel::window {

enum class backend_kind {
  cocoa,
  glfw,
  wayland,
  win32,
  xlib,
};

struct window_descriptor {
  backend_kind backend;
  int width;
  int height;
};

class window {
 public:
  static window* create(window_descriptor const& desc);

  virtual ~window();

  virtual void set_title(char const* str);

  virtual void close();
};

}  // namespace spargel::window
