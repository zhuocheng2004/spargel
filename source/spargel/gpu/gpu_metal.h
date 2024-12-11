#pragma once

#include <spargel/gpu/gpu.h>

namespace spargel::gpu {

    int metal_create_default_device(struct device_descriptor const* descriptor, device_id* device);
    void metal_destroy_device(device_id device);
    int metal_create_command_queue(device_id device, command_queue_id* queue);
    void metal_destroy_command_queue(device_id device, command_queue_id queue);
    void metal_destroy_shader_function(device_id device, shader_function_id func);
    int metal_create_render_pipeline(device_id device,
                                     struct render_pipeline_descriptor const* descriptor,
                                     render_pipeline_id* pipeline);
    void metal_destroy_render_pipeline(device_id device, render_pipeline_id pipeline);
    int metal_create_command_buffer(device_id device,
                                    struct command_buffer_descriptor const* descriptor,
                                    command_buffer_id* command_buffer);
    void metal_destroy_command_buffer(device_id device, command_buffer_id command_buffer);
    void metal_reset_command_buffer(device_id device, command_buffer_id command_buffer);
    int metal_create_surface(device_id device, struct surface_descriptor const* descriptor,
                             surface_id* surface);
    void metal_destroy_surface(device_id device, surface_id surface);
    int metal_create_swapchain(device_id device, struct swapchain_descriptor const* descriptor,
                               swapchain_id* swapchain);
    void metal_destroy_swapchain(device_id device, swapchain_id swapchain);
    int metal_acquire_image(device_id device, struct acquire_descriptor const* descriptor,
                            presentable_id* presentable);
    void metal_begin_render_pass(device_id device, struct render_pass_descriptor const* descriptor,
                                 render_pass_encoder_id* encoder);
    void metal_end_render_pass(device_id device, render_pass_encoder_id encoder);
    void metal_present(device_id device, struct present_descriptor const* descriptor);
    void metal_presentable_texture(device_id device, presentable_id presentable,
                                   texture_id* texture);

    /* metal specific api */

    typedef struct metal_shader_library* metal_shader_library_id;

    /* todo: refactor using sbase_url and asset loading */
    struct metal_shader_library_descriptor {
        void* code;
        ssize size;
    };

    struct metal_shader_function_descriptor {
        metal_shader_library_id library;
        char const* name;
    };

    int metal_create_shader_function(device_id device,
                                     struct metal_shader_function_descriptor const* descriptor,
                                     shader_function_id* func);

    int metal_create_shader_library(device_id device,
                                    struct metal_shader_library_descriptor const* descriptor,
                                    metal_shader_library_id* library);

    void metal_destroy_shader_library(device_id device, metal_shader_library_id library);

}  // namespace spargel::gpu
