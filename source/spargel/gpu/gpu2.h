#pragma once

#include <spargel/base/base.h>

typedef struct sgpu2_device* sgpu2_device_id;
typedef struct sgpu2_surface* sgpu2_surface_id;

typedef struct sgpu2_shader* sgpu2_shader_id;

typedef struct sgpu2_buffer* sgpu2_buffer_id;
typedef struct sgpu2_texture* sgpu2_texture_id;

typedef struct sgpu2_render_task* sgpu2_render_task_id;
typedef struct sgpu2_compute_task* sgpu2_compute_task_id;

enum sgpu2_backend {
    SGPU2_BACKEND_DIRECTX,
    SGPU2_BACKEND_METAL,
    SGPU2_BACKEND_VULKAN,
};

sgpu2_device_id sgpu2_create_device(int backend);
void sgpu2_destroy_device(sgpu2_device_id device);

struct sui_window;
sgpu2_surface_id sgpu2_create_surface(sgpu2_device_id device, struct sui_window* window);
void sgpu2_destroy_surface(sgpu2_device_id device, sgpu2_surface_id surface);

/**
 * Reset the task graph frontend for the next frame.
 */
void sgpu2_prepare(sgpu2_device_id device);
/**
 * Start to execute the task graph.
 */
void sgpu2_execute(sgpu2_device_id device);

/**
 * Create a persistent buffer with given data.
 */
sgpu2_buffer_id sgpu2_create_buffer_with_data(sgpu2_device_id device, ssize size, void* data);
void sgpu2_destroy_buffer(sgpu2_device_id id, sgpu2_buffer_id buffer);

struct sgpu2_texture_descriptor {
    int width;
    int height;
    int format;
    bool render_target;
};

/**
 * Declare a transient texture.
 */
sgpu2_texture_id sgpu2_declare_texture(sgpu2_device_id device,
                                       struct sgpu2_texture_descriptor const* descriptor);

/**
 * Declare a read/write/readwrite dependency on a texture.
 */
void sgpu2_declare_texture_read(sgpu2_device_id device, sgpu2_render_task_id task,
                                sgpu2_texture_id texture);
void sgpu2_declare_texture_write(sgpu2_device_id device, sgpu2_render_task_id task,
                                 sgpu2_texture_id texture);
void sgpu2_declare_texture_readwrite(sgpu2_device_id device, sgpu2_render_task_id task,
                                     sgpu2_texture_id texture);

/**
 * Declare a new render task in the task graph.
 */
sgpu2_render_task_id sgpu2_declare_render_task(sgpu2_device_id device);

void sgpu2_declare_present_task(sgpu2_device_id device, sgpu2_surface_id surface);

void sgpu2_set_vertex_shader(sgpu2_device_id device, sgpu2_render_task_id task, sgpu2_shader_id shader);
void sgpu2_set_fragment(sgpu2_device_id device, sgpu2_render_task_id task, sgpu2_shader_id shader);
void sgpu2_set_depth_enabled(sgpu2_device_id device, sgpu2_render_task_id task, bool enabled);
void sgpu2_set_depth_compare(sgpu2_device_id device, sgpu2_render_task_id task, int compare);
void sgpu2_set_cull_mode(sgpu2_device_id device, sgpu2_render_task_id task, int cull);
void sgpu2_set_vertex_buffer(sgpu2_device_id device, sgpu2_render_task_id task, int index,
                             sgpu2_buffer_id buffer);
void sgpu2_draw_indexed(sgpu2_device_id device, sgpu2_render_task_id task, int index_count,
                        int instance_count);

enum sgpu2_depth_compare {
    SGPU2_DEPTH_LESS_EQUAL,
};

enum sgpu2_cull_mode {
    SGPU2_CULL_BACK,
};
