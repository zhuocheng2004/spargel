#include <spargel/gpu/gpu.h>
#include <spargel/ui/ui.h>
#include <stdio.h>
#include <stdlib.h>

static bool load_file(char const* path, void** data, ssize* size)
{
    FILE* file = fopen(path, "rb");
    if (!file) return false;
    fseek(file, 0, SEEK_END);
    ssize len = ftell(file);
    *data = malloc(len);
    fseek(file, 0, SEEK_SET);
    fread(*data, len, 1, file);
    *size = len;
    return true;
}

int main()
{
    spargel_ui_init_platform();
    spargel_ui_window_id window = spargel_ui_create_window(500, 500);
    spargel_ui_window_set_title(window, "Spargel Demo - GPU");

    int result = 0;

#if __APPLE__
    int gpu_backend = SGPU_BACKEND_METAL;
#else
    int gpu_backend = SGPU_BACKEND_VULKAN;
#endif

    sgpu_device_id device;
    result = sgpu_create_default_device(gpu_backend, &device);
    if (result != 0) goto cleanup_nothing;
    printf("info: device created\n");

    sgpu_command_queue_id queue;
    result = sgpu_create_command_queue(device, &queue);
    if (result != 0) goto cleanup_device;
    printf("info: command queue created\n");

    void* metal_library_data;
    ssize metal_library_size;
    if (!load_file("source/spargel/renderer/shader.metallib",
                   &metal_library_data, &metal_library_size)) {
        printf("error: cannot load metal shaders\n");
        result = 1;
        goto cleanup_queue;
    }
    sgpu_metal_shader_library_id metal_library;
    struct sgpu_metal_shader_library_descriptor sl_desc;
    sl_desc = {metal_library_data, metal_library_size};
    result = sgpu_create_metal_shader_library(device, &sl_desc, &metal_library);
    if (result != 0) goto cleanup_queue;
    printf("info: metal shader library created\n");

    sgpu_shader_function_id vertex_func;
    struct sgpu_shader_function_descriptor sf_desc;
    sf_desc.metal.library = metal_library;
    sf_desc.metal.name = "vertex_shader";
    result = sgpu_create_shader_function(device, &sf_desc, &vertex_func);
    if (result != 0) goto cleanup_shader_library;
    printf("info: vertex shader function created\n");

    // spargel_ui_platform_run();

    sgpu_destroy_shader_function(vertex_func);
cleanup_shader_library:
    sgpu_destroy_metal_shader_library(metal_library);
cleanup_queue:
    sgpu_destroy_command_queue(queue);
cleanup_device:
    sgpu_destroy_device(device);
cleanup_nothing:
    return result;
}
