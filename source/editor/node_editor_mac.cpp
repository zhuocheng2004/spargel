#include <spargel/base/logging.h>
#include <spargel/entry/component.h>
#include <spargel/entry/launch_data.h>
#include <spargel/ui/ui.h>

// platform
#include <dlfcn.h>

namespace spargel::editor::stub {

    class window final : public spargel::ui::window {
    public:
        void set_title(char const* title) override {
            spargel_log_info("game component sets title <<%s>>", title);
        }

        spargel::ui::window_handle handle() override { return {.apple = {.layer = nullptr}}; }
    };

    class platform final : public spargel::ui::platform {
    public:
        platform(spargel::ui::platform* real) : spargel::ui::platform(real->kind()) {}

        void start_loop() {}

        base::unique_ptr<spargel::ui::window> make_window(u32 width, u32 height) {
            return base::make_unique<window>();
        }

    private:
    };

}  // namespace spargel::editor::stub

int main(int argc, char* argv[]) {
    if (argc < 2) {
        spargel_log_error("missing game component");
        return 1;
    }
    char* path = argv[1];
    spargel_log_info("loading game component <<%s>>", path);

    auto lib = dlopen(path, RTLD_NOW | RTLD_LOCAL);
    auto maker = reinterpret_cast<spargel::entry::component* (*)(spargel::entry::launch_data*)>(
        dlsym(lib, "_spargel_make_component"));

    spargel::entry::launch_data data;
    auto real_platform = spargel::ui::make_platform();
    auto stub_platform =
        spargel::base::make_unique<spargel::editor::stub::platform>(real_platform.get());
    data.platform = stub_platform.get();

    auto component = maker(&data);
    component->on_load();

    stub_platform->start_loop();

    dlclose(lib);
    return 0;
}
