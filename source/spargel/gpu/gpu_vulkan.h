#pragma once

#include <spargel/gpu/gpu.h>

#if SPARGEL_IS_LINUX
#define VULKAN_LIB_FILENAME "libvulkan.so.1"
#elif SPARGEL_IS_MACOS
#define VULKAN_LIB_FILENAME "libvulkan.dylib"
#elif SPARGEL_IS_WINDOWS
#define VULKAN_LIB_FILENAME "vulkan-1.dll"
#endif

int sgpu_vulkan_create_default_device(sgpu_device_id* device);
void sgpu_vulkan_destroy_device(sgpu_device_id device);
int sgpu_vulkan_create_command_queue(sgpu_device_id device, sgpu_command_queue_id* queue);
void sgpu_vulkan_destroy_command_queue(sgpu_command_queue_id queue);
void sgpu_vulkan_destroy_shader_function(sgpu_shader_function_id func);
int sgpu_vulkan_create_render_pipeline(sgpu_device_id device,
                                       struct sgpu_render_pipeline_descriptor const* descriptor,
                                       sgpu_render_pipeline_id* pipeline);
void sgpu_vulkan_destroy_render_pipeline(sgpu_render_pipeline_id pipeline);
int sgpu_vulkan_create_command_buffer(sgpu_command_queue_id queue,
                                      sgpu_command_buffer_id* command_buffer);
void sgpu_vulkan_destroy_command_buffer(sgpu_command_buffer_id command_buffer);

/* vulkan specific api */

struct sgpu_vulkan_shader_function_descriptor {
    u8* code;
    ssize size;
};

int sgpu_vulkan_create_shader_function(
    sgpu_device_id device, struct sgpu_vulkan_shader_function_descriptor const* descriptor,
    sgpu_shader_function_id* func);
