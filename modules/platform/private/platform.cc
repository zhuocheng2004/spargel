#include "modules/platform/public/platform.h"

#if __APPLE__
namespace spargel::platform {
platform* create_macos_platform();
}
#endif

namespace spargel::platform {

// static
platform* platform::create() {
#if __APPLE__
  return create_macos_platform();
#else
  return nullptr;
#endif
}

platform::~platform() = default;

}  // namespace spargel::platform
