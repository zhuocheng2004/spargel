#ifndef GPU_VULKAN_VULKAN_HEADERS_H_
#define GPU_VULKAN_VULKAN_HEADERS_H_

// Do not use function declarations, as we have a custom loader.
#define VK_NO_PROTOTYPES

#include "vulkan/vk_platform.h"  // IWYU pragma: export
#include "vulkan/vulkan_core.h"  // IWYU pragma: export

// TODO: vulkan_win32.h
// TODO: vulkan_wayland.h

// TODO: vulkan_metal.h
#if defined(__APPLE__)
#include "vulkan/vulkan_metal.h"
#endif

#endif
