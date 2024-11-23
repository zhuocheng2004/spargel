#include <spargel/base/base.h>
#include <spargel/gpu/gpu.h>

#if SPARGEL_GPU_ENABLE_METAL
#include <spargel/gpu/gpu_mtl.h>
#endif

#if SPARGEL_GPU_ENABLE_VULKAN
#include <spargel/gpu/gpu_vk.h>
#endif

#if SPARGEL_GPU_ENABLE_DIRECTX
#include <spargel/gpu/gpu_dx.h>
#endif

sgpu_result sgpu_create_instance(sgpu_instance_id* instance, sgpu_instance_descriptor const* descriptor) {
  switch (descriptor->backend) {
#if SPARGEL_GPU_ENABLE_METAL
    case SGPU_BACKEND_METAL:
      return sgpu_mtl_create_instance((sgpu_mtl_instance_id*)instance, descriptor);
#endif
#if SPARGEL_GPU_ENABLE_VULKAN
    case SGPU_BACKEND_VULKAN:
      return sgpu_vk_create_instance((sgpu_vk_instance_id*)instance, descriptor);
#endif
#if SPARGEL_GPU_ENABLE_DIRECTX
    case SGPU_BACKEND_DIRECTX:
      return sgpu_dx_create_instance((sgpu_dx_instance_id*)instance, descriptor);
#endif
    default:
      sbase_unreachable();
  }
}
void sgpu_destroy_instance(sgpu_instance_id instance) {
  sgpu_backend backend = *(sgpu_backend*)instance;
  switch (backend) {
#if SPARGEL_GPU_ENABLE_METAL
    case SGPU_BACKEND_METAL:
      sgpu_mtl_destroy_instance((sgpu_mtl_instance_id)instance);
      break;
#endif
#if SPARGEL_GPU_ENABLE_VULKAN
    case SGPU_BACKEND_VULKAN:
      sgpu_vk_destroy_instance((sgpu_vk_instance_id)instance);
      break;
#endif
#if SPARGEL_GPU_ENABLE_DIRECTX
    case SGPU_BACKEND_DIRECTX:
      sgpu_dx_destroy_instance((sgpu_dx_instance_id)instance);
      break;
#endif
    default:
      sbase_unreachable();
  }
}
sgpu_result sgpu_create_default_device(sgpu_instance_id instance, sgpu_device_id* device) {
  sgpu_backend backend = *(sgpu_backend*)instance;
  switch (backend) {
#if SPARGEL_GPU_ENABLE_METAL
    case SGPU_BACKEND_METAL:
      return sgpu_mtl_create_default_device((sgpu_mtl_instance_id)instance, (sgpu_mtl_device_id*)device);
#endif
#if SPARGEL_GPU_ENABLE_VULKAN
    case SGPU_BACKEND_VULKAN:
      return sgpu_vk_create_default_device((sgpu_vk_instance_id)instance, (sgpu_vk_device_id*)device);
#endif
#if SPARGEL_GPU_ENABLE_DIRECTX
    case SGPU_BACKEND_DIRECTX:
      return sgpu_dx_create_default_device((sgpu_dx_instance_id)instance, (sgpu_dx_device_id*)device);
#endif
    default:
      sbase_unreachable();
  }
}
void sgpu_destroy_device(sgpu_device_id device) {
  sgpu_backend backend = *(sgpu_backend*)device;
  switch (backend) {
#if SPARGEL_GPU_ENABLE_METAL
    case SGPU_BACKEND_METAL:
      sgpu_mtl_destroy_device((sgpu_mtl_device_id)device);
      break;
#endif
#if SPARGEL_GPU_ENABLE_VULKAN
    case SGPU_BACKEND_VULKAN:
      sgpu_vk_destroy_device((sgpu_vk_device_id)device);
      break;
#endif
#if SPARGEL_GPU_ENABLE_DIRECTX
    case SGPU_BACKEND_DIRECTX:
      sgpu_dx_destroy_device((sgpu_dx_device_id)device);
      break;
#endif
    default:
      sbase_unreachable();
  }
}
