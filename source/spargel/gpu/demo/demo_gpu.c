#include <spargel/gpu/gpu.h>
#include <spargel/gpu/gpu_metal.h>
#include <spargel/gpu/gpu_vulkan.h>
#include <spargel/ui/ui.h>

#define USE_VULKAN 1
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

int main() {
    // sui_init_platform();
    // sui_window_id window = sui_create_window(500, 500);
    // sui_window_set_title(window, "Spargel Demo - GPU");

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

    struct sgpu_render_pipeline_descriptor pipeline_desc = {
        // .primitive = SGPU_PRIMITIVE_TRIANGLE,
        .target_format = SGPU_FORMAT_BRGA8_UNORM,
        .vertex_function = vertex_func,
        .fragment_function = fragment_func,
    };

    sgpu_render_pipeline_id pipeline;
    result = sgpu_create_render_pipeline(device, &pipeline_desc, &pipeline);
    if (result != 0) goto fail_pipeline;
    sbase_log_info("pipeline created");

    // sui_platform_run();

    sgpu_destroy_render_pipeline(pipeline);
fail_pipeline:
    sgpu_destroy_shader_function(fragment_func);
fail_fragment_shader:
    sgpu_destroy_shader_function(vertex_func);
fail_vertex_shader:
#if USE_METAL
    sgpu_metal_destroy_shader_library(metal_library);
fail_shader_library:
#endif
    sgpu_destroy_command_queue(queue);
fail_queue:
    sgpu_destroy_device(device);
fail_device:
    sbase_report_allocation();
    sbase_check_leak();
    return result;
}
