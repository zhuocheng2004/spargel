#pragma once

#include <spargel/base/base.h>

typedef struct sgpu_device* sgpu_device_id;
typedef struct sgpu_command_queue* sgpu_command_queue_id;
typedef struct sgpu_render_pipeline* sgpu_render_pipeline_id;
typedef struct sgpu_command_buffer* sgpu_command_buffer_id;
typedef struct sgpu_shader_function* sgpu_shader_function_id;

typedef struct sgpu_metal_shader_library* sgpu_metal_shader_library_id;

enum sgpu_result {
    SGPU_RESULT_SUCCESS,
    SGPU_RESULT_NO_BACKEND,
    SGPU_RESULT_ALLOCATION_FAILED,
    SGPU_RESULT_CANNOT_CREATE_METAL_LIBRARY,
    SGPU_RESULT_CANNOT_CREATE_SHADER_FUNCTION,
};

enum sgpu_backend {
    SGPU_BACKEND_DIRECTX,
    SGPU_BACKEND_METAL,
    SGPU_BACKEND_VULKAN,
};

/* todo: refactor using sbase_url and asset loading */
struct sgpu_metal_shader_library_descriptor {
    void* code;
    ssize size;
};

/**
 * Metal
 *  - one metal library contains multiple entry points
 *
 * Vulkan
 *  - one SPIR-V module for one shader
 */
struct sgpu_shader_function_descriptor {
    struct {
        sgpu_metal_shader_library_id library;
        char const* name;
    } metal;
    struct {
        void* code;
        ssize size;
        char const* name;
    } vulkan;
};

struct sgpu_render_pipeline_descriptor {};

/**
 * @brief create default device
 */
int sgpu_create_default_device(int backend, sgpu_device_id* device);

/**
 * @brief destroy device
 */
void sgpu_destroy_device(sgpu_device_id device);

int sgpu_create_command_queue(sgpu_device_id device,
                              sgpu_command_queue_id* queue);

void sgpu_destroy_command_queue(sgpu_command_queue_id queue);

int sgpu_create_shader_function(
    sgpu_device_id device,
    struct sgpu_shader_function_descriptor const* descriptor,
    sgpu_shader_function_id* func);

void sgpu_destroy_shader_function(sgpu_shader_function_id func);

int sgpu_create_metal_shader_library(
    sgpu_device_id device,
    struct sgpu_metal_shader_library_descriptor const* descriptor,
    sgpu_metal_shader_library_id* library);

void sgpu_destroy_metal_shader_library(sgpu_metal_shader_library_id library);

int sgpu_create_render_pipeline(
    sgpu_device_id device,
    struct sgpu_render_pipeline_descriptor const* descriptor,
    sgpu_render_pipeline_id* pipeline);

void sgpu_destroy_render_pipeline(sgpu_render_pipeline_id pipeline);

int sgpu_create_command_buffer(sgpu_command_queue_id queue,
                               sgpu_command_buffer_id* command_buffer);

void sgpu_destroy_command_buffer(sgpu_command_buffer_id command_buffer);
