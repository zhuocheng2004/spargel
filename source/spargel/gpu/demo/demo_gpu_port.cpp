#include <spargel/base/logging.h>
#include <spargel/entry/simple.h>
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

struct renderer final : spargel::ui::window_delegate {
    void on_render() override {
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
            .clear_color = {fabs(sin(frame / 120.f + 72)), fabs(sin(frame / 120.f + 36)),
                            fabs(sin(frame / 120.f)), 1.0},
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
        frame++;
    }

    spargel::gpu::device_id device;
    spargel::gpu::surface_id surface;
    spargel::gpu::swapchain_id swapchain;
    spargel::gpu::command_queue_id queue;
    spargel::gpu::command_buffer_id cmdbuf;
    spargel::base::unique_ptr<spargel::ui::window> window;
    int frame;
};

static int create(int backend, char const* title, renderer* r, spargel::ui::platform* platform,
                  spargel::base::unique_ptr<spargel::ui::window> window) {
    window->set_title(title);
    window->set_delegate(r);

    int result;

    spargel::gpu::device_id device;
    {
        struct spargel::gpu::device_descriptor desc = {
            .backend = spargel::gpu::BACKEND_VULKAN,
            .platform = platform->kind(),
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
            .window = window.get(),
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
    r->window = spargel::base::move(window);

    return 0;
}

int spargel::entry::simple_entry(spargel::entry::simple_entry_data* data) {
    auto platform = spargel::base::move(data->platform);

#if USE_VULKAN
    renderer r_vk;
    if (create(spargel::gpu::BACKEND_VULKAN, "Spargel Demo - Vulkan", &r_vk, platform.get(),
               spargel::base::move(data->window)) != 0)
        return 1;
#endif
#if USE_METAL
    renderer r_mtl;
    if (create(spargel::gpu::BACKEND_VULKAN, "Spargel Demo - Metal", &r_mtl, platform.get(),
               spargel::base::move(data->window)) != 0)
        return 1;
#endif

    platform->start_loop();
    return 0;
}
