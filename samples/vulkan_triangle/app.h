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
  bool QueryDeviceExtensions(size_t idx);
  void QueryDeviceQueueFamilies(size_t idx);
  bool QueryDeviceSurfaceCapabilities(size_t idx);
  bool QueryDeviceSurfaceFormats(size_t idx);
  bool QueryDevicePresentModes(size_t idx);
  bool QueryPhysicalDeviceProperties();
  bool SelectPhysicalDevice();
  bool SelectDeviceExtensions();
  bool SelectQueues();
  bool SelectDeviceFeatures();
  bool CreateDevice();
  bool LoadDeviceProcs();
  void GetQueue();
  void SelectSurfaceFormat();
  bool CreateSwapchain();
  bool GetSwapchainImages();
  bool CreateSwapchainImageViews();

  void DestroyInstance();
  void DestroyDebugMessenger();
  void DestroySurface();
  void DestroyDevice();
  void DestroySwapchain();
  void DestroySwapchainImageViews();

  static VkBool32 DebugMessageCallback(
      VkDebugUtilsMessageSeverityFlagBitsEXT severity,
      VkDebugUtilsMessageTypeFlagsEXT type,
      VkDebugUtilsMessengerCallbackDataEXT const* data, void* user_data);

  AppDelegate* delegate_ = nullptr;

  ProcTable table_;

  VkInstance instance_ = nullptr;
  VkDebugUtilsMessengerEXT debug_messenger_ = nullptr;
  VkSurfaceKHR surface_ = nullptr;
  VkPhysicalDevice physical_device_ = nullptr;
  size_t physical_device_id_ = 0;
  uint32_t queue_family_id_ = 0;
  VkDevice device_ = nullptr;
  VkQueue queue_ = nullptr;
  VkSwapchainKHR swapchain_ = nullptr;

  std::vector<VkImage> swapchain_images_;
  std::vector<VkImageView> swapchain_image_views_;
  VkSurfaceFormatKHR surface_format_;

  std::vector<VkLayerProperties> instance_layers_;
  std::vector<VkExtensionProperties> instance_extensions_;
  std::vector<char const*> selected_instance_layers_;
  std::vector<char const*> selected_instance_extensions_;
  std::vector<char const*> selected_device_extensions_;
  VkPhysicalDeviceFeatures device_features_ = {};

  std::vector<VkPhysicalDevice> physical_devices_;

  struct PhysicalDeviceProperty {
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    std::vector<VkExtensionProperties> extensions;
    std::vector<VkQueueFamilyProperties> queue_families;
    VkSurfaceCapabilitiesKHR surface_capabilities;
    std::vector<VkSurfaceFormatKHR> surface_formats;
    std::vector<VkPresentModeKHR> present_modes;
  };
  std::vector<PhysicalDeviceProperty> physical_device_properties_;

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
