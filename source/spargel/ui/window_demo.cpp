#include <spargel/ui/ui.h>
#include <unistd.h>

int main() {
    auto platform = spargel::ui::make_platform();
    auto window = platform->make_window(500, 500);
    window->set_title("Spargel Demo - Window");
    platform->start_loop();
    return 0;
}
