#import <Metal/Metal.h>
#include <spargel/gpu/gpu.h>
#include <spargel/gpu/operations.h>
#include <stdlib.h>

const struct sgpu_operations sgpu_mtl_operations;

/**
 * We only target Apple Silicon.
 * This in particular guarantees that there is only one GPU.
 */

struct instance {
  const struct sgpu_operations* _ops;
};

struct device {
  const struct sgpu_operations* _ops;
  id<MTLDevice> device;
};

int create_instance(struct sgpu_instance_descriptor const* descriptor, sgpu_instance_id* instance) {
  struct instance* inst = malloc(sizeof(struct instance));
  if (!inst) return SGPU_RESULT_ALLOCATION_FAILED;
  inst->_ops = &sgpu_mtl_operations;
  *instance = (struct sgpu_instance*)inst;
  return SGPU_RESULT_SUCCESS;
}

static void destroy_instance(sgpu_instance_id i) {
  struct instance* instance = (struct instance*)i;
  free(instance);
}

static int create_default_device(sgpu_instance_id instance, sgpu_device_id* device) {
  struct device* d = malloc(sizeof(struct device));
  if (!d) return SGPU_RESULT_ALLOCATION_FAILED;
  d->_ops = &sgpu_mtl_operations;
  d->device = MTLCreateSystemDefaultDevice();
  *device = (struct sgpu_device*)d;
  return SGPU_RESULT_SUCCESS;
}

static void destroy_device(sgpu_device_id d) {
  struct device* device = (struct device*)d;
  [device->device release];
  free(device);
}

const struct sgpu_operations sgpu_mtl_operations = {
    .create_instance = create_instance,
    .destroy_instance = destroy_instance,
    .create_default_device = create_default_device,
    .destroy_device = destroy_device,
};
