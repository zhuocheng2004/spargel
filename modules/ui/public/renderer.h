#pragma once

namespace spargel::ui {

struct color3 {
  float r;
  float g;
  float b;
};

class renderer {
 public:
  virtual ~renderer();

  virtual void init() = 0;

  virtual void begin() = 0;
  virtual void end() = 0;

  virtual void draw_quad(float x, float y, float width, float height, color3 color) = 0;
};

}  // namespace spargel::ui
