#ifndef SAMPLES_VULKAN_TRIANGLE_PROC_TABLE_H_
#define SAMPLES_VULKAN_TRIANGLE_PROC_TABLE_H_

#include "gpu/vulkan/vulkan_headers.h"

struct ProcTable {
  #define VULKAN_PROC(name) PFN_##name name = nullptr;
  #include "samples/vulkan_triangle/vulkan_procs.inc"
  #undef VULKAN_PROC
};

#endif
