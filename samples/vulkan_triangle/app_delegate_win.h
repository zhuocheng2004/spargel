#ifndef SAMPLES_VULKAN_TRIANGLE_APP_DELEGATE_WIN_H_
#define SAMPLES_VULKAN_TRIANGLE_APP_DELEGATE_WIN_H_

#include <vector>

#include "samples/vulkan_triangle/app_delegate.h"
#include "samples/vulkan_triangle/vulkan_headers.h"

class AppDelegateWin final : public AppDelegate {
 public:
  AppDelegateWin();
  ~AppDelegateWin() override;

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

  void PollEvents() override;
  bool ShouldQuit() override;

  void Deinit() override;

  void SetShouldQuit(bool v);

 private:
  bool DoCreateWindow();

  HMODULE module_ = nullptr;
  HWND hwnd_ = nullptr;

  bool should_quit_ = false;
};

#endif
