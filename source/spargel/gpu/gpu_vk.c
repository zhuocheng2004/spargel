#include <spargel/base/base.h>
#include <spargel/gpu/gpu.h>
#include <spargel/gpu/operations.h>
#include <stdlib.h>

const struct sgpu_operations sgpu_vk_operations;

struct instance {
  struct sgpu_operations const* _ops;
};

static int create_instance(struct sgpu_instance_descriptor const* descriptor,
                           sgpu_instance_id* instance) {
  struct instance* inst = malloc(sizeof(struct instance));
  if (!inst) return SGPU_RESULT_ALLOCATION_FAILED;
  inst->_ops = &sgpu_vk_operations;
  *instance = (sgpu_instance_id)inst;
  return SGPU_RESULT_SUCCESS;
}

static void destroy_instance(sgpu_instance_id i) {
  struct instance* instance = (struct instance*)i;
  free(instance);
}

static int create_default_device(sgpu_instance_id instance,
                                 sgpu_device_id* device) {
  sbase_unreachable();
}

static void destroy_device(sgpu_device_id device) { sbase_unreachable(); }

const struct sgpu_operations sgpu_vk_operations = {
    .create_instance = create_instance,
    .destroy_instance = destroy_instance,
    .create_default_device = create_default_device,
    .destroy_device = destroy_device,
};
