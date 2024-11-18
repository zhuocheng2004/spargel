#include "modules/ui/public/window.h"

#include "modules/ui/public/platform.h"

namespace spargel::ui {

Window::Window() { _delegate = nullptr; }

Window::~Window() = default;

void Window::setDelegate(WindowDelegate* delegate) { _delegate = delegate; }

WindowDelegate* Window::delegate() { return _delegate; }

}  // namespace spargel::ui
