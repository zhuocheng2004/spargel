#include <spargel/gpu/gpu3.h>
#include <spargel/ui/ui.h>

using namespace spargel::gpu;

#define USE_VULKAN 1
#if SPARGEL_IS_MACOS
#define USE_METAL 1
#else
#define USE_METAL 0
#endif

#if USE_METAL
#include <spargel/gpu/demo/shader.metallib.inc>
#endif

#if USE_VULKAN
#include <spargel/gpu/demo/fragment_shader.spirv.inc>
#include <spargel/gpu/demo/vertex_shader.spirv.inc>
#endif

struct renderer final : public spargel::ui::window_delegate {
    void on_render() override {
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

    auto device = make_device(device_kind::metal);

    auto shader = device->make_shader_library({
        .size = sizeof(demo_gpu_metal_shader_code),
        .bytes = demo_gpu_metal_shader_code,
    });

    device->destroy_shader_library(shader);

    // platform->start_loop();
    return 0;
}
