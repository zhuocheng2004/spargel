#include <spargel/base/base.h>
#include <spargel/ui/ui.h>

namespace spargel::ui {

    void init_platform() { spargel_panic_here(); }

    void platform_run() { spargel_panic_here(); }

    window_id create_window(int width, int height) { spargel_panic_here(); }

    void destroy_window(window_id window) { spargel_panic_here(); }

    void window_set_title(window_id window, char const* title) { spargel_panic_here(); }

    void window_set_render_callback(window_id window, void (*render)(void*), void* data) {
        spargel_panic_here();
    }

}  // namespace spargel::ui
