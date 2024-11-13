#include "samples/vulkan_triangle/utils.h"

bool SelectLayerByName(char const* layer,
                       std::vector<VkLayerProperties> const& available,
                       std::vector<char const*>& selected) {
  auto iter = std::find_if(
      available.begin(), available.end(),
      [=](auto const& prop) { return strcmp(prop.layerName, layer) == 0; });
  if (iter != available.end()) {
    selected.push_back(layer);
    return true;
  }
  return false;
}

bool SelectExtensionByName(char const* extension,
                           std::vector<VkExtensionProperties> const& available,
                           std::vector<char const*>& selected) {
  auto iter =
      std::find_if(available.begin(), available.end(), [=](auto const& prop) {
        return strcmp(prop.extensionName, extension) == 0;
      });
  if (iter != available.end()) {
    selected.push_back(extension);
    return true;
  }
  return false;
}
