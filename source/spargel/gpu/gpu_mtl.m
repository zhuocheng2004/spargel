#import <Metal/Metal.h>
#include <spargel/gpu/gpu_mtl.h>
#include <stdlib.h>

/**
 * We only target Apple Silicon.
 * This in particular guarantees that there is only one GPU.
 */

struct sgpu_mtl_instance {
  enum sgpu_backend backend;
};

sgpu_result sgpu_mtl_create_instance(sgpu_mtl_instance_id* instance,
                                     struct sgpu_instance_descriptor const* descriptor) {
  struct sgpu_mtl_instance* inst = malloc(sizeof(struct sgpu_mtl_instance));
  if (!inst) return SGPU_RESULT_ALLOCATION_FAILED;
  inst->backend = SGPU_BACKEND_METAL;
  *instance = inst;
  return SGPU_RESULT_SUCCESS;
}
