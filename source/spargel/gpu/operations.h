#pragma once

#include <spargel/gpu/gpu.h>

struct sgpu_operations {
    int (*create_default_device)(sgpu_device_id*);
    void (*destroy_device)(sgpu_device_id);
    int (*create_command_queue)(sgpu_device_id, sgpu_command_queue_id*);
    void (*destroy_command_queue)(sgpu_command_queue_id);
    int (*create_shader_function)(sgpu_device_id, struct sgpu_shader_function_descriptor const*,
                                  sgpu_shader_function_id*);
    void (*destroy_shader_function)(sgpu_shader_function_id);
    int (*create_metal_shader_library)(sgpu_device_id,
                                       struct sgpu_metal_shader_library_descriptor const*,
                                       sgpu_metal_shader_library_id*);
    void (*destroy_metal_shader_library)(sgpu_metal_shader_library_id);

    int (*create_render_pipeline)(sgpu_device_id, struct sgpu_render_pipeline_descriptor const*,
                                  sgpu_render_pipeline_id*);
    void (*destroy_render_pipeline)(sgpu_render_pipeline_id);
    int (*create_command_buffer)(sgpu_command_queue_id, sgpu_command_buffer_id*);
    void (*destroy_command_buffer)(sgpu_command_buffer_id);
};
