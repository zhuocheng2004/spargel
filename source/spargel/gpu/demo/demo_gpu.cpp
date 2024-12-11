#include <spargel/gpu/gpu.h>
#include <spargel/gpu/gpu_metal.h>
#include <spargel/gpu/gpu_vulkan.h>
#include <spargel/ui/ui.h>

/* libc */
#include <math.h>

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

struct renderer {
    spargel::gpu::device_id device;
    spargel::gpu::surface_id surface;
    spargel::gpu::swapchain_id swapchain;
    spargel::gpu::command_queue_id queue;
    spargel::gpu::command_buffer_id cmdbuf;
    int frame;
};

static void render(struct renderer* r) {
    spargel::gpu::device_id device = r->device;
    spargel::gpu::swapchain_id swapchain = r->swapchain;
    // spargel::gpu::command_queue_id queue = r->queue;
    spargel::gpu::command_buffer_id cmdbuf = r->cmdbuf;

    spargel::gpu::reset_command_buffer(device, cmdbuf);

    spargel::gpu::presentable_id presentable;
    spargel::gpu::texture_id texture;
    struct spargel::gpu::acquire_descriptor adesc = {.swapchain = swapchain};
    spargel::gpu::acquire_image(device, &adesc, &presentable);
    spargel::gpu::presentable_texture(device, presentable, &texture);
    spargel::gpu::render_pass_encoder_id encoder;
    struct spargel::gpu::render_pass_descriptor rpdesc = {
        .command_buffer = cmdbuf,
        .color_attachment = texture,
        .clear_color = {fabs(sin(r->frame / 120.f + 72)), fabs(sin(r->frame / 120.f + 36)),
                        fabs(sin(r->frame / 120.f)), 1.0},
        .swapchain = swapchain,
    };
    spargel::gpu::begin_render_pass(device, &rpdesc, &encoder);
    spargel::gpu::end_render_pass(device, encoder);
    {
        struct spargel::gpu::present_descriptor desc = {
            .command_buffer = cmdbuf,
            .presentable = presentable,
        };
        spargel::gpu::present(device, &desc);
    }
    r->frame++;
}

static int create(int backend, char const* title, struct renderer* r) {
    spargel::ui::window_id window = spargel::ui::create_window(500, 500);
    spargel::ui::window_set_title(window, title);

    int result;

    spargel::gpu::device_id device;
    {
        struct spargel::gpu::device_descriptor desc = {
            .backend = spargel::gpu::BACKEND_VULKAN,
            .platform = spargel::ui::platform_id(),
        };
        result = spargel::gpu::create_default_device(&desc, &device);
    }
    if (result != 0) return 1;
    spargel_log_info("device created");

    spargel::gpu::command_queue_id queue;
    result = spargel::gpu::create_command_queue(device, &queue);
    if (result != 0) return 1;
    spargel_log_info("command queue created");

    spargel::gpu::surface_id surface;
    {
        struct spargel::gpu::surface_descriptor desc = {
            .window = window,
        };
        result = spargel::gpu::create_surface(device, &desc, &surface);
    }
    if (result != 0) return 1;
    spargel_log_info("surface created");

    spargel::gpu::swapchain_id swapchain;
    {
        struct spargel::gpu::swapchain_descriptor desc = {
            .surface = surface,
            .width = 500,
            .height = 500,
        };
        result = spargel::gpu::create_swapchain(device, &desc, &swapchain);
    }
    if (result != 0) return 1;
    spargel_log_info("swapchain created");

    spargel::gpu::command_buffer_id cmdbuf;
    {
        struct spargel::gpu::command_buffer_descriptor desc = {
            .queue = queue,
        };
        result = spargel::gpu::create_command_buffer(device, &desc, &cmdbuf);
    }
    if (result != 0) return 1;
    spargel_log_info("command buffer created");

    r->device = device;
    r->queue = queue;
    r->surface = surface;
    r->swapchain = swapchain;
    r->cmdbuf = cmdbuf;
    r->frame = 0;

    window_set_render_callback(window, (void (*)(void*))render, r);

    return 0;
}

int main() {
    spargel::ui::init_platform();

#if USE_VULKAN
    struct renderer r_vk;
    if (create(spargel::gpu::BACKEND_VULKAN, "Spargel Demo - Vulkan", &r_vk) != 0) return 1;
#endif
#if USE_METAL
    struct renderer r_mtl;
    if (create(spargel::gpu::BACKEND_VULKAN, "Spargel Demo - Metal", &r_mtl) != 0) return 1;
#endif

    spargel::ui::platform_run();
    return 0;
}
