#include <spargel/gpu/gpu.h>
#include <spargel/gpu/gpu_metal.h>
#include <spargel/gpu/gpu_vulkan.h>
#include <spargel/ui/ui.h>

#define USE_VULKAN 0
#if SPARGEL_IS_MACOS && !USE_VULKAN
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
};

static void render(struct renderer* r) {
    sgpu_device_id device = r->device;
    sgpu_swapchain_id swapchain = r->swapchain;
    sgpu_command_queue_id queue = r->queue;

    sgpu_command_buffer_id cmdbuf;
    {
        struct sgpu_command_buffer_descriptor desc = {
            .queue = queue,
        };
        int result = sgpu_create_command_buffer(device, &desc, &cmdbuf);
        if (result != 0) {
            sbase_log_info("command buffer created");
            return;
        }
    }

    sgpu_presentable_id presentable;
    sgpu_texture_id texture;
    sgpu_acquire_image(device, &(struct sgpu_acquire_descriptor){.swapchain = swapchain},
                       &presentable);
    sgpu_presentable_texture(device, presentable, &texture);
    sgpu_render_pass_encoder_id encoder;
    sgpu_begin_render_pass(device,
                           &(struct sgpu_render_pass_descriptor){
                               .command_buffer = cmdbuf,
                               .color_attachment = texture,
                               .clear_color = {0.0, 1.0, 1.0, 1.0},
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
}

int main() {
    sui_init_platform();
    sui_window_id window = sui_create_window(500, 500);
    sui_window_set_title(window, "Spargel Demo - GPU");

    int result = 0;

#if USE_VULKAN
    int gpu_backend = SGPU_BACKEND_VULKAN;
#elif USE_METAL
    int gpu_backend = SGPU_BACKEND_METAL;
#endif

    sgpu_device_id device;
    {
        struct sgpu_device_descriptor desc = {
            .backend = gpu_backend,
            .platform = sui_platform_id(),
        };
        result = sgpu_create_default_device(&desc, &device);
    }
    if (result != 0) goto fail_device;
    sbase_log_info("device created");

    sgpu_command_queue_id queue;
    result = sgpu_create_command_queue(device, &queue);
    if (result != 0) goto fail_queue;
    sbase_log_info("command queue created");

#if USE_METAL
    sgpu_metal_shader_library_id metal_library;
    {
        struct sgpu_metal_shader_library_descriptor desc = {
            .code = demo_gpu_metal_shader_code,
            .size = sizeof(demo_gpu_metal_shader_code),
        };
        result = sgpu_metal_create_shader_library(device, &desc, &metal_library);
    }
    if (result != 0) goto fail_shader_library;
    sbase_log_info("metal shader library created");
#endif

    sgpu_shader_function_id vertex_func;
    sgpu_shader_function_id fragment_func;

#if USE_VULKAN
    {
        struct sgpu_vulkan_shader_function_descriptor desc = {
            .code = demo_gpu_vulkan_vertex_code,
            .size = sizeof(demo_gpu_vulkan_vertex_code),
        };
        result = sgpu_vulkan_create_shader_function(device, &desc, &vertex_func);
    }
    if (result != 0) goto fail_vertex_shader;
    sbase_log_info("vertex shader created");
    {
        struct sgpu_vulkan_shader_function_descriptor desc = {
            .code = demo_gpu_vulkan_fragment_code,
            .size = sizeof(demo_gpu_vulkan_fragment_code),
        };
        result = sgpu_vulkan_create_shader_function(device, &desc, &fragment_func);
    }
    if (result != 0) goto fail_fragment_shader;
    sbase_log_info("fragment shader created");
#elif USE_METAL
    {
        struct sgpu_metal_shader_function_descriptor desc = {
            .library = metal_library,
            .name = "vertex_shader",
        };
        result = sgpu_metal_create_shader_function(device, &desc, &vertex_func);
    }
    if (result != 0) goto fail_vertex_shader;
    sbase_log_info("vertex shader created");
    {
        struct sgpu_metal_shader_function_descriptor desc = {
            .library = metal_library,
            .name = "fragment_shader",
        };
        result = sgpu_metal_create_shader_function(device, &desc, &fragment_func);
    }
    if (result != 0) goto fail_fragment_shader;
    sbase_log_info("fragment shader created");
#else
#error unimplemented
#endif

    sgpu_render_pipeline_id pipeline;
    {
        struct sgpu_render_pipeline_descriptor desc = {
            // .primitive = SGPU_PRIMITIVE_TRIANGLE,
            .target_format = SGPU_FORMAT_BRGA8_UNORM,
            .vertex_function = vertex_func,
            .fragment_function = fragment_func,
        };
        result = sgpu_create_render_pipeline(device, &desc, &pipeline);
    }
    if (result != 0) goto fail_pipeline;
    sbase_log_info("pipeline created");

    sgpu_surface_id surface;
    {
        struct sgpu_surface_descriptor desc = {
            .window = window,
        };
        result = sgpu_create_surface(device, &desc, &surface);
    }
    if (result != 0) goto fail_surface;
    sbase_log_info("surface created");

    sgpu_swapchain_id swapchain;
    {
        struct sgpu_swapchain_descriptor desc = {
            .surface = surface,
        };
        result = sgpu_create_swapchain(device, &desc, &swapchain);
    }
    if (result != 0) goto fail_swapchain;
    sbase_log_info("swapchain created");

    struct renderer data = {
        .device = device,
        .swapchain = swapchain,
        .queue = queue,
    };
    sui_window_set_render_callback(window, (void (*)(void*))render, &data);

    sui_platform_run();

    sgpu_destroy_swapchain(device, swapchain);
fail_swapchain:
    sgpu_destroy_surface(device, surface);
fail_surface:
    sgpu_destroy_render_pipeline(device, pipeline);
fail_pipeline:
    sgpu_destroy_shader_function(device, fragment_func);
fail_fragment_shader:
    sgpu_destroy_shader_function(device, vertex_func);
fail_vertex_shader:
#if USE_METAL
    sgpu_metal_destroy_shader_library(device, metal_library);
fail_shader_library:
#endif
    sgpu_destroy_command_queue(device, queue);
fail_queue:
    sgpu_destroy_device(device);
fail_device:
    sbase_report_allocation();
    sbase_check_leak();
    return result;
}
