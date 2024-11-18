#include "spargel/ui/platform.h"
#include "spargel/ui/window.h"
#include "vulkan_triangle/app.h"
#include "vulkan_triangle/logging.h"

#if __APPLE__
#include "vulkan_triangle/app_delegate_mac.h"
#endif
#if _WIN32
// #include "vulkan_triangle/app_delegate_win.h"
#endif

using enum base::logging::LogLevel;

int main() {
  auto platform = spargel::ui::Platform::create();
  platform->init();

  auto window = platform->createWindow();
  window->init(500, 500);
  window->setTitle("Vulkan Triangle");

#if __APPLE__
  AppDelegateMac delegate(window);
#elif _WIN32
//  AppDelegateWin delegate;
#else
#error "unsupported platform"
#endif

  App app(&delegate);
  LOG(Info, "before init");
  if (!app.Init()) return 1;

  window->setDelegate(&app);

  platform->run();
  return 0;
}
