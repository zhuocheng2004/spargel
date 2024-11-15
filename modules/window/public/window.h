#pragma once

namespace spargel::window {

struct window_descriptor;

class window {
 public:
  static window* create(window_descriptor const& desc);

  virtual ~window();
};

}  // namespace spargel::window
