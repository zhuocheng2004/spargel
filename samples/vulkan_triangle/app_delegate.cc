#include "samples/vulkan_triangle/app_delegate.h"

void AppDelegate::SetProcTable(ProcTable* table) { table_ = table; }

bool AppDelegate::SelectInstanceLayers(
    std::vector<VkLayerProperties> const& available,
    std::vector<char const*>& selected) {
  return true;
}

bool AppDelegate::SelectInstanceExtensions(
    std::vector<VkExtensionProperties> const& available,
    std::vector<char const*>& selected) {
  return true;
}
