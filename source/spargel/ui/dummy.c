#include <spargel/base/base.h>
#include <spargel/ui/ui.h>

void spargel_ui_init_platform() { sbase_panic_here(); }

void spargel_ui_platform_run() { sbase_panic_here(); }

spargel_ui_window_id spargel_ui_create_window(int width, int height) { sbase_panic_here(); }

void spargel_ui_destroy_window(spargel_ui_window_id window) { sbase_panic_here(); }

void spargel_ui_window_set_title(spargel_ui_window_id window, char const* title) {
    sbase_panic_here();
}

void spargel_ui_window_set_render_callback(spargel_ui_window_id window, void (*render)(void*),
                                           void* data) {
    sbase_panic_here();
}
