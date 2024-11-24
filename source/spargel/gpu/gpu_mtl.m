#import <Metal/Metal.h>
#include <spargel/gpu/gpu_mtl.h>
#include <stdlib.h>

/**
 * We only target Apple Silicon.
 * This in particular guarantees that there is only one GPU.
 */

struct sgpu_mtl_instance {
  int backend;
};

struct sgpu_mtl_device {
  int backend;
  id<MTLDevice> device;
};

int sgpu_mtl_create_instance(sgpu_mtl_instance_id* instance,
                             struct sgpu_instance_descriptor const* descriptor) {
  struct sgpu_mtl_instance* inst = malloc(sizeof(struct sgpu_mtl_instance));
  if (!inst) return SGPU_RESULT_ALLOCATION_FAILED;
  inst->backend = SGPU_BACKEND_METAL;
  *instance = inst;
  return SGPU_RESULT_SUCCESS;
}

void sgpu_mtl_destroy_instance(sgpu_mtl_instance_id instance) { free(instance); }

int sgpu_mtl_create_default_device(sgpu_mtl_instance_id instance, sgpu_mtl_device_id* device) {
  struct sgpu_mtl_device* d = malloc(sizeof(struct sgpu_mtl_device));
  if (!d) return SGPU_RESULT_ALLOCATION_FAILED;
  d->backend = SGPU_BACKEND_METAL;
  d->device = MTLCreateSystemDefaultDevice();
  *device = d;
  return SGPU_RESULT_SUCCESS;
}

void sgpu_mtl_destroy_device(sgpu_mtl_device_id device) {
  [device->device release];
  free(device);
}
