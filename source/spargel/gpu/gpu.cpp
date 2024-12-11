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

namespace spargel::gpu {

    int create_default_device(struct device_descriptor const* descriptor, device_id* device) {
        switch (descriptor->backend) {
#if SPARGEL_GPU_ENABLE_METAL
        case BACKEND_METAL:
            return metal_create_default_device(descriptor, device);
#endif
#if SPARGEL_GPU_ENABLE_VULKAN
        case BACKEND_VULKAN:
            return vulkan_create_default_device(descriptor, device);
#endif
#if SPARGEL_GPU_ENABLE_DIRECTX
        case BACKEND_DIRECTX:
            spargel_panic_here();
#endif
        default:
            spargel_panic_here();
        }
    }

#if SPARGEL_GPU_ENABLE_METAL
#define MTL_DISPATCH(name, ...)    \
    case BACKEND_METAL:            \
        metal_##name(__VA_ARGS__); \
        break;
#define MTL_DISPATCH_R(name, ...) \
    case BACKEND_METAL:           \
        return metal_##name(__VA_ARGS__);
#else
#define MTL_DISPATCH(name, ...)
#define MTL_DISPATCH_R(name, ...)
#endif

#if SPARGEL_GPU_ENABLE_VULKAN
#define VK_DISPATCH(name, ...)      \
    case BACKEND_VULKAN:            \
        vulkan_##name(__VA_ARGS__); \
        break;
#define VK_DISPATCH_R(name, ...) \
    case BACKEND_VULKAN:         \
        return vulkan_##name(__VA_ARGS__);
#else
#define VK_DISPATCH(name, ...)
#define VK_DISPATCH_R(name, ...)
#endif

#define DISPATCH(name, ...)             \
    switch (*(int*)(device)) {          \
        MTL_DISPATCH(name, __VA_ARGS__) \
        VK_DISPATCH(name, __VA_ARGS__)  \
    default:                            \
        spargel_panic_here();           \
    }

#define DISPATCH_R(name, ...)             \
    switch (*(int*)(device)) {            \
        MTL_DISPATCH_R(name, __VA_ARGS__) \
        VK_DISPATCH_R(name, __VA_ARGS__)  \
    default:                              \
        spargel_panic_here();             \
    }

    void destroy_device(device_id device) { DISPATCH(destroy_device, device); }

    int create_command_queue(device_id device, command_queue_id* queue) {
        DISPATCH_R(create_command_queue, device, queue);
    }

    void destroy_command_queue(device_id device, command_queue_id queue) {
        DISPATCH(destroy_command_queue, device, queue);
    }

    void destroy_shader_function(device_id device, shader_function_id func) {
        DISPATCH(destroy_shader_function, device, func);
    }

    int create_render_pipeline(device_id device,
                               struct render_pipeline_descriptor const* descriptor,
                               render_pipeline_id* pipeline) {
        DISPATCH_R(create_render_pipeline, device, descriptor, pipeline);
    }

    void destroy_render_pipeline(device_id device, render_pipeline_id pipeline) {
        DISPATCH(destroy_render_pipeline, device, pipeline);
    }

    int create_command_buffer(device_id device, struct command_buffer_descriptor const* descriptor,
                              command_buffer_id* command_buffer) {
        DISPATCH_R(create_command_buffer, device, descriptor, command_buffer);
    }

    void destroy_command_buffer(device_id device, command_buffer_id command_buffer) {
        DISPATCH(destroy_command_buffer, device, command_buffer);
    }

    void reset_command_buffer(device_id device, command_buffer_id command_buffer) {
        DISPATCH(reset_command_buffer, device, command_buffer);
    }

    int create_surface(device_id device, struct surface_descriptor const* descriptor,
                       surface_id* surface) {
        DISPATCH_R(create_surface, device, descriptor, surface);
    }

    void destroy_surface(device_id device, surface_id surface) {
        DISPATCH(destroy_surface, device, surface);
    }

    int create_swapchain(device_id device, struct swapchain_descriptor const* descriptor,
                         swapchain_id* swapchain) {
        DISPATCH_R(create_swapchain, device, descriptor, swapchain);
    }

    void destroy_swapchain(device_id device, swapchain_id swapchain) {
        DISPATCH(destroy_swapchain, device, swapchain);
    }

    int acquire_image(device_id device, struct acquire_descriptor const* descriptor,
                      presentable_id* presentable) {
        DISPATCH_R(acquire_image, device, descriptor, presentable);
    }

    void begin_render_pass(device_id device, struct render_pass_descriptor const* descriptor,
                           render_pass_encoder_id* encoder) {
        DISPATCH(begin_render_pass, device, descriptor, encoder);
    }

    void end_render_pass(device_id device, render_pass_encoder_id encoder) {
        DISPATCH(end_render_pass, device, encoder);
    }

    void present(device_id device, struct present_descriptor const* descriptor) {
        DISPATCH(present, device, descriptor);
    }

    void presentable_texture(device_id device, presentable_id presentable, texture_id* texture) {
        DISPATCH(presentable_texture, device, presentable, texture);
    }

}  // namespace spargel::gpu
