#ifndef SAMPLES_VULKAN_TRIANGLE_APP_H_
#define SAMPLES_VULKAN_TRIANGLE_APP_H_

#include <vector>

#include "gpu/vulkan/vulkan_headers.h"
#include "samples/vulkan_triangle/app_delegate.h"
#include "samples/vulkan_triangle/proc_table.h"

class App {
 public:
  explicit App(AppDelegate* delegate);

  bool Init();

  void Deinit();

  void Run();

 private:
  bool LoadGeneralProcs();
  bool EnumerateInstanceLayers();
  void PrintInstanceLayers();
  bool EnumerateInstanceLayerExtensions(char const* layer);
  bool EnumerateInstanceExtensions();
  void PrintInstanceExtensions();
  bool SelectInstanceLayers();
  bool SelectInstanceExtensions();
  void PrintSelectedInstanceLayers();
  void PrintSelectedInstanceExtensions();
  bool CreateInstance();
  bool LoadInstanceProcs();
  bool CreateDebugMessenger();
  bool EnumeratePhysicalDevices();
  bool LoadDeviceProcs();

  void DestroyInstance();
  void DestroyDebugMessenger();

  static VkBool32 DebugMessageCallback(
      VkDebugUtilsMessageSeverityFlagBitsEXT severity,
      VkDebugUtilsMessageTypeFlagsEXT type,
      VkDebugUtilsMessengerCallbackDataEXT const* data, void* user_data);

  AppDelegate* delegate_ = nullptr;

  ProcTable table_;

  VkInstance instance_ = nullptr;
  VkDebugUtilsMessengerEXT debug_messenger_ = nullptr;

  std::vector<VkLayerProperties> instance_layers_;
  std::vector<VkExtensionProperties> instance_extensions_;
  std::vector<char const*> selected_instance_layers_;
  std::vector<char const*> selected_instance_extensions_;

  std::vector<VkPhysicalDevice> physical_devices_;

  bool has_validation_ = false;
  bool has_api_dump_ = false;
  bool has_debug_utils_ = false;
  // if VK_KHR_portability_enumeration is present, selecte it
  // note: the MoltenVK driver is non-conformant
  // note: is it possible that we link with MoltenVK directly in shipping build?
  // todo: consider move this to platform specific logic
  bool has_portability_ = false;
};

#endif
