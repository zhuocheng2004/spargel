#include "modules/ui/public/window.h"

#include "modules/ui/public/platform.h"

namespace spargel::ui {

WindowDelegate::~WindowDelegate() = default;

void WindowDelegate::render(Renderer* r) {}

void WindowDelegate::onMouseMove(double x, double y) {}

void WindowDelegate::onMouseDown(double x, double y) {}

void WindowDelegate::setWindow(Window* window) {}

Window::Window() : delegate_{nullptr} {}

Window::~Window() = default;

void Window::setDelegate(WindowDelegate* delegate) {
  delegate_ = delegate;
  delegate->setWindow(this);
}

WindowDelegate* Window::delegate() { return delegate_; }

}  // namespace spargel::ui
