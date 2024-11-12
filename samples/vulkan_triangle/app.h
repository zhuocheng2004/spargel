#ifndef SAMPLES_VULKAN_TRIANGLE_APP_H_
#define SAMPLES_VULKAN_TRIANGLE_APP_H_

#include "gpu/vulkan/vulkan_headers.h"
#include "samples/vulkan_triangle/app_delegate.h"
#include "samples/vulkan_triangle/proc_table.h"

class App {
 public:
  explicit App(AppDelegate* delegate);

  bool Init();
  bool CreateInstance();

  void Deinit();

  void Run();

 private:
  AppDelegate* delegate_ = nullptr;
  ProcTable table_;
  VkInstance instance_ = nullptr;
};

#endif
