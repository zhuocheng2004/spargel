#pragma once

namespace spargel::ui {

struct Point {
  float x;
  float y;
};

struct RectSize {
  float width;
  float height;
};

struct Rect {
  Point origin;
  RectSize size;
};

struct Color3 {
  float r;
  float g;
  float b;
};

}  // namespace spargel::ui
