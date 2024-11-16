#ifndef SAMPLES_VULKAN_TRIANGLE_PROC_TABLE_H_
#define SAMPLES_VULKAN_TRIANGLE_PROC_TABLE_H_

#include "samples/vulkan_triangle/vulkan_headers.h"

struct ProcTable {
#define DEFINE_FUNCTION_POINTER(name) PFN_##name name = nullptr;
#define VULKAN_LIBRARY_PROC(name) DEFINE_FUNCTION_POINTER(name)
#define VULKAN_GENERAL_PROC(name) DEFINE_FUNCTION_POINTER(name)
#define VULKAN_INSTANCE_PROC(name) DEFINE_FUNCTION_POINTER(name)
#define VULKAN_DEVICE_PROC(name) DEFINE_FUNCTION_POINTER(name)
#include "samples/vulkan_triangle/vulkan_procs.inc"
#undef VULKAN_DEVICE_PROC
#undef VULKAN_INSTANCE_PROC
#undef VULKAN_GENERAL_PROC
#undef VULKAN_LIBRARY_PROC
#undef DEFINE_FUNCTION_POINTER
};

#endif
