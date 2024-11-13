#ifndef SAMPLES_VULKAN_TRIANGLE_APP_DELEGATE_MAC_H_
#define SAMPLES_VULKAN_TRIANGLE_APP_DELEGATE_MAC_H_

#include <vector>

#include "gpu/vulkan/vulkan_headers.h"
#include "samples/vulkan_triangle/app_delegate.h"

class AppDelegateMac final : public AppDelegate {
 public:
  bool LoadLibrary() override;
  bool LoadVkGetInstanceProcAddr() override;

  bool SelectInstanceLayers(std::vector<VkLayerProperties> const& available,
                            std::vector<char const*>& selected) override;
  bool SelectInstanceExtensions(
      std::vector<VkExtensionProperties> const& available,
      std::vector<char const*>& selected) override;

  void Deinit() override;

 private:
  void* library_ = nullptr;
};

#endif
