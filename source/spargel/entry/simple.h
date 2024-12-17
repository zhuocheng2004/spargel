#pragma once

#include <spargel/resource/resource.h>
#include <spargel/ui/ui.h>

#if SPARGEL_IS_ANDROID
struct android_app;
#endif

namespace spargel::entry {

    struct simple_entry_data {
        spargel::base::unique_ptr<spargel::ui::platform> platform;
        spargel::base::unique_ptr<spargel::ui::window> window;
#if SPARGEL_IS_ANDROID
        android_app* app;
#endif
    };

    resource::resource_manager* get_resource_manager(simple_entry_data* data);

}  // namespace spargel::entry

extern "C" {

int simple_entry(spargel::entry::simple_entry_data* data);
}
