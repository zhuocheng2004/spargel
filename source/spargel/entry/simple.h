#pragma once

#include <spargel/resource/resource.h>
#include <spargel/ui/ui.h>

namespace spargel::entry {

    struct simple_entry_data {
        base::unique_ptr<ui::platform> platform;
        base::unique_ptr<ui::window> window;
        base::unique_ptr<resource::resource_manager> resource_manager;
    };

    int simple_entry(spargel::entry::simple_entry_data* entry_data);

}  // namespace spargel::entry
