#include <spargel/base/logging.h>
#include <spargel/ui/ui.h>

class delegate final : public spargel::ui::window_delegate {
public:
    void on_keyboard(spargel::ui::keyboard_event& e) override {
        spargel_log_info("key down : %d", e.key);
    }
};

int main() {
    auto platform = spargel::ui::make_platform();
    auto window = platform->make_window(500, 500);
    window->set_title("Spargel Demo - Window");
    delegate d;
    window->set_delegate(&d);
    platform->start_loop();
    return 0;
}
