/* generated from source/spargel/gpu/registry.json */
#pragma once
#include <spargel/gpu/gpu.h>
typedef struct sgpu_mtl_instance* sgpu_mtl_instance_id;
typedef struct sgpu_mtl_device* sgpu_mtl_device_id;
int sgpu_mtl_create_instance(sgpu_mtl_instance_id* instance, struct sgpu_instance_descriptor const* descriptor);
void sgpu_mtl_destroy_instance(sgpu_mtl_instance_id instance);
int sgpu_mtl_create_default_device(sgpu_mtl_instance_id instance, sgpu_mtl_device_id* device);
void sgpu_mtl_destroy_device(sgpu_mtl_device_id device);
