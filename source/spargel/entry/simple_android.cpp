
#include <spargel/base/base.h>
#include <spargel/base/logging.h>
#include <spargel/entry/simple.h>
#include <spargel/ui/ui_android.h>

// JNI & Android
#include <jni.h>

// Game Activity
#include <game-activity/native_app_glue/android_native_app_glue.h>

static void onAppCmd(struct android_app* app, int32_t cmd) {
    auto* data = (spargel::ui::android_app_data*)app->userData;
    auto* platform = data->platform;
    auto* window = platform->window_handle();

    switch (cmd) {
    case APP_CMD_START:
        break;
    case APP_CMD_INIT_WINDOW:
        data->can_render = true;
        break;
    case APP_CMD_TERM_WINDOW:
        window->delegate()->on_close_requested();
        data->can_render = false;
        break;
    case APP_CMD_DESTROY:
        window->delegate()->on_closed();
        break;
    default:
        break;
    }
}

extern "C" {

void android_main(struct android_app* app) {
    auto* data = new spargel::ui::android_app_data;
    data->app = app;
    data->can_render = false;
    app->userData = data;
    app->onAppCmd = onAppCmd;

    spargel::entry::simple_entry_data entry_data;
    auto platform = spargel::base::make_unique<spargel::ui::platform_android>(app);
    entry_data.window = platform->generate_window_handle();
    entry_data.platform = spargel::base::move(platform);

    simple_entry(&entry_data);

    delete data;
}
}
