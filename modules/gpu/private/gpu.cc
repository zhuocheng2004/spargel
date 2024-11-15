#include "modules/gpu/public/gpu.h"

#include "modules/gpu/build_flags.h"

#if BUILD_FLAG(ENABLE_DIRECTX)
#include "modules/gpu/private/directx/directx.h"
namespace spargel::gpu {
instance* create_directx_instance(instance_descriptor const& desc);
}  // namespace spargel::gpu
#endif

#if BUILD_FLAG(ENABLE_METAL)
#include "modules/gpu/private/metal/metal.h"
namespace spargel::gpu {
// these have to return instance* since instance_mtl must be incomplete here.
instance* create_metal_instance(instance_descriptor const& desc);
}  // namespace spargel::gpu
#endif

#if BUILD_FLAG(ENABLE_VULKAN)
#include "modules/gpu/private/vulkan/vulkan.h"
namespace spargel::gpu {
instance* create_vulkan_instance(instance_descriptor const& desc);
}  // namespace spargel::gpu
#endif

namespace spargel::gpu {

// static
instance* instance::create(instance_descriptor const& desc) {
  switch (desc.backend) {
#if BUILD_FLAG(ENABLE_DIRECTX)
    case directx:
      return create_directx_instance(desc);
#endif
#if BUILD_FLAG(ENABLE_METAL)
    case metal:
      return create_metal_instance(desc);
#endif
#if BUILD_FLAG(ENABLE_VULKAN)
    case vulkan:
      return create_vulkan_instance(desc);
#endif
    default:
      return nullptr;
  }
}

instance::~instance() = default;

}  // namespace spargel::gpu
