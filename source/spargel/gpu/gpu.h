#pragma once

#include <spargel/base/base.h>

typedef struct sgpu_device* sgpu_device_id;
typedef struct sgpu_command_queue* sgpu_command_queue_id;
typedef struct sgpu_render_pipeline* sgpu_render_pipeline_id;
typedef struct sgpu_command_buffer* sgpu_command_buffer_id;
typedef struct sgpu_shader_function* sgpu_shader_function_id;

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

void sgpu_destroy_command_queue(sgpu_command_queue_id queue);

void sgpu_destroy_shader_function(sgpu_shader_function_id func);

int sgpu_create_render_pipeline(sgpu_device_id device,
                                struct sgpu_render_pipeline_descriptor const* descriptor,
                                sgpu_render_pipeline_id* pipeline);

void sgpu_destroy_render_pipeline(sgpu_render_pipeline_id pipeline);

int sgpu_create_command_buffer(sgpu_command_queue_id queue, sgpu_command_buffer_id* command_buffer);

void sgpu_destroy_command_buffer(sgpu_command_buffer_id command_buffer);
