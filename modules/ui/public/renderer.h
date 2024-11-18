#pragma once

namespace spargel::ui {

struct Color3 {
  float r;
  float g;
  float b;
};

class Renderer {
 public:
  virtual ~Renderer();

  virtual void init() = 0;

  virtual void begin() = 0;
  virtual void end() = 0;

  virtual void drawQuad(float x, float y, float width, float height, Color3 color) = 0;
};

}  // namespace spargel::ui
