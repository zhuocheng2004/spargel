#include "base/logging/logging.h"
#include "samples/vulkan_triangle/app.h"

#if __APPLE__
#include "samples/vulkan_triangle/app_delegate_mac.h"
#endif

int main() {
#if __APPLE__
  AppDelegateMac delegate;
#else
#error "unsupported platform"
#endif

  App app(&delegate);
  LOG(base::logging::LogLevel::Info, "before init");
  if (!app.Init()) return 1;
  app.Run();
  // while(true) {
  //   delegate.PollEvents();
  // }
  app.Deinit();
  return 0;
}
