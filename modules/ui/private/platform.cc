#include "modules/ui/public/platform.h"

#if __APPLE__
namespace spargel::ui {
platform* create_appkit_platform();
window* create_appkit_window();
}  // namespace spargel::ui
#endif

namespace spargel::ui {

platform* platform::create() {
#if __APPLE__
  return create_appkit_platform();
#endif
  return nullptr;
}

platform* platform::create(platform_backend backend) {
  switch (backend) {
#if __APPLE__
    case platform_backend::appkit:
      return create_appkit_platform();
#endif
    default:
      return nullptr;
  }
}

platform::~platform() = default;

platform::platform(platform_backend backend) : backend_{backend} {}

platform_backend platform::backend() const { return backend_; }

window* platform::create_window() {
  switch (backend_) {
#if __APPLE__
    case platform_backend::appkit:
      return create_appkit_window();
#endif
    default:
      return nullptr;
  }
}

}  // namespace spargel::ui
