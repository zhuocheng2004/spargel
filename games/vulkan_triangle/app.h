#ifndef SAMPLES_VULKAN_TRIANGLE_APP_H_
#define SAMPLES_VULKAN_TRIANGLE_APP_H_

#include <vector>

#include "spargel/ui/window_delegate.h"
#include "vulkan_triangle/app_delegate.h"
#include "vulkan_triangle/proc_table.h"
#include "vulkan_triangle/vulkan_headers.h"

class App final : public spargel::ui::WindowDelegate {
 public:
  explicit App(AppDelegate* delegate);

  bool Init();

  void Deinit();

  void render() override;

  void onClose() override;

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
  bool CreateCommandPool();
  bool CreateCommandBuffers();
  bool CreateFence(VkFlags flags, VkFence* fence);
  bool CreateSemaphore(VkFlags flags, VkSemaphore* semaphore);
  bool CreateRenderFences();
  bool CreateSwapchainSemaphores();
  bool CreateRenderSemaphores();
  bool CreateRenderPass();
  bool CreateFramebuffers();

  void DestroyInstance();
  void DestroyDebugMessenger();
  void DestroySurface();
  void DestroyDevice();
  void DestroySwapchain();
  void DestroySwapchainImageViews();
  void DestroyCommandPool();
  void DestroyRenderFences();
  void DestroySwapchainSemaphores();
  void DestroyRenderSemaphores();
  void DestroyFramebuffers();
  void DestroyRenderPass();

  bool WaitAndResetFence();
  bool AcquireNextImage(uint32_t* id);
  bool BeginCommandBuffer();
  bool EndCommandBuffer();
  void BeginRenderPass(VkCommandBuffer cmd, uint32_t image_id,
                       VkClearColorValue clear);
  // bool TransitionImage(VkCommandBuffer cmd, VkImage image,
  //                      VkImageLayout cur_layout, VkImageLayout new_layout);
  bool Submit();
  bool Present(uint32_t id);

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
  VkCommandPool command_pool_ = nullptr;

  VkRenderPass render_pass_ = nullptr;

  static constexpr size_t NumFramesInFlight = 2;
  struct FrameData {
    VkCommandBuffer command_buffer[NumFramesInFlight] = {nullptr};
    // signalled when rendering is finished
    VkFence render_fence[NumFramesInFlight] = {nullptr};
    // signalled when swapchain image is ready for rendering
    VkSemaphore swapchain_semaphore[NumFramesInFlight] = {nullptr};
    // signalled when rendering is finished
    VkSemaphore render_semaphore[NumFramesInFlight] = {nullptr};
  };
  FrameData frames_;
  size_t current_frame_ = 0;

  std::vector<VkImage> swapchain_images_;
  std::vector<VkImageView> swapchain_image_views_;
  std::vector<VkFramebuffer> framebuffers_;
  VkSurfaceFormatKHR surface_format_;

  uint32_t width_ = 0;
  uint32_t height_ = 0;

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
  // bool has_api_dump_ = false;
  bool has_debug_utils_ = false;
  // if VK_KHR_portability_enumeration is present, selecte it
  // note: the MoltenVK driver is non-conformant
  // note: is it possible that we link with MoltenVK directly in shipping build?
  // todo: consider move this to platform specific logic
  bool has_portability_ = false;

  int frame_count_ = 0;
  bool closing_ = false;
};

#endif
