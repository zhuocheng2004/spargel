#pragma once

namespace spargel::ui {

class WindowDelegate {
 public:
  virtual ~WindowDelegate();

  virtual void render();

  virtual void onMouseMove(float x, float y);

  virtual void onMouseDown(float x, float y);

  // window is about to close
  virtual void onClose();
};

}  // namespace spargel::ui
