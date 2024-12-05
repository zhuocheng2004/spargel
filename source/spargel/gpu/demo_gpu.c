#include <spargel/gpu/gpu.h>
#include <spargel/gpu/gpu_metal.h>
#include <spargel/gpu/gpu_vulkan.h>
#include <spargel/ui/ui.h>

/* libc */
#include <stdio.h>

#define USE_VULKAN 1

// static bool load_file(char const* path, void** data, ssize* size) {
//     FILE* file = fopen(path, "rb");
//     if (!file) return false;
//     fseek(file, 0, SEEK_END);
//     ssize len = ftell(file);
//     *data = sbase_allocate(len, SBASE_ALLOCATION_GPU);
//     fseek(file, 0, SEEK_SET);
//     fread(*data, len, 1, file);
//     *size = len;
//     return true;
// }

int main() {
    // sui_init_platform();
    // sui_window_id window = sui_create_window(500, 500);
    // sui_window_set_title(window, "Spargel Demo - GPU");

    int result = 0;

#if USE_VULKAN
    int gpu_backend = SGPU_BACKEND_VULKAN;
#elif SPARGEL_IS_MACOS
    int gpu_backend = SGPU_BACKEND_METAL;
#endif

    sgpu_device_id device;
    result = sgpu_create_default_device(
        &(struct sgpu_device_descriptor){
            .backend = gpu_backend,
            .platform = sui_platform_id(),
        },
        &device);
    if (result != 0) goto cleanup_nothing;
    sbase_log_info("device created");

    sgpu_command_queue_id queue;
    result = sgpu_create_command_queue(device, &queue);
    if (result != 0) goto cleanup_device;
    sbase_log_info("command queue created");

#if SPARGEL_IS_MACOS && !USE_VULKAN
    void* metal_library_data;
    ssize metal_library_size;
    if (!load_file("source/spargel/renderer/shader.metallib", &metal_library_data,
                   &metal_library_size)) {
        sbase_log_fatal("cannot load metal shaders");
        sbase_panic_here();
    }

    sgpu_metal_shader_library_id metal_library;
    result = sgpu_metal_create_shader_library(device,
                                              &(struct sgpu_metal_shader_library_descriptor){
                                                  .code = metal_library_data,
                                                  .size = metal_library_size,
                                              },
                                              &metal_library);
    if (result != 0) goto cleanup_queue;
    sbase_log_info("metal shader library created");
#endif

    sgpu_shader_function_id vertex_func;

#if USE_VULKAN
    result = sgpu_vulkan_create_shader_function(device,
                                                &(struct sgpu_vulkan_shader_function_descriptor){
                                                    .code = NULL,
                                                    .size = 0,
                                                },
                                                &vertex_func);
#elif SPARGEL_IS_MACOS
    result = sgpu_metal_create_shader_function(device,
                                               &(struct sgpu_metal_shader_function_descriptor){
                                                   .library = metal_library,
                                                   .name = "vertex_shader",
                                               },
                                               &vertex_func);
#else
#error unimplemented
#endif
    if (result != 0) goto cleanup_shader_library;
    sbase_log_info("vertex shader function created");

    // sui_platform_run();

    sgpu_destroy_shader_function(vertex_func);
cleanup_shader_library:
#if SPARGEL_IS_MACOS && !USE_VULKAN
    sgpu_metal_destroy_shader_library(metal_library);
    sbase_deallocate(metal_library_data, metal_library_size, SBASE_ALLOCATION_GPU);
cleanup_queue:
#endif
    sgpu_destroy_command_queue(queue);
cleanup_device:
    sgpu_destroy_device(device);
cleanup_nothing:

    sbase_report_allocation();
    sbase_check_leak();
    return result;
}
