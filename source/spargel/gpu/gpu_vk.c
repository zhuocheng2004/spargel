#include <spargel/gpu/gpu_vk.h>
#include <stdlib.h>

struct sgpu_vk_instance {
  enum sgpu_backend backend;
};

sgpu_result sgpu_vk_create_instance(
    sgpu_vk_instance_id* instance,
    struct sgpu_instance_descriptor const* descriptor) {
  struct sgpu_vk_instance* inst = malloc(sizeof(struct sgpu_vk_instance));
  if (!inst) return SGPU_RESULT_ALLOCATION_FAILED;
  inst->backend = SGPU_BACKEND_VULKAN;
  *instance = inst;
  return SGPU_RESULT_SUCCESS;
}
