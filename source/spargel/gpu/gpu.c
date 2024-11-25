#include <spargel/base/base.h>
#include <spargel/gpu/gpu.h>
#include <spargel/gpu/operations.h>

#if SPARGEL_GPU_ENABLE_METAL
extern const struct sgpu_operations sgpu_mtl_operations;
#endif

#if SPARGEL_GPU_ENABLE_VULKAN
extern const struct sgpu_operations sgpu_vk_operations;
#endif

#if SPARGEL_GPU_ENABLE_DIRECTX
extern const struct sgpu_operations sgpu_dx_operations;
#endif

int sgpu_create_instance(struct sgpu_instance_descriptor const* descriptor,
                         sgpu_instance_id* instance) {
  switch (descriptor->backend) {
#if SPARGEL_GPU_ENABLE_METAL
    case SGPU_BACKEND_METAL:
      return sgpu_mtl_operations.create_instance(descriptor, instance);
#endif
#if SPARGEL_GPU_ENABLE_VULKAN
    case SGPU_BACKEND_VULKAN:
      return sgpu_vk_operations.create_instance(descriptor, instance);
#endif
#if SPARGEL_GPU_ENABLE_DIRECTX
    case SGPU_BACKEND_DIRECTX:
      return sgpu_dx_operations.create_instance(descriptor, instance);
#endif
    default:
      sbase_unreachable();
  }
}

#define OPS(object) ((struct sgpu_operations const*)object)

void sgpu_destroy_instance(sgpu_instance_id instance) {
  OPS(instance)->destroy_instance(instance);
}

int sgpu_create_default_device(sgpu_instance_id instance,
                               sgpu_device_id* device) {
  return OPS(instance)->create_default_device(instance, device);
}

void sgpu_destroy_device(sgpu_device_id device) {
  OPS(device)->destroy_device(device);
}
