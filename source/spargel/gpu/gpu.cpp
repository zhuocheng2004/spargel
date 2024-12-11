#include <spargel/base/base.h>
#include <spargel/config.h>
#include <spargel/gpu/gpu.h>

#if SPARGEL_GPU_ENABLE_METAL
#include <spargel/gpu/gpu_metal.h>
#endif

#if SPARGEL_GPU_ENABLE_VULKAN
#include <spargel/gpu/gpu_vulkan.h>
#endif

#if SPARGEL_GPU_ENABLE_DIRECTX
#endif

int sgpu_create_default_device(struct sgpu_device_descriptor const* descriptor,
                               sgpu_device_id* device) {
    switch (descriptor->backend) {
#if SPARGEL_GPU_ENABLE_METAL
    case SGPU_BACKEND_METAL:
        return sgpu_metal_create_default_device(descriptor, device);
#endif
#if SPARGEL_GPU_ENABLE_VULKAN
    case SGPU_BACKEND_VULKAN:
        return sgpu_vulkan_create_default_device(descriptor, device);
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

#define DISPATCH(name, ...)             \
    switch (*(int*)(device)) {          \
        MTL_DISPATCH(name, __VA_ARGS__) \
        VK_DISPATCH(name, __VA_ARGS__)  \
    default:                            \
        sbase_panic_here();             \
    }

#define DISPATCH_R(name, ...)             \
    switch (*(int*)(device)) {            \
        MTL_DISPATCH_R(name, __VA_ARGS__) \
        VK_DISPATCH_R(name, __VA_ARGS__)  \
    default:                              \
        sbase_panic_here();               \
    }

void sgpu_destroy_device(sgpu_device_id device) { DISPATCH(destroy_device, device); }

int sgpu_create_command_queue(sgpu_device_id device, sgpu_command_queue_id* queue) {
    DISPATCH_R(create_command_queue, device, queue);
}

void sgpu_destroy_command_queue(sgpu_device_id device, sgpu_command_queue_id queue) {
    DISPATCH(destroy_command_queue, device, queue);
}

void sgpu_destroy_shader_function(sgpu_device_id device, sgpu_shader_function_id func) {
    DISPATCH(destroy_shader_function, device, func);
}

int sgpu_create_render_pipeline(sgpu_device_id device,
                                struct sgpu_render_pipeline_descriptor const* descriptor,
                                sgpu_render_pipeline_id* pipeline) {
    DISPATCH_R(create_render_pipeline, device, descriptor, pipeline);
}

void sgpu_destroy_render_pipeline(sgpu_device_id device, sgpu_render_pipeline_id pipeline) {
    DISPATCH(destroy_render_pipeline, device, pipeline);
}

int sgpu_create_command_buffer(sgpu_device_id device,
                               struct sgpu_command_buffer_descriptor const* descriptor,
                               sgpu_command_buffer_id* command_buffer) {
    DISPATCH_R(create_command_buffer, device, descriptor, command_buffer);
}

void sgpu_destroy_command_buffer(sgpu_device_id device, sgpu_command_buffer_id command_buffer) {
    DISPATCH(destroy_command_buffer, device, command_buffer);
}

void sgpu_reset_command_buffer(sgpu_device_id device, sgpu_command_buffer_id command_buffer) {
    DISPATCH(reset_command_buffer, device, command_buffer);
}

int sgpu_create_surface(sgpu_device_id device, struct sgpu_surface_descriptor const* descriptor,
                        sgpu_surface_id* surface) {
    DISPATCH_R(create_surface, device, descriptor, surface);
}

void sgpu_destroy_surface(sgpu_device_id device, sgpu_surface_id surface) {
    DISPATCH(destroy_surface, device, surface);
}

int sgpu_create_swapchain(sgpu_device_id device, struct sgpu_swapchain_descriptor const* descriptor,
                          sgpu_swapchain_id* swapchain) {
    DISPATCH_R(create_swapchain, device, descriptor, swapchain);
}

void sgpu_destroy_swapchain(sgpu_device_id device, sgpu_swapchain_id swapchain) {
    DISPATCH(destroy_swapchain, device, swapchain);
}

int sgpu_acquire_image(sgpu_device_id device, struct sgpu_acquire_descriptor const* descriptor,
                       sgpu_presentable_id* presentable) {
    DISPATCH_R(acquire_image, device, descriptor, presentable);
}

void sgpu_begin_render_pass(sgpu_device_id device,
                            struct sgpu_render_pass_descriptor const* descriptor,
                            sgpu_render_pass_encoder_id* encoder) {
    DISPATCH(begin_render_pass, device, descriptor, encoder);
}

void sgpu_end_render_pass(sgpu_device_id device, sgpu_render_pass_encoder_id encoder) {
    DISPATCH(end_render_pass, device, encoder);
}

void sgpu_present(sgpu_device_id device, struct sgpu_present_descriptor const* descriptor) {
    DISPATCH(present, device, descriptor);
}

void sgpu_presentable_texture(sgpu_device_id device, sgpu_presentable_id presentable,
                              sgpu_texture_id* texture) {
    DISPATCH(presentable_texture, device, presentable, texture);
}
