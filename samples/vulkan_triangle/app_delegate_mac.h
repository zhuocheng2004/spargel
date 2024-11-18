#ifndef SAMPLES_VULKAN_TRIANGLE_APP_DELEGATE_MAC_H_
#define SAMPLES_VULKAN_TRIANGLE_APP_DELEGATE_MAC_H_

#include <vector>

#include "samples/vulkan_triangle/vulkan_headers.h"
#include "samples/vulkan_triangle/app_delegate.h"
#include "modules/ui/public/window.h"

class AppDelegateMac final : public AppDelegate {
 public:
  explicit AppDelegateMac(spargel::ui::Window* window);
  ~AppDelegateMac() override;

  bool LoadVkLibrary() override;
  bool LoadVkGetInstanceProcAddr() override;

  bool SelectInstanceLayers(std::vector<VkLayerProperties> const& available,
                            std::vector<char const*>& selected) override;
  bool SelectInstanceExtensions(
      std::vector<VkExtensionProperties> const& available,
      std::vector<char const*>& selected) override;
  bool SelectDeviceExtensions(
      std::vector<VkExtensionProperties> const& available,
      std::vector<char const*>& selected) override;

  bool CreateSurface(VkInstance instance, VkSurfaceKHR* surface,
                     ProcTable const& table) override;

  uint32_t GetWidth() override;
  uint32_t GetHeight() override;

  void Deinit() override;

 private:
  void* library_ = nullptr;
  spargel::ui::Window* _window;
};

#endif
