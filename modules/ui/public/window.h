#pragma once

namespace spargel::ui {

class window {
 public:
  virtual ~window();

  virtual void init(int width, int height) = 0;

  virtual void set_title(char const* str) = 0;
  virtual void set_width(int width) = 0;
  virtual void set_height(int height) = 0;
};

}  // namespace spargel::ui
