#ifndef SAMPLES_VULKAN_TRIANGLE_APP_DELEGATE_MAC_H_
#define SAMPLES_VULKAN_TRIANGLE_APP_DELEGATE_MAC_H_

#include "samples/vulkan_triangle/app_delegate.h"

class AppDelegateMac final : public AppDelegate {
 public:
  bool LoadLibrary() override;
  bool LoadVkGetInstanceProcAddr() override;

  void Deinit() override;

 private:
  void* library_ = nullptr;
};

#endif
