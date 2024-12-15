
#include <spargel/base/platform.h>
#include <spargel/base/string.h>
#include <spargel/entry/simple.h>
#include <spargel/resource/directory.h>
#include <spargel/ui/ui.h>
#include <spargel/util/path.h>

#if SPARGEL_IS_ANDROID
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <spargel/resource/android_asset.h>
#endif

int main() {
    spargel::entry::simple_entry_data data;
    data.platform = spargel::ui::make_platform();
    data.window = data.platform->make_window(500, 500);

    int ret = simple_entry(&data);

    return ret;
}

namespace spargel::entry {
#if SPARGEL_IS_ANDROID
    base::unique_ptr<resource::resource_manager> make_resource_manager(simple_entry_data* data) {
        return base::make_unique<resource::android_asset_resource_manager>(
            data->app->activity->assetManager);
    }
#else
    base::unique_ptr<resource::resource_manager> make_resource_manager(simple_entry_data* data) {
        base::string root_path =
            util::dirname(base::get_executable_path()) + PATH_SPLIT + base::string("resources");
        return base::make_unique<resource::directory_resource_manager>(root_path.view());
    }
#endif
}  // namespace spargel::entry
