#include "modules/ui/public/window_delegate.h"

namespace spargel::ui {

WindowDelegate::~WindowDelegate() = default;

void WindowDelegate::render() {}

void WindowDelegate::onMouseMove(float x, float y) {}

void WindowDelegate::onMouseDown(float x, float y) {}

void WindowDelegate::onClose() {}

}  // namespace spargel::ui
