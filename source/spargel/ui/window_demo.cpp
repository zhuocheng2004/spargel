#include <spargel/ui/ui.h>
#include <unistd.h>

static void render_callback(void*) { sleep(1); }

int main() {
    spargel::ui::init_platform();
    spargel::ui::window_id window = spargel::ui::create_window(500, 500);
    spargel::ui::window_set_title(window, "Spargel Demo - Window");
    spargel::ui::window_set_render_callback(window, render_callback, 0);
    spargel::ui::platform_run();
    return 0;
}
