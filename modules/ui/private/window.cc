#include "modules/ui/public/window.h"

#include "modules/ui/public/platform.h"

namespace spargel::ui {

WindowDelegate::~WindowDelegate() = default;

void WindowDelegate::render() {}

void WindowDelegate::onMouseMove(float x, float y) {}

void WindowDelegate::onMouseDown(float x, float y) {}

void WindowDelegate::setWindow(Window* window) {}

Window::Window() { _delegate = nullptr; }

Window::~Window() = default;

void Window::setDelegate(WindowDelegate* delegate) {
  _delegate = delegate;
  delegate->setWindow(this);
}

WindowDelegate* Window::delegate() { return _delegate; }

}  // namespace spargel::ui
