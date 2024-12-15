#include <spargel/base/logging.h>
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

struct vertex_shader_info {
    static constexpr vertex_buffer_location position_buffer = {
        .apple = {0},
        .vulkan = {0},
    };
    static constexpr vertex_buffer_location color_buffer = {
        .apple = {1},
        .vulkan = {1},
    };
};

struct renderer final : public spargel::ui::window_delegate {
    void on_render() override {
        // task_graph graph(_device);
        // auto surface = graph.current_surface();
        // auto my_pass = graph.add_render_task();
        // my_pass->set_name("my_pass");
        // my_pass->add_write(surface, load_action::clear, store_action::store);
        // my_pass->set_execute([=](render_encoder& encoder) {
        //     encoder.set_pipeline();
        //     encoder.draw();
        //     surface;
        // });
        // graph.add_present_task(surface);
        // graph.execute();
        auto tex = _surface->next_texture();
        _device->begin_render_pass(tex);
        _device->set_viewport({0, 0, _surface->width(), _surface->height(), 0, 1});
        _device->set_render_pipeline(_pipeline);
        _device->set_vertex_buffer(_positions, vertex_shader_info::position_buffer);
        _device->set_vertex_buffer(_colors, vertex_shader_info::color_buffer);
        _device->draw(0, 6);
        _device->end_render_pass();
        _device->present(_surface);
    }
    device* _device;
    object_ptr<render_pipeline> _pipeline;
    object_ptr<buffer> _positions;
    object_ptr<buffer> _colors;
    object_ptr<surface> _surface;
};

struct vertex_position {
    float x;
    float y;
};
struct vertex_color {
    float r;
    float g;
    float b;
    float a;
};
static constexpr vertex_position positions[] = {
    {-0.5, -0.5}, {0.5, 0.5}, {0.5, -0.5}, {-0.5, -0.5}, {-0.5, 0.5}, {0.5, 0.5},
};
static constexpr vertex_color colors[] = {
    {1.0, 0.0, 0.0, 1.0}, {0.0, 1.0, 0.0, 1.0}, {0.0, 0.0, 1.0, 1.0},
    {0.0, 1.0, 0.0, 1.0}, {1.0, 0.0, 0.0, 1.0}, {0.0, 0.0, 1.0, 1.0},
};

int main() {
    auto platform = spargel::ui::make_platform();
    auto window = platform->make_window(500, 500);
    window->set_title("Spargel Engine - GPU3");

    renderer r;
    window->set_delegate(&r);

    auto device = make_device(device_kind::metal);

    auto shader = device->make_shader_library({
        .bytes = spargel::base::make_span<u8>(demo_gpu_metal_shader_code),
    });

    auto pipeline = device->make_render_pipeline({
        .primitive = primitive_kind::triangle,
        //.front_face = orientation::clockwise,
        //.cull = cull_mode::back,
        .vertex_shader =
            {
                .library = shader,
                .entry = "vertex_shader",
            },
        .vertex_buffers =
            {
                // positions
                {
                    .stride = sizeof(vertex_position),
                    .step_mode = vertex_step_mode::vertex,
                },
                // colors
                {
                    .stride = sizeof(vertex_color),
                    .step_mode = vertex_step_mode::vertex,
                },
            },
        .vertex_attributes =
            {
                {
                    .buffer = 0,
                    .location = 0,
                    .format = vertex_attribute_format::float32x2,
                    .offset = 0,
                },
                {
                    .buffer = 1,
                    .location = 1,
                    .format = vertex_attribute_format::float32x4,
                    .offset = 0,
                },
            },
        .fragment_shader =
            {
                .library = shader,
                .entry = "fragment_shader",
            },
    });

    auto position_buffer = device->make_buffer_with_bytes(
        spargel::base::make_span<u8>(sizeof(positions), (u8*)positions));
    auto color_buffer =
        device->make_buffer_with_bytes(spargel::base::make_span<u8>(sizeof(colors), (u8*)colors));

    r._device = device.get();
    r._pipeline = pipeline;
    r._positions = position_buffer;
    r._colors = color_buffer;
    r._surface = device->make_surface(window.get());

    // device->destroy_buffer(buffer);
    // device->destroy_render_pipeline(pipeline);
    // device->destroy_shader_library(shader);

    platform->start_loop();
    return 0;
}
