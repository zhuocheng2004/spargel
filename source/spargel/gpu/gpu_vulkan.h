#pragma once

#include <spargel/gpu/gpu.h>

namespace spargel::gpu {

    int vulkan_create_default_device(struct device_descriptor const* descriptor, device_id* device);
    void vulkan_destroy_device(device_id device);
    int vulkan_create_command_queue(device_id device, command_queue_id* queue);
    void vulkan_destroy_command_queue(device_id device, command_queue_id queue);
    void vulkan_destroy_shader_function(device_id device, shader_function_id func);
    int vulkan_create_render_pipeline(device_id device,
                                      struct render_pipeline_descriptor const* descriptor,
                                      render_pipeline_id* pipeline);
    void vulkan_destroy_render_pipeline(device_id device, render_pipeline_id pipeline);
    int vulkan_create_command_buffer(device_id device,
                                     struct command_buffer_descriptor const* descriptor,
                                     command_buffer_id* command_buffer);
    void vulkan_destroy_command_buffer(device_id device, command_buffer_id command_buffer);
    void vulkan_reset_command_buffer(device_id device, command_buffer_id command_buffer);
    int vulkan_create_surface(device_id device, struct surface_descriptor const* descriptor,
                              surface_id* surface);
    void vulkan_destroy_surface(device_id device, surface_id surface);
    int vulkan_create_swapchain(device_id device, struct swapchain_descriptor const* descriptor,
                                swapchain_id* swapchain);
    void vulkan_destroy_swapchain(device_id device, swapchain_id swapchain);
    int vulkan_acquire_image(device_id device, struct acquire_descriptor const* descriptor,
                             presentable_id* presentable);
    void vulkan_begin_render_pass(device_id device, struct render_pass_descriptor const* descriptor,
                                  render_pass_encoder_id* encoder);
    void vulkan_end_render_pass(device_id device, render_pass_encoder_id encoder);
    void vulkan_present(device_id device, struct present_descriptor const* descriptor);
    void vulkan_presentable_texture(device_id device, presentable_id presentable,
                                    texture_id* texture);

    /* vulkan specific api */

    struct vulkan_shader_function_descriptor {
        u8* code;
        ssize size; /* in bytes */
    };

    int vulkan_create_shader_function(device_id device,
                                      struct vulkan_shader_function_descriptor const* descriptor,
                                      shader_function_id* func);

}  // namespace spargel::gpu
