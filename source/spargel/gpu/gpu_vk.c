#include <spargel/base/base.h>
#include <spargel/gpu/gpu_vk.h>
#include <stdlib.h>

struct sgpu_vk_instance {
  int backend;
};

int sgpu_vk_create_instance(sgpu_vk_instance_id* instance,
                            struct sgpu_instance_descriptor const* descriptor) {
  struct sgpu_vk_instance* inst = malloc(sizeof(struct sgpu_vk_instance));
  if (!inst) return SGPU_RESULT_ALLOCATION_FAILED;
  inst->backend = SGPU_BACKEND_VULKAN;
  *instance = inst;
  return SGPU_RESULT_SUCCESS;
}

void sgpu_vk_destroy_instance(sgpu_vk_instance_id instance) { free(instance); }

int sgpu_vk_create_default_device(sgpu_vk_instance_id instance,
                                  sgpu_vk_device_id* device) {
  sbase_unreachable();
}

void sgpu_vk_destroy_device(sgpu_vk_device_id device) { sbase_unreachable(); }
