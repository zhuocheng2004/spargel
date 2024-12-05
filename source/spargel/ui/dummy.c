#include <spargel/base/base.h>
#include <spargel/ui/ui.h>

void sui_init_platform() { sbase_panic_here(); }

void sui_platform_run() { sbase_panic_here(); }

sui_window_id sui_create_window(int width, int height) { sbase_panic_here(); }

void sui_destroy_window(sui_window_id window) { sbase_panic_here(); }

void sui_window_set_title(sui_window_id window, char const* title) { sbase_panic_here(); }

void sui_window_set_render_callback(sui_window_id window, void (*render)(void*), void* data) {
    sbase_panic_here();
}
