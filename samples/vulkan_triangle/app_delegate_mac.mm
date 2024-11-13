#include "samples/vulkan_triangle/app_delegate_mac.h"

#include <dlfcn.h>

#include "base/logging/logging.h"
#include "samples/vulkan_triangle/utils.h"

using base::logging::Log;
using enum base::logging::LogLevel;

bool AppDelegateMac::LoadLibrary() {
  // library_ = dlopen("libMoltenVK.dylib", RTLD_NOW | RTLD_LOCAL);
  library_ = dlopen("libvulkan.dylib", RTLD_NOW | RTLD_LOCAL);
  if (!library_) {
    Log(Error, "cannot load libvulkan.dylib");
    return false;
  }
  return true;
}

bool AppDelegateMac::LoadVkGetInstanceProcAddr() {
  table_->vkGetInstanceProcAddr =
      reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(library_, "vkGetInstanceProcAddr"));
  if (!table_->vkGetInstanceProcAddr) {
    Log(Error, "cannot load vkGetInstanceProcAddr");
    return false;
  }
  return true;
}

bool AppDelegateMac::SelectInstanceLayers(std::vector<VkLayerProperties> const& available,
                                          std::vector<char const*>& selected) {
  return true;
}

bool AppDelegateMac::SelectInstanceExtensions(std::vector<VkExtensionProperties> const& available,
                                              std::vector<char const*>& selected) {
  auto has_surface = SelectExtensionByName("VK_EXT_metal_surface", available, selected);
  if (!has_surface) {
    Log(Error, "cannot find extension VK_EXT_metal_surface");
    return false;
  }
  return true;
}

void AppDelegateMac::Deinit() { dlclose(library_); }
