/* generated from source/spargel/gpu/registry.json */
#pragma once
#include <spargel/gpu/gpu.h>
typedef struct sgpu_dx_instance* sgpu_dx_instance_id;
typedef struct sgpu_dx_device* sgpu_dx_device_id;
int sgpu_dx_create_instance(sgpu_dx_instance_id* instance, struct sgpu_instance_descriptor const* descriptor);
void sgpu_dx_destroy_instance(sgpu_dx_instance_id instance);
int sgpu_dx_create_default_device(sgpu_dx_instance_id instance, sgpu_dx_device_id* device);
void sgpu_dx_destroy_device(sgpu_dx_device_id device);
