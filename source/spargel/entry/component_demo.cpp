#include <spargel/base/allocator.h>
#include <spargel/base/logging.h>
#include <spargel/base/object.h>
#include <spargel/base/unique_ptr.h>
#include <spargel/entry/component.h>
#include <spargel/entry/launch_data.h>
#include <spargel/ui/ui.h>

#define DEMO_EXPORT [[gnu::visibility("default")]]

class DEMO_EXPORT demo_component final : public spargel::entry::component {
public:
    explicit demo_component(spargel::entry::launch_data* l) : spargel::entry::component(l) {}

    void on_load() override {
        _window = get_launch_data()->platform->make_window(500, 500);
        _window->set_title("Spargel - Component Demo");
        spargel_log_info("demo component loaded");
    }

private:
    spargel::base::unique_ptr<spargel::ui::window> _window;
};

extern "C" DEMO_EXPORT spargel::entry::component* _spargel_make_component(spargel::entry::launch_data* l) {
    demo_component* ptr = static_cast<demo_component*>(
        spargel::base::default_allocator()->alloc(sizeof(demo_component)));
    spargel::base::construct_at<demo_component>(ptr, l);
    return ptr;
}
