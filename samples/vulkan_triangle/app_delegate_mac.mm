#include "samples/vulkan_triangle/app_delegate_mac.h"

#include <dlfcn.h>

#import <AppKit/NSWindow.h>
#import <QuartzCore/CAMetalLayer.h>

#include "base/logging/logging.h"
#include "samples/vulkan_triangle/utils.h"

using base::logging::Log;
using enum base::logging::LogLevel;

struct AppDelegateMac::ObjcData {
  NSWindow* window;
  NSView* view;
  CAMetalLayer* layer;
};

AppDelegateMac::AppDelegateMac() { data_ = new ObjcData; }
AppDelegateMac::~AppDelegateMac() { delete data_; }

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

bool AppDelegateMac::SelectDeviceExtensions(std::vector<VkExtensionProperties> const& available,
                                            std::vector<char const*>& selected) {
  return true;
}

bool AppDelegateMac::CreateSurface(VkInstance instance, VkSurfaceKHR* surface,
                                   ProcTable const& table) {
  if (!CreateWindow()) return false;
  VkMetalSurfaceCreateInfoEXT info;
  info.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
  info.pNext = nullptr;
  info.flags = 0;
  info.pLayer = data_->layer;
  auto result = table.vkCreateMetalSurfaceEXT(instance, &info, nullptr, surface);
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot create metal surface");
    return false;
  }
  return true;
}

void AppDelegateMac::Deinit() { dlclose(library_); }

bool AppDelegateMac::CreateWindow() {
  auto style = NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable |
               NSWindowStyleMaskResizable | NSWindowStyleMaskTitled;
  auto rect = NSMakeRect(0, 0, 500, 500);
  data_->window = [[NSWindow alloc] initWithContentRect:rect
                                              styleMask:style
                                                backing:NSBackingStoreBuffered
                                                  defer:NO];
  if (!data_->window) {
    LOG(Error, "cannot create window");
    return false;
  }

  data_->view = [[NSView alloc] init];
  if (!data_->view) {
    LOG(Error, "cannot create view");
    return false;
  }
  [data_->window setContentView:data_->view];

  data_->layer = [CAMetalLayer layer];
  if (!data_->layer) {
    LOG(Error, "cannot create layer");
    return false;
  }
  [data_->view setWantsLayer:YES];
  [data_->view setLayer:data_->layer];
  return true;
}

uint32_t AppDelegateMac::GetWidth() { return data_->view.bounds.size.width; }
uint32_t AppDelegateMac::GetHeight() { return data_->view.bounds.size.height; }
