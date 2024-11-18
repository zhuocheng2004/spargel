#include "vulkan_triangle/app_delegate_mac.h"

#include <dlfcn.h>

#include "spargel/ui/metal/render_target_mtl.h"
#include "vulkan_triangle/logging.h"
#include "vulkan_triangle/utils.h"

using base::logging::Log;
using enum base::logging::LogLevel;

AppDelegateMac::AppDelegateMac(spargel::ui::Window* window) { _window = window; }
AppDelegateMac::~AppDelegateMac() {}

bool AppDelegateMac::LoadVkLibrary() {
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

bool AppDelegateMac::SelectDeviceExtensions(std::vector<VkExtensionProperties> const& available,
                                            std::vector<char const*>& selected) {
  return true;
}

bool AppDelegateMac::CreateSurface(VkInstance instance, VkSurfaceKHR* surface,
                                   ProcTable const& table) {
  auto render_target = static_cast<spargel::ui::RenderTargetMTL*>(_window->renderTarget());

  VkMetalSurfaceCreateInfoEXT info;
  info.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
  info.pNext = nullptr;
  info.flags = 0;
  info.pLayer = render_target->layer();
  auto result = table.vkCreateMetalSurfaceEXT(instance, &info, nullptr, surface);
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot create metal surface");
    return false;
  }
  return true;
}

void AppDelegateMac::Deinit() { dlclose(library_); }

uint32_t AppDelegateMac::GetWidth() { return _window->width(); }
uint32_t AppDelegateMac::GetHeight() { return _window->height(); }
