#include <spargel/ui/ui.h>

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
    sui_init_platform();

    auto window = sui_create_window(500, 500);
    sui_window_set_title(window, "Vulkan Triangle");

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

    sui_window_set_render_callback(window, &App::render, &app);

    sui_platform_run();
    return 0;
}
