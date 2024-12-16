#pragma once

#include <spargel/ui/ui.h>

namespace spargel::entry {

    struct simple_entry_data {
        spargel::base::unique_ptr<spargel::ui::platform> platform;
        spargel::base::unique_ptr<spargel::ui::window> window;
    };

}  // namespace spargel::entry

extern "C" {

int simple_entry(spargel::entry::simple_entry_data* data);
}
