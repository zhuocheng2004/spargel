#pragma once

#include <spargel/base/base.h>
#include <spargel/ui/ui.h>

namespace spargel::gpu {

    typedef struct device* device_id;
    typedef u64 command_queue_id;
    typedef u64 command_buffer_id;
    typedef u64 render_pass_encoder_id;
    typedef u64 render_pipeline_id;
    typedef u64 shader_function_id;
    typedef u64 surface_id;
    typedef u64 swapchain_id;
    typedef u64 texture_id;
    typedef u64 presentable_id;

    typedef u64 bind_group_layout;
    typedef u64 bind_group;
    typedef u64 pipeline_layout;

    enum result {
        RESULT_SUCCESS,
        RESULT_NO_BACKEND,
        RESULT_ALLOCATION_FAILED,
        RESULT_CANNOT_CREATE_METAL_LIBRARY,
        RESULT_CANNOT_CREATE_SHADER_FUNCTION,
        RESULT_CANNOT_CREATE_RENDER_PIPELINE,
    };

    enum backend {
        BACKEND_DIRECTX,
        BACKEND_METAL,
        BACKEND_VULKAN,
    };

    enum primitive {
        PRIMITIVE_TRIANGLE,
    };

    enum format {
        FORMAT_BRGA8_UNORM,
        FORMAT_BGRA8_SRGB,
    };

    struct device_descriptor {
        int backend;
        ui::platform_kind platform;
    };

    struct render_pipeline_descriptor {
        // int primitive;
        int target_format;
        shader_function_id vertex_function;
        shader_function_id fragment_function;
    };

    struct command_buffer_descriptor {
        command_queue_id queue;
    };

    struct surface_descriptor {
        spargel::ui::window* window;
    };

    struct swapchain_descriptor {
        surface_id surface;
        int width;
        int height;
    };

    struct acquire_descriptor {
        swapchain_id swapchain;
    };

    struct color4 {
        double r;
        double g;
        double b;
        double a;
    };

    struct render_pass_descriptor {
        command_buffer_id command_buffer;
        texture_id color_attachment;
        struct color4 clear_color;
        /* todo: remove this immediately! */
        swapchain_id swapchain;
    };

    struct present_descriptor {
        command_buffer_id command_buffer;
        presentable_id presentable;
    };

    /**
     * @brief create default device
     */
    int create_default_device(struct device_descriptor const* descriptor, device_id* device);

    /**
     * @brief destroy device
     */
    void destroy_device(device_id device);

    /**
     * Questions:
     *  1. Metal has `MTLIOCommandQueue`.
     */
    int create_command_queue(device_id device, command_queue_id* queue);

    void destroy_command_queue(device_id device, command_queue_id queue);

    int create_command_buffer(device_id device, struct command_buffer_descriptor const* descriptor,
                              command_buffer_id* command_buffer);

    void destroy_command_buffer(device_id device, command_buffer_id command_buffer);

    void reset_command_buffer(device_id device, command_buffer_id command_buffer);

    void destroy_shader_function(device_id device, shader_function_id func);

    int create_render_pipeline(device_id device,
                               struct render_pipeline_descriptor const* descriptor,
                               render_pipeline_id* pipeline);

    void destroy_render_pipeline(device_id device, render_pipeline_id pipeline);

    int create_surface(device_id device, struct surface_descriptor const* descriptor,
                       surface_id* surface);

    void destroy_surface(device_id device, surface_id surface);

    int create_swapchain(device_id device, struct swapchain_descriptor const* descriptor,
                         swapchain_id* swapchain);

    void destroy_swapchain(device_id device, swapchain_id swapchain);

    int acquire_image(device_id device, struct acquire_descriptor const* descriptor,
                      presentable_id* presentable);

    void begin_render_pass(device_id device, struct render_pass_descriptor const* descriptor,
                           render_pass_encoder_id* encoder);

    void end_render_pass(device_id device, render_pass_encoder_id encoder);

    void present(device_id device, struct present_descriptor const* descriptor);

    void presentable_texture(device_id device, presentable_id presentable, texture_id* texture);

}  // namespace spargel::gpu
