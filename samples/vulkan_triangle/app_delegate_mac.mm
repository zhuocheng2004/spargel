#include "samples/vulkan_triangle/app_delegate_mac.h"

#include <dlfcn.h>

#include "base/logging/logging.h"

using base::logging::Log;
using enum base::logging::LogLevel;

bool AppDelegateMac::LoadLibrary() {
  library_ = dlopen("libMoltenVK.dylib", RTLD_NOW | RTLD_LOCAL);
  if (!library_) {
    Log(Error, "cannot load libMoltenVK.dylib");
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

void AppDelegateMac::Deinit() { dlclose(library_); }
