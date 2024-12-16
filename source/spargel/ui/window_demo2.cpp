
#include <spargel/base/logging.h>
#include <spargel/entry/simple.h>
#include <spargel/ui/ui.h>

class delegate final : public spargel::ui::window_delegate {};

int simple_entry(spargel::entry::simple_entry_data* data) {
    spargel_log_debug("window_demo2");
    data->window->set_title("Spargel - Window Demo 2");
    delegate d;
    data->window->set_delegate(&d);
    data->platform->start_loop();
    return 0;
}
