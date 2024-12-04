#include <spargel/base/base.h>
#include <spargel/gpu/gpu.h>
#include <spargel/gpu/operations.h>

#if SPARGEL_GPU_ENABLE_METAL
extern const struct sgpu_operations sgpu_mtl_operations;
#endif

#if SPARGEL_GPU_ENABLE_VULKAN
extern const struct sgpu_operations sgpu_vk_operations;
#endif

#if SPARGEL_GPU_ENABLE_DIRECTX
extern const struct sgpu_operations sgpu_dx_operations;
#endif

int sgpu_create_default_device(int backend, sgpu_device_id* device) {
    switch (backend) {
#if SPARGEL_GPU_ENABLE_METAL
    case SGPU_BACKEND_METAL:
        return sgpu_mtl_operations.create_default_device(device);
#endif
#if SPARGEL_GPU_ENABLE_VULKAN
    case SGPU_BACKEND_VULKAN:
        return sgpu_vk_operations.create_default_device(device);
#endif
#if SPARGEL_GPU_ENABLE_DIRECTX
    case SGPU_BACKEND_DIRECTX:
        return sgpu_dx_operations.create_default_device(device);
#endif
    default:
        sbase_panic();
    }
}

#define OPS(object) (*((struct sgpu_operations const**)object))

void sgpu_destroy_device(sgpu_device_id device) { OPS(device)->destroy_device(device); }

int sgpu_create_command_queue(sgpu_device_id device, sgpu_command_queue_id* queue) {
    return OPS(device)->create_command_queue(device, queue);
}

void sgpu_destroy_command_queue(sgpu_command_queue_id queue) {
    OPS(queue)->destroy_command_queue(queue);
}

int sgpu_create_shader_function(sgpu_device_id device,
                                struct sgpu_shader_function_descriptor const* descriptor,
                                sgpu_shader_function_id* func) {
    return OPS(device)->create_shader_function(device, descriptor, func);
}

void sgpu_destroy_shader_function(sgpu_shader_function_id func) {
    OPS(func)->destroy_shader_function(func);
}

int sgpu_create_metal_shader_library(sgpu_device_id device,
                                     struct sgpu_metal_shader_library_descriptor const* descriptor,
                                     sgpu_metal_shader_library_id* library) {
    return OPS(device)->create_metal_shader_library(device, descriptor, library);
}

void sgpu_destroy_metal_shader_library(sgpu_metal_shader_library_id library) {
    return OPS(library)->destroy_metal_shader_library(library);
}

int sgpu_create_render_pipeline(sgpu_device_id device,
                                struct sgpu_render_pipeline_descriptor const* descriptor,
                                sgpu_render_pipeline_id* pipeline);

void sgpu_destroy_render_pipeline(sgpu_render_pipeline_id pipeline);

int sgpu_create_command_buffer(sgpu_command_queue_id queue, sgpu_command_buffer_id* command_buffer);

void sgpu_destroy_command_buffer(sgpu_command_buffer_id command_buffer);
