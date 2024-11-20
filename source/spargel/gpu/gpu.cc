#include "spargel/gpu/gpu.h"

#include "spargel/base/unreachable.h"

#if SPARGEL_GPU_ENABLE_DIRECTX
namespace spargel::gpu {
instance* createDirectXInstance();
}  // namespace spargel::gpu
#endif

#if SPARGEL_GPU_ENABLE_METAL
namespace spargel::gpu {
// these have to return instance* since instance_mtl must be incomplete here.
Instance* createMetalInstance();
}  // namespace spargel::gpu
#endif

#if SPARGEL_GPU_ENABLE_VULKAN
namespace spargel::gpu {
Instance* createVulkanInstance();
}  // namespace spargel::gpu
#endif

namespace spargel::gpu {

Instance* Instance::create() {
#if __APPLE__
  return createMetalInstance();
#endif
  base::unreachable();
}

Instance* Instance::create(BackendKind backend) {
  switch (backend) {
#if SPARGEL_GPU_ENABLE_DIRECTX
    case backend_kind::directx:
      return createDirectXInstance();
#endif
#if SPARGEL_GPU_ENABLE_METAL
    case BackendKind::metal:
      return createMetalInstance();
#endif
#if SPARGEL_GPU_ENABLE_VULKAN
    case BackendKind::vulkan:
      return createVulkanInstance();
#endif
    default:
      return nullptr;
  }
}

Instance::~Instance() {}

}  // namespace spargel::gpu
