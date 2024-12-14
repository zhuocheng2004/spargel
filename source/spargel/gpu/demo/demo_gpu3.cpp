#include <spargel/gpu/gpu3.h>
#include <spargel/ui/ui.h>

using namespace spargel::gpu;

struct renderer final : public spargel::ui::window_delegate {
    void render() override {
        task_graph graph(_device);
        auto surface = graph.current_surface();
        auto my_pass = graph.add_render_task();
        my_pass->set_name("my_pass");
        my_pass->add_write(surface, load_action::clear, store_action::store);
        my_pass->set_execute([=](render_encoder& encoder) {
            encoder.set_pipeline();
            encoder.draw();
            surface;
        });
        graph.add_present_task(surface);
        graph.execute();
    }
    device* _device;
};

int main() {
    auto platform = spargel::ui::make_platform();
    auto window = platform->make_window(500, 500);
    window->set_title("Spargel Engine - GPU");

    renderer r;
    window->set_delegate(&r);

    platform->start_loop();
    return 0;
}
