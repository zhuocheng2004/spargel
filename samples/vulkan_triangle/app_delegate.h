#ifndef SAMPLES_VULKAN_TRIANGLE_APP_DELEGATE_H_
#define SAMPLES_VULKAN_TRIANGLE_APP_DELEGATE_H_

#include <vector>

#include "samples/vulkan_triangle/vulkan_headers.h"
#include "samples/vulkan_triangle/proc_table.h"

class AppDelegate {
 public:
  virtual ~AppDelegate() = default;

  void SetProcTable(ProcTable* table);

  virtual bool LoadVkLibrary() = 0;
  virtual bool LoadVkGetInstanceProcAddr() = 0;

  virtual bool SelectInstanceLayers(
      std::vector<VkLayerProperties> const& available,
      std::vector<char const*>& selected);
  virtual bool SelectInstanceExtensions(
      std::vector<VkExtensionProperties> const& available,
      std::vector<char const*>& selected);
  virtual bool SelectDeviceExtensions(
      std::vector<VkExtensionProperties> const& available,
      std::vector<char const*>& selected);

  virtual bool CreateSurface(VkInstance instance, VkSurfaceKHR* surface,
                             ProcTable const& table) = 0;

  virtual uint32_t GetWidth() = 0;
  virtual uint32_t GetHeight() = 0;

  virtual void PollEvents() = 0;
  virtual bool ShouldQuit() = 0;

  virtual void Deinit() = 0;

 protected:
  ProcTable* table_ = nullptr;
};

#endif
