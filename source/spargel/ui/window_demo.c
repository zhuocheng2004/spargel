#include <spargel/ui/ui.h>
#include <unistd.h>

static void render_callback() { sleep(1); }

int main() {
    sui_init_platform();
    sui_window_id window = sui_create_window(500, 500);
    sui_window_set_title(window, "Spargel Demo - Window");
    sui_window_set_render_callback(window, render_callback, 0);
    sui_platform_run();
    return 0;
}
