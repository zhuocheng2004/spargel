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

int main() {
    sui_init_platform();

    int result;

#if USE_VULKAN
    sui_window_id window_vk = sui_create_window(500, 500);
    sui_window_set_title(window_vk, "Spargel Demo - Vulkan");
    sgpu_device_id device_vk;
    {
        struct sgpu_device_descriptor desc = {
            .backend = SGPU_BACKEND_VULKAN,
            .platform = sui_platform_id(),
        };
        result = sgpu_create_default_device(&desc, &device_vk);
    }
    if (result != 0) return 1;
    sbase_log_info("device created");

    sgpu_command_queue_id queue_vk;
    result = sgpu_create_command_queue(device_vk, &queue_vk);
    if (result != 0) return 1;
    sbase_log_info("command queue created");

    sgpu_surface_id surface_vk;
    {
        struct sgpu_surface_descriptor desc = {
            .window = window_vk,
        };
        result = sgpu_create_surface(device_vk, &desc, &surface_vk);
    }
    if (result != 0) return 1;
    sbase_log_info("surface created");

    sgpu_swapchain_id swapchain_vk;
    {
        struct sgpu_swapchain_descriptor desc = {
            .surface = surface_vk,
            .width = 500,
            .height = 500,
        };
        result = sgpu_create_swapchain(device_vk, &desc, &swapchain_vk);
    }
    if (result != 0) return 1;
    sbase_log_info("swapchain created");

    sgpu_command_buffer_id cmdbuf_vk;
    {
        struct sgpu_command_buffer_descriptor desc = {
            .queue = queue_vk,
        };
        result = sgpu_create_command_buffer(device_vk, &desc, &cmdbuf_vk);
    }
    if (result != 0) return 1;
    sbase_log_info("command buffer created");

    struct renderer data_vk = {
        .device = device_vk,
        .swapchain = swapchain_vk,
        .queue = queue_vk,
        .cmdbuf = cmdbuf_vk,
        .frame = 0,
    };
    sui_window_set_render_callback(window_vk, (void (*)(void*))render, &data_vk);
#endif

#if USE_METAL
    sui_window_id window_mtl = sui_create_window(500, 500);
    sui_window_set_title(window_mtl, "Spargel Demo - Metal");
    sgpu_device_id device_mtl;
    {
        struct sgpu_device_descriptor desc = {
            .backend = SGPU_BACKEND_VULKAN,
            .platform = sui_platform_id(),
        };
        result = sgpu_create_default_device(&desc, &device_mtl);
    }
    if (result != 0) return 1;
    sbase_log_info("device created");

    sgpu_command_queue_id queue_mtl;
    result = sgpu_create_command_queue(device_mtl, &queue_mtl);
    if (result != 0) return 1;
    sbase_log_info("command queue created");

    sgpu_surface_id surface_mtl;
    {
        struct sgpu_surface_descriptor desc = {
            .window = window_mtl,
        };
        result = sgpu_create_surface(device_mtl, &desc, &surface_mtl);
    }
    if (result != 0) return 1;
    sbase_log_info("surface created");

    sgpu_swapchain_id swapchain_mtl;
    {
        struct sgpu_swapchain_descriptor desc = {
            .surface = surface_mtl,
            .width = 500,
            .height = 500,
        };
        result = sgpu_create_swapchain(device_mtl, &desc, &swapchain_mtl);
    }
    if (result != 0) return 1;
    sbase_log_info("swapchain created");

    sgpu_command_buffer_id cmdbuf_mtl;
    {
        struct sgpu_command_buffer_descriptor desc = {
            .queue = queue_mtl,
        };
        result = sgpu_create_command_buffer(device_mtl, &desc, &cmdbuf_mtl);
    }
    if (result != 0) return 1;
    sbase_log_info("command buffer created");

    struct renderer data_mtl = {
        .device = device_mtl,
        .swapchain = swapchain_mtl,
        .queue = queue_mtl,
        .cmdbuf = cmdbuf_mtl,
        .frame = 0,
    };
    sui_window_set_render_callback(window_mtl, (void (*)(void*))render, &data_mtl);
#endif

    sui_platform_run();
    return 0;
}
