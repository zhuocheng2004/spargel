
#include <spargel/entry/simple.h>
#include <spargel/ui/ui.h>

int main() {
    spargel::entry::simple_entry_data data;
    data.platform = spargel::ui::make_platform();
    data.window = data.platform->make_window(500, 500);

    int ret = simple_entry(data);

    return ret;
}
