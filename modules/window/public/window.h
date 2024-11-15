#pragma once

#include "modules/platform/public/platform.h"

namespace spargel::window {

enum class backend_kind {
  cocoa,
  glfw,
  wayland,
  win32,
  xlib,
};

struct window_descriptor {
  // todo: is platform used anywhere
  platform::platform* platform;
  backend_kind backend;
  int width;
  int height;
};

class window {
 public:
  static window* create(window_descriptor const& desc);

  virtual ~window();
};

}  // namespace spargel::window
