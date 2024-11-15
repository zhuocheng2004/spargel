#include "modules/window/public/window.h"

#if __APPLE__
namespace spargel::window {
window* create_cocoa_window(window_descriptor const& desc);
}
#endif

namespace spargel::window {

// static
window* window::create(window_descriptor const& desc) {
  switch (desc.backend) {
#if __APPLE__
    case backend_kind::cocoa:
      return create_cocoa_window(desc);
#endif
    default:
      return nullptr;
  }
}

window::~window() = default;

void window::set_title(char const* str) {}

void window::close() {}

}  // namespace spargel::window
