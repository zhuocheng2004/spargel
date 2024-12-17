
#include <spargel/ui/ui_android.h>

namespace spargel::ui {

    platform_android::platform_android(struct android_app* app)
        : platform(platform_kind::android), _app(app) {
        auto* data = (spargel::ui::android_app_data*)_app->userData;
        data->platform = this;
    }

    platform_android::~platform_android() {
        // TODO
    }

    void platform_android::start_loop() {
        auto* data = (spargel::ui::android_app_data*)_app->userData;

        while (!_app->destroyRequested) {
            android_poll_source* source;
            auto result = ALooper_pollOnce(0, nullptr, nullptr, (void**)&source);
            if (result == ALOOPER_POLL_ERROR) {
                spargel_log_error("ALooper_pollOnce returned an error");
                spargel_panic_here();
            }

            if (source != nullptr) {
                source->process(_app, source);
            }

            if (data->can_render && window) {
                window->delegate()->on_render();
            }
        }
    }

    base::unique_ptr<window_android> platform_android::generate_window_handle() {
        return base::make_unique<window_android>(*this);
    }

    window_android::window_android(platform_android& platform) : _platform(platform) {
        platform.window = this;
    }

    window_android::~window_android() {}

    void window_android::set_title(const char* title) {
        spargel_log_debug("TODO: window_android::set_title");
    }

    window_handle window_android::handle() {
        window_handle handle{};
        handle.android.window = _platform._app->window;
        return handle;
    }

}  // namespace spargel::ui
