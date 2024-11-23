/* generated from source/spargel/gpu/registry.json */
#pragma once
typedef struct sgpu_instance* sgpu_instance_id;
typedef struct sgpu_device* sgpu_device_id;
typedef enum sgpu_result {
  SGPU_RESULT_SUCCESS,
  SGPU_RESULT_NO_BACKEND,
  SGPU_RESULT_ALLOCATION_FAILED,
} sgpu_result;
typedef enum sgpu_backend {
  SGPU_BACKEND_DIRECTX,
  SGPU_BACKEND_METAL,
  SGPU_BACKEND_VULKAN,
} sgpu_backend;
typedef struct sgpu_instance_descriptor {
  sgpu_backend backend;
} sgpu_instance_descriptor;
sgpu_result sgpu_create_instance(sgpu_instance_id* instance, sgpu_instance_descriptor const* descriptor);
void sgpu_destroy_instance(sgpu_instance_id instance);
sgpu_result sgpu_create_default_device(sgpu_instance_id instance, sgpu_device_id* device);
void sgpu_destroy_device(sgpu_device_id device);
