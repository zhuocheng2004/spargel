#include <spargel/gpu/gpu.h>
#include <spargel/gpu/gpu_metal.h>
#include <spargel/gpu/gpu_vulkan.h>
#include <spargel/ui/ui.h>

/* libc */
#include <stdio.h>

#define USE_VULKAN 1

static bool load_file(char const* path, void** data, ssize* size) {
    FILE* file = fopen(path, "rb");
    if (!file) return false;
    fseek(file, 0, SEEK_END);
    ssize len = ftell(file);
    *data = sbase_allocate(len, SBASE_ALLOCATION_GPU);
    fseek(file, 0, SEEK_SET);
    fread(*data, len, 1, file);
    *size = len;
    return true;
}

int main() {
    sui_init_platform();
    sui_window_id window = sui_create_window(500, 500);
    ui_window_set_title(window, "Spargel Demo - GPU");

    int result = 0;

#if USE_VULKAN
    int gpu_backend = SGPU_BACKEND_VULKAN;
#elif __APPLE__
    int gpu_backend = SGPU_BACKEND_METAL;
#endif

    sgpu_device_id device;
    result = sgpu_create_default_device(gpu_backend, &device);
    if (result != 0) goto cleanup_nothing;
    printf("info: device created\n");

    sgpu_command_queue_id queue;
    result = sgpu_create_command_queue(device, &queue);
    if (result != 0) goto cleanup_device;
    printf("info: command queue created\n");

#if __APPLE__
    void* metal_library_data;
    ssize metal_library_size;
    if (!load_file("source/spargel/renderer/shader.metallib", &metal_library_data,
                   &metal_library_size)) {
        printf("error: cannot load metal shaders\n");
        result = 1;
        goto cleanup_queue;
    }

    sgpu_metal_shader_library_id metal_library;
    result = sgpu_metal_create_shader_library(device,
                                              &(struct sgpu_metal_shader_library_descriptor){
                                                  .code = metal_library_data,
                                                  .size = metal_library_size,
                                              },
                                              &metal_library);
    if (result != 0) goto cleanup_queue;
    printf("info: metal shader library created\n");
#endif

    sgpu_shader_function_id vertex_func;

#if __APPLE__
    result = sgpu_metal_create_shader_function(device,
                                               &(struct sgpu_metal_shader_function_descriptor){
                                                   .library = metal_library,
                                                   .name = "vertex_shader",
                                               },
                                               &vertex_func);
#else
    result = sgpu_vulkan_create_shader_function(device,
                                                &(struct sgpu_vulkan_shader_function_descriptor){
                                                    .code = NULL,
                                                    .size = 0,
                                                },
                                                &vertex_func);
#endif
    if (result != 0) goto cleanup_shader_library;
    printf("info: vertex shader function created\n");

    // spargel_ui_platform_run();

    sgpu_destroy_shader_function(vertex_func);
cleanup_shader_library:
#if __APPLE__
    sgpu_metal_destroy_shader_library(metal_library);
    sbase_deallocate(metal_library_data, metal_library_size, SBASE_ALLOCATION_GPU);
#endif
cleanup_queue:
    sgpu_destroy_command_queue(queue);
cleanup_device:
    sgpu_destroy_device(device);
cleanup_nothing:

    sbase_report_allocation();
    sbase_check_leak();
    return result;
}
