#ifndef GPU_VULKAN_DEVICE_VULKAN_H_
#define GPU_VULKAN_DEVICE_VULKAN_H_

#include "gpu/device.h"
#include "gpu/vulkan/vulkan_headers.h"

namespace gpu::vulkan {

class DeviceVulkan : public Device {
 private:
  VkDevice handle_;
};

}  // namespace gpu::vulkan

#endif
