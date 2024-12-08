#pragma once

#include <spargel/base/base.h>

typedef struct sgpu_device* sgpu_device_id;
typedef u64 sgpu_command_queue_id;
typedef u64 sgpu_command_buffer_id;
typedef u64 sgpu_render_pass_encoder_id;
typedef u64 sgpu_render_pipeline_id;
typedef u64 sgpu_shader_function_id;
typedef u64 sgpu_surface_id;
typedef u64 sgpu_swapchain_id;
typedef u64 sgpu_texture_id;
typedef u64 sgpu_presentable_id;

typedef u64 sgpu_bind_group_layout;
typedef u64 sgpu_bind_group;
typedef u64 sgpu_pipeline_layout;

/* external */
struct sui_window;

enum sgpu_result {
    SGPU_RESULT_SUCCESS,
    SGPU_RESULT_NO_BACKEND,
    SGPU_RESULT_ALLOCATION_FAILED,
    SGPU_RESULT_CANNOT_CREATE_METAL_LIBRARY,
    SGPU_RESULT_CANNOT_CREATE_SHADER_FUNCTION,
    SGPU_RESULT_CANNOT_CREATE_RENDER_PIPELINE,
};

enum sgpu_backend {
    SGPU_BACKEND_DIRECTX,
    SGPU_BACKEND_METAL,
    SGPU_BACKEND_VULKAN,
};

enum sgpu_primitive {
    SGPU_PRIMITIVE_TRIANGLE,
};

enum sgpu_format {
    SGPU_FORMAT_BRGA8_UNORM,
    SGPU_FORMAT_BGRA8_SRGB,
};

struct sgpu_device_descriptor {
    int backend;
    int platform;
};

struct sgpu_render_pipeline_descriptor {
    // int primitive;
    int target_format;
    sgpu_shader_function_id vertex_function;
    sgpu_shader_function_id fragment_function;
};

struct sgpu_command_buffer_descriptor {
    sgpu_command_queue_id queue;
};

struct sgpu_surface_descriptor {
    struct sui_window* window;
};

struct sgpu_swapchain_descriptor {
    sgpu_surface_id surface;
    int width;
    int height;
};

struct sgpu_acquire_descriptor {
    sgpu_swapchain_id swapchain;
};

struct sgpu_color4 {
    double r;
    double g;
    double b;
    double a;
};

struct sgpu_render_pass_descriptor {
    sgpu_command_buffer_id command_buffer;
    sgpu_texture_id color_attachment;
    struct sgpu_color4 clear_color;
    /* todo: remove this immediately! */
    sgpu_swapchain_id swapchain;
};

struct sgpu_present_descriptor {
    sgpu_command_buffer_id command_buffer;
    sgpu_presentable_id presentable;
};

/**
 * @brief create default device
 */
int sgpu_create_default_device(struct sgpu_device_descriptor const* descriptor,
                               sgpu_device_id* device);

/**
 * @brief destroy device
 */
void sgpu_destroy_device(sgpu_device_id device);

/**
 * Questions:
 *  1. Metal has `MTLIOCommandQueue`.
 */
int sgpu_create_command_queue(sgpu_device_id device, sgpu_command_queue_id* queue);

void sgpu_destroy_command_queue(sgpu_device_id device, sgpu_command_queue_id queue);

int sgpu_create_command_buffer(sgpu_device_id device,
                               struct sgpu_command_buffer_descriptor const* descriptor,
                               sgpu_command_buffer_id* command_buffer);

void sgpu_destroy_command_buffer(sgpu_device_id device, sgpu_command_buffer_id command_buffer);

void sgpu_reset_command_buffer(sgpu_device_id device, sgpu_command_buffer_id command_buffer);

void sgpu_destroy_shader_function(sgpu_device_id device, sgpu_shader_function_id func);

int sgpu_create_render_pipeline(sgpu_device_id device,
                                struct sgpu_render_pipeline_descriptor const* descriptor,
                                sgpu_render_pipeline_id* pipeline);

void sgpu_destroy_render_pipeline(sgpu_device_id device, sgpu_render_pipeline_id pipeline);

int sgpu_create_surface(sgpu_device_id device, struct sgpu_surface_descriptor const* descriptor,
                        sgpu_surface_id* surface);

void sgpu_destroy_surface(sgpu_device_id device, sgpu_surface_id surface);

int sgpu_create_swapchain(sgpu_device_id device, struct sgpu_swapchain_descriptor const* descriptor,
                          sgpu_swapchain_id* swapchain);

void sgpu_destroy_swapchain(sgpu_device_id device, sgpu_swapchain_id swapchain);

int sgpu_acquire_image(sgpu_device_id device, struct sgpu_acquire_descriptor const* descriptor,
                       sgpu_presentable_id* presentable);

void sgpu_begin_render_pass(sgpu_device_id device,
                            struct sgpu_render_pass_descriptor const* descriptor,
                            sgpu_render_pass_encoder_id* encoder);

void sgpu_end_render_pass(sgpu_device_id device, sgpu_render_pass_encoder_id encoder);

void sgpu_present(sgpu_device_id device, struct sgpu_present_descriptor const* descriptor);

void sgpu_presentable_texture(sgpu_device_id device, sgpu_presentable_id presentable,
                              sgpu_texture_id* texture);
