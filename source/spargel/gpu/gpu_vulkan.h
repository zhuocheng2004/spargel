#pragma once

#include <spargel/gpu/gpu.h>

int sgpu_vulkan_create_default_device(struct sgpu_device_descriptor const* descriptor,
                                      sgpu_device_id* device);
void sgpu_vulkan_destroy_device(sgpu_device_id device);
int sgpu_vulkan_create_command_queue(sgpu_device_id device, sgpu_command_queue_id* queue);
void sgpu_vulkan_destroy_command_queue(sgpu_device_id device, sgpu_command_queue_id queue);
void sgpu_vulkan_destroy_shader_function(sgpu_device_id device, sgpu_shader_function_id func);
int sgpu_vulkan_create_render_pipeline(sgpu_device_id device,
                                       struct sgpu_render_pipeline_descriptor const* descriptor,
                                       sgpu_render_pipeline_id* pipeline);
void sgpu_vulkan_destroy_render_pipeline(sgpu_device_id device, sgpu_render_pipeline_id pipeline);
int sgpu_vulkan_create_command_buffer(sgpu_device_id device,
                                      struct sgpu_command_buffer_descriptor const* descriptor,
                                      sgpu_command_buffer_id* command_buffer);
void sgpu_vulkan_destroy_command_buffer(sgpu_device_id device,
                                        sgpu_command_buffer_id command_buffer);
void sgpu_vulkan_reset_command_buffer(sgpu_device_id device, sgpu_command_buffer_id command_buffer);
int sgpu_vulkan_create_surface(sgpu_device_id device,
                               struct sgpu_surface_descriptor const* descriptor,
                               sgpu_surface_id* surface);
void sgpu_vulkan_destroy_surface(sgpu_device_id device, sgpu_surface_id surface);
int sgpu_vulkan_create_swapchain(sgpu_device_id device,
                                 struct sgpu_swapchain_descriptor const* descriptor,
                                 sgpu_swapchain_id* swapchain);
void sgpu_vulkan_destroy_swapchain(sgpu_device_id device, sgpu_swapchain_id swapchain);
int sgpu_vulkan_acquire_image(sgpu_device_id device,
                              struct sgpu_acquire_descriptor const* descriptor,
                              sgpu_presentable_id* presentable);
void sgpu_vulkan_begin_render_pass(sgpu_device_id device,
                                   struct sgpu_render_pass_descriptor const* descriptor,
                                   sgpu_render_pass_encoder_id* encoder);
void sgpu_vulkan_end_render_pass(sgpu_device_id device, sgpu_render_pass_encoder_id encoder);
void sgpu_vulkan_present(sgpu_device_id device, struct sgpu_present_descriptor const* descriptor);
void sgpu_vulkan_presentable_texture(sgpu_device_id device, sgpu_presentable_id presentable,
                                     sgpu_texture_id* texture);

/* vulkan specific api */

struct sgpu_vulkan_shader_function_descriptor {
    u8* code;
    ssize size; /* in bytes */
};

int sgpu_vulkan_create_shader_function(
    sgpu_device_id device, struct sgpu_vulkan_shader_function_descriptor const* descriptor,
    sgpu_shader_function_id* func);
