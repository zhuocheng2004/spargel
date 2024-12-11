#include <spargel/base/base.h>
#include <spargel/ui/ui.h>

namespace spargel::ui {

    void init_platform() { sbase_panic_here(); }

    void platform_run() { sbase_panic_here(); }

    window_id create_window(int width, int height) { sbase_panic_here(); }

    void destroy_window(window_id window) { sbase_panic_here(); }

    void window_set_title(window_id window, char const* title) { sbase_panic_here(); }

    void window_set_render_callback(window_id window, void (*render)(void*), void* data) {
        sbase_panic_here();
    }

}  // namespace spargel::ui
