module;

module spargel.gpu;

import spargel.base.c;
import spargel.base.assert;

#if SPARGEL_GPU_ENABLE_METAL
namespace spargel::gpu {
    base::UniquePtr<Device> CreateMetalDevice();
}
#endif

#if SPARGEL_GPU_ENABLE_VULKAN
namespace spargel::gpu {
    base::UniquePtr<Device> CreateVulkanDevice();
}
#endif

#if SPARGEL_GPU_ENABLE_DIRECTX
import :directx;
#endif

namespace spargel::gpu {

    base::UniquePtr<Device> CreateDevice() {
#if SPARGEL_GPU_ENABLE_METAL
        return CreateMetalDevice();
#elif SPARGEL_GPU_ENABLE_VULKAN
        return CreateVulkanDevice();
#endif
        base::unreachable();
    }

}
