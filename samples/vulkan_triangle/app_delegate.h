#ifndef SAMPLES_VULKAN_TRIANGLE_APP_DELEGATE_H_
#define SAMPLES_VULKAN_TRIANGLE_APP_DELEGATE_H_

#include "samples/vulkan_triangle/proc_table.h"

class AppDelegate {
 public:
  void SetProcTable(ProcTable* table);

  virtual bool LoadLibrary() = 0;
  virtual bool LoadVkGetInstanceProcAddr() = 0;

  virtual void Deinit() = 0;

 protected:
  ProcTable* table_ = nullptr;
};

#endif
