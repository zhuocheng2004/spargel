#include <spargel/base/base.h>
#include <spargel/gpu/gpu.h>

#if SPARGEL_GPU_ENABLE_METAL
#include <spargel/gpu/gpu_metal.h>
#endif

#if SPARGEL_GPU_ENABLE_VULKAN
#include <spargel/gpu/gpu_vulkan.h>
#endif

#if SPARGEL_GPU_ENABLE_DIRECTX
#endif

int sgpu_create_default_device(int backend, sgpu_device_id* device) {
    switch (backend) {
#if SPARGEL_GPU_ENABLE_METAL
    case SGPU_BACKEND_METAL:
        return sgpu_metal_create_default_device(device);
#endif
#if SPARGEL_GPU_ENABLE_VULKAN
    case SGPU_BACKEND_VULKAN:
        return sgpu_vulkan_create_default_device(device);
#endif
#if SPARGEL_GPU_ENABLE_DIRECTX
    case SGPU_BACKEND_DIRECTX:
        sbase_panic_here();
#endif
    default:
        sbase_panic_here();
    }
}

#if SPARGEL_GPU_ENABLE_METAL
#define MTL_DISPATCH(name, ...)         \
    case SGPU_BACKEND_METAL:            \
        sgpu_metal_##name(__VA_ARGS__); \
        break;
#define MTL_DISPATCH_R(name, ...) \
    case SGPU_BACKEND_METAL:      \
        return sgpu_metal_##name(__VA_ARGS__);
#else
#define MTL_DISPATCH(name, ...)
#define MTL_DISPATCH_R(name, ...)
#endif

#if SPARGEL_GPU_ENABLE_VULKAN
#define VK_DISPATCH(name, ...)           \
    case SGPU_BACKEND_VULKAN:            \
        sgpu_vulkan_##name(__VA_ARGS__); \
        break;
#define VK_DISPATCH_R(name, ...) \
    case SGPU_BACKEND_VULKAN:    \
        return sgpu_vulkan_##name(__VA_ARGS__);
#else
#define VK_DISPATCH(name, ...)
#define VK_DISPATCH_R(name, ...)
#endif

#define DISPATCH(name, obj, ...)        \
    switch (*(int*)(obj)) {             \
        MTL_DISPATCH(name, __VA_ARGS__) \
        VK_DISPATCH(name, __VA_ARGS__)  \
    default:                            \
        sbase_panic_here();             \
    }

#define DISPATCH_R(name, obj, ...)        \
    switch (*(int*)(obj)) {               \
        MTL_DISPATCH_R(name, __VA_ARGS__) \
        VK_DISPATCH_R(name, __VA_ARGS__)  \
    default:                              \
        sbase_panic_here();               \
    }

void sgpu_destroy_device(sgpu_device_id device) { DISPATCH(destroy_device, device, device); }

int sgpu_create_command_queue(sgpu_device_id device, sgpu_command_queue_id* queue) {
    DISPATCH_R(create_command_queue, device, device, queue);
}

void sgpu_destroy_command_queue(sgpu_command_queue_id queue) {
    DISPATCH(destroy_command_queue, queue, queue);
}

void sgpu_destroy_shader_function(sgpu_shader_function_id func) {
    DISPATCH(destroy_shader_function, func, func);
}

int sgpu_create_render_pipeline(sgpu_device_id device,
                                struct sgpu_render_pipeline_descriptor const* descriptor,
                                sgpu_render_pipeline_id* pipeline);

void sgpu_destroy_render_pipeline(sgpu_render_pipeline_id pipeline);

int sgpu_create_command_buffer(sgpu_command_queue_id queue, sgpu_command_buffer_id* command_buffer);

void sgpu_destroy_command_buffer(sgpu_command_buffer_id command_buffer);
