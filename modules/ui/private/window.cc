#include "modules/ui/public/window.h"

#include "modules/ui/public/platform.h"

namespace spargel::ui {

window_delegate::~window_delegate() = default;

void window_delegate::render(renderer* r) {}

void window_delegate::on_mouse_move(double x, double y) {}

void window_delegate::on_mouse_down(double x, double y) {}

void window_delegate::set_window(window* window) {}

window::window() : delegate_{nullptr} {}

window::~window() = default;

void window::set_delegate(window_delegate* delegate) {
  delegate_ = delegate;
  delegate->set_window(this);
}

window_delegate* window::delegate() { return delegate_; }

}  // namespace spargel::ui
