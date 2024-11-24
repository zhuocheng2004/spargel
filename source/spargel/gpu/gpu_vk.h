/* generated from source/spargel/gpu/registry.json */
#pragma once
#include <spargel/gpu/gpu.h>
typedef struct sgpu_vk_instance* sgpu_vk_instance_id;
typedef struct sgpu_vk_device* sgpu_vk_device_id;
int sgpu_vk_create_instance(sgpu_vk_instance_id* instance, struct sgpu_instance_descriptor const* descriptor);
void sgpu_vk_destroy_instance(sgpu_vk_instance_id instance);
int sgpu_vk_create_default_device(sgpu_vk_instance_id instance, sgpu_vk_device_id* device);
void sgpu_vk_destroy_device(sgpu_vk_device_id device);
