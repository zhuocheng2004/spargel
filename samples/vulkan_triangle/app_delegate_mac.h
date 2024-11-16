#ifndef SAMPLES_VULKAN_TRIANGLE_APP_DELEGATE_MAC_H_
#define SAMPLES_VULKAN_TRIANGLE_APP_DELEGATE_MAC_H_

#include <vector>

#include "samples/vulkan_triangle/vulkan_headers.h"
#include "samples/vulkan_triangle/app_delegate.h"

class AppDelegateMac final : public AppDelegate {
 public:
  AppDelegateMac();
  ~AppDelegateMac() override;

  bool LoadLibrary() override;
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

  void PollEvents() override;
  bool ShouldQuit() override;

  void Deinit() override;

  void SetShouldQuit(bool v);

 private:
  bool CreateWindow();

  void* library_ = nullptr;

  bool should_quit_ = false;

  struct ObjcData;
  ObjcData* data_;
};

#endif
