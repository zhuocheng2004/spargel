#include "spargel/ui/platform.h"

#if __APPLE__
namespace spargel::ui {
Platform* createAppKitPlatform();
Window* createAppKitWindow();
Renderer* createMetalRenderer();
}  // namespace spargel::ui
#endif

namespace spargel::ui {

Platform* Platform::create() {
#if __APPLE__
  return createAppKitPlatform();
#endif
  return nullptr;
}

Platform* Platform::create(PlatformBackend backend) {
  switch (backend) {
#if __APPLE__
    case PlatformBackend::appkit:
      return createAppKitPlatform();
#endif
    default:
      return nullptr;
  }
}

Platform::~Platform() = default;

Platform::Platform(PlatformBackend backend) : backend_{backend} {}

PlatformBackend Platform::backend() const { return backend_; }

Renderer* Platform::createRenderer() {
  switch (backend_) {
#if __APPLE__
    case PlatformBackend::appkit:
      return createMetalRenderer();
#endif
    default:
      return nullptr;
  }
}

Window* Platform::createWindow() {
  switch (backend_) {
#if __APPLE__
    case PlatformBackend::appkit:
      return createAppKitWindow();
#endif
    default:
      return nullptr;
  }
}

}  // namespace spargel::ui
