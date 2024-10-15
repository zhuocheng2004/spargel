#include "ui/window.h"

#include "ui/window_delegate.h"

namespace spargel::ui {

Window::Window(WindowDelegate* delegate) noexcept
  : delegate_{delegate} {}

Window::~Window() noexcept {}

auto Window::GetTitle() const noexcept -> char const* {
  return title_;
}

auto Window::SetTitle(char const* title) noexcept -> void {
  // TODO: copy or not
  title_ = title;
}

}
