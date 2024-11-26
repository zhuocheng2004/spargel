#pragma once

typedef struct sgpu_instance* sgpu_instance_id;
typedef struct sgpu_device* sgpu_device_id;
typedef struct sgpu_command_queue* sgpu_command_queue_id;
typedef struct sgpu_render_pipeline* sgpu_render_pipeline_id;
typedef struct sgpu_command_buffer* sgpu_command_buffer_id;
typedef struct sgpu_shader_function* sgpu_shader_function_id;

enum sgpu_result {
  SGPU_RESULT_SUCCESS,
  SGPU_RESULT_NO_BACKEND,
  SGPU_RESULT_ALLOCATION_FAILED,
};

enum sgpu_backend {
  SGPU_BACKEND_DIRECTX,
  SGPU_BACKEND_METAL,
  SGPU_BACKEND_VULKAN,
};

struct sgpu_instance_descriptor {
  int backend;
};

struct sgpu_shader_function_descriptor {};

struct sgpu_render_pipeline_descriptor {};

/**
 * @brief create instance
 */
int sgpu_create_instance(struct sgpu_instance_descriptor const* descriptor,
                         sgpu_instance_id* instance);

/**
 * @brief destroy instance
 */
void sgpu_destroy_instance(sgpu_instance_id instance);

/**
 * @brief create default device
 */
int sgpu_create_default_device(sgpu_instance_id instance,
                               sgpu_device_id* device);

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

int sgpu_create_render_pipeline(
    sgpu_device_id device,
    struct sgpu_render_pipeline_descriptor const* descriptor,
    sgpu_render_pipeline_id* pipeline);

void sgpu_destroy_render_pipeline(sgpu_render_pipeline_id pipeline);

int sgpu_create_command_buffer(sgpu_command_queue_id queue,
                               sgpu_command_buffer_id* command_buffer);

void sgpu_destroy_command_buffer(sgpu_command_buffer_id command_buffer);
