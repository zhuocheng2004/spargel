#ifndef SAMPLES_VULKAN_TRIANGLE_UTILS_H_
#define SAMPLES_VULKAN_TRIANGLE_UTILS_H_

#include <vector>

#include "vulkan_triangle/vulkan_headers.h"

bool SelectLayerByName(char const* layer,
                       std::vector<VkLayerProperties> const& available,
                       std::vector<char const*>& selected);
bool SelectExtensionByName(char const* extension,
                           std::vector<VkExtensionProperties> const& available,
                           std::vector<char const*>& selected);

#endif
