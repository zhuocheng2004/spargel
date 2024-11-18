#pragma once

namespace spargel::ui {

struct Point {
  float x;
  float y;
};

struct RectSize {
  float width;
  float height;

  friend bool operator==(RectSize const& lhs, RectSize const& rhs);
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
