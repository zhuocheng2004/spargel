#include <math.h>
#include <spargel/gpu/gpu.h>
#include <spargel/gpu/gpu_metal.h>
#include <spargel/gpu/gpu_vulkan.h>
#include <spargel/ui/ui.h>
#include <stdlib.h>

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
    sgpu_device_id device;
    sgpu_surface_id surface;
    sgpu_swapchain_id swapchain;
    sgpu_command_queue_id queue;
    sgpu_command_buffer_id cmdbuf;
    int frame;
};

static void render(struct renderer* r) {
    sgpu_device_id device = r->device;
    sgpu_swapchain_id swapchain = r->swapchain;
    // sgpu_command_queue_id queue = r->queue;
    sgpu_command_buffer_id cmdbuf = r->cmdbuf;

    sgpu_reset_command_buffer(device, cmdbuf);

    sgpu_presentable_id presentable;
    sgpu_texture_id texture;
    sgpu_acquire_image(device, &(struct sgpu_acquire_descriptor){.swapchain = swapchain},
                       &presentable);
    sgpu_presentable_texture(device, presentable, &texture);
    sgpu_render_pass_encoder_id encoder;
    sgpu_begin_render_pass(
        device,
        &(struct sgpu_render_pass_descriptor){
            .command_buffer = cmdbuf,
            .color_attachment = texture,
            .clear_color = {fabs(sin(r->frame / 120.f + 72)), fabs(sin(r->frame / 120.f + 36)),
                            fabs(sin(r->frame / 120.f)), 1.0},
            .swapchain = swapchain,
        },
        &encoder);
    sgpu_end_render_pass(device, encoder);
    {
        struct sgpu_present_descriptor desc = {
            .command_buffer = cmdbuf,
            .presentable = presentable,
        };
        sgpu_present(device, &desc);
    }
    r->frame++;
}

static int create(int backend, char const* title, struct renderer* r) {
    sui_window_id window = sui_create_window(500, 500);
    sui_window_set_title(window, title);

    int result;

    sgpu_device_id device;
    {
        struct sgpu_device_descriptor desc = {
            .backend = SGPU_BACKEND_VULKAN,
            .platform = sui_platform_id(),
        };
        result = sgpu_create_default_device(&desc, &device);
    }
    if (result != 0) return 1;
    sbase_log_info("device created");

    sgpu_command_queue_id queue;
    result = sgpu_create_command_queue(device, &queue);
    if (result != 0) return 1;
    sbase_log_info("command queue created");

    sgpu_surface_id surface;
    {
        struct sgpu_surface_descriptor desc = {
            .window = window,
        };
        result = sgpu_create_surface(device, &desc, &surface);
    }
    if (result != 0) return 1;
    sbase_log_info("surface created");

    sgpu_swapchain_id swapchain;
    {
        struct sgpu_swapchain_descriptor desc = {
            .surface = surface,
            .width = 500,
            .height = 500,
        };
        result = sgpu_create_swapchain(device, &desc, &swapchain);
    }
    if (result != 0) return 1;
    sbase_log_info("swapchain created");

    sgpu_command_buffer_id cmdbuf;
    {
        struct sgpu_command_buffer_descriptor desc = {
            .queue = queue,
        };
        result = sgpu_create_command_buffer(device, &desc, &cmdbuf);
    }
    if (result != 0) return 1;
    sbase_log_info("command buffer created");

    r->device = device;
    r->queue = queue;
    r->surface = surface;
    r->swapchain = swapchain;
    r->cmdbuf = cmdbuf;
    r->frame = 0;

    sui_window_set_render_callback(window, (void (*)(void*))render, r);

    return 0;
}

int main() {
    sui_init_platform();

#if USE_VULKAN
    struct renderer r_vk;
    if (create(SGPU_BACKEND_VULKAN, "Spargel Demo - Vulkan", &r_vk) != 0) return 1;
#endif
#if USE_METAL
    struct renderer r_mtl;
    if (create(SGPU_BACKEND_VULKAN, "Spargel Demo - Metal", &r_mtl) != 0) return 1;
#endif

    sui_platform_run();
    return 0;
}
