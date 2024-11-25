#pragma once

typedef struct sgpu_instance* sgpu_instance_id;
typedef struct sgpu_device* sgpu_device_id;

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
