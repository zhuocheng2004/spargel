#pragma once

#include <spargel/gpu/gpu.h>

int sgpu_metal_create_default_device(struct sgpu_device_descriptor const* descriptor,
                                     sgpu_device_id* device);
void sgpu_metal_destroy_device(sgpu_device_id device);
int sgpu_metal_create_command_queue(sgpu_device_id device, sgpu_command_queue_id* queue);
void sgpu_metal_destroy_command_queue(sgpu_device_id device, sgpu_command_queue_id queue);
void sgpu_metal_destroy_shader_function(sgpu_device_id device, sgpu_shader_function_id func);
int sgpu_metal_create_render_pipeline(sgpu_device_id device,
                                      struct sgpu_render_pipeline_descriptor const* descriptor,
                                      sgpu_render_pipeline_id* pipeline);
void sgpu_metal_destroy_render_pipeline(sgpu_device_id device, sgpu_render_pipeline_id pipeline);
int sgpu_metal_create_command_buffer(sgpu_device_id device,
                                     struct sgpu_command_buffer_descriptor const* descriptor,
                                     sgpu_command_buffer_id* command_buffer);
void sgpu_metal_destroy_command_buffer(sgpu_device_id device,
                                       sgpu_command_buffer_id command_buffer);
int sgpu_metal_create_surface(sgpu_device_id device,
                              struct sgpu_surface_descriptor const* descriptor,
                              sgpu_surface_id* surface);
void sgpu_metal_destroy_surface(sgpu_device_id device, sgpu_surface_id surface);
int sgpu_metal_create_swapchain(sgpu_device_id device,
                                struct sgpu_swapchain_descriptor const* descriptor,
                                sgpu_swapchain_id* swapchain);
void sgpu_metal_destroy_swapchain(sgpu_device_id device, sgpu_swapchain_id swapchain);
int sgpu_metal_acquire_image(sgpu_device_id device,
                             struct sgpu_acquire_descriptor const* descriptor,
                             sgpu_presentable_id* presentable);
void sgpu_metal_begin_render_pass(sgpu_device_id device,
                                  struct sgpu_render_pass_descriptor const* descriptor,
                                  sgpu_render_pass_encoder_id* encoder);
void sgpu_metal_end_render_pass(sgpu_device_id device, sgpu_render_pass_encoder_id encoder);
void sgpu_metal_present(sgpu_device_id device, struct sgpu_present_descriptor const* descriptor);
void sgpu_metal_presentable_texture(sgpu_device_id device, sgpu_presentable_id presentable,
                                    sgpu_texture_id* texture);

/* metal specific api */

typedef struct sgpu_metal_shader_library* sgpu_metal_shader_library_id;

/* todo: refactor using sbase_url and asset loading */
struct sgpu_metal_shader_library_descriptor {
    void* code;
    ssize size;
};

struct sgpu_metal_shader_function_descriptor {
    sgpu_metal_shader_library_id library;
    char const* name;
};

int sgpu_metal_create_shader_function(
    sgpu_device_id device, struct sgpu_metal_shader_function_descriptor const* descriptor,
    sgpu_shader_function_id* func);

int sgpu_metal_create_shader_library(sgpu_device_id device,
                                     struct sgpu_metal_shader_library_descriptor const* descriptor,
                                     sgpu_metal_shader_library_id* library);

void sgpu_metal_destroy_shader_library(sgpu_device_id device, sgpu_metal_shader_library_id library);
