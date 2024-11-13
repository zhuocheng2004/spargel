#include "samples/vulkan_triangle/app.h"

#include <math.h>
#include <stdio.h>

#include "base/logging/logging.h"
#include "samples/vulkan_triangle/utils.h"

using base::logging::Log;
using enum base::logging::LogLevel;

App::App(AppDelegate* delegate) : delegate_{delegate} {
  delegate_->SetProcTable(&table_);
}

// todo: this approach is bad, for there is no way to clean up resourcecs in
// failure path
bool App::Init() {
  if (!delegate_->LoadLibrary()) return false;
  if (!delegate_->LoadVkGetInstanceProcAddr()) return false;
  if (!LoadGeneralProcs()) return false;
  if (!EnumerateInstanceLayers()) return false;
  // PrintInstanceLayers();
  if (!EnumerateInstanceExtensions()) return false;
  // PrintInstanceExtensions();
  if (!SelectInstanceLayers()) return false;
  if (!delegate_->SelectInstanceLayers(instance_layers_,
                                       selected_instance_layers_))
    return false;
  if (!SelectInstanceExtensions()) return false;
  if (!delegate_->SelectInstanceExtensions(instance_extensions_,
                                           selected_instance_extensions_))
    return false;
  PrintSelectedInstanceLayers();
  PrintSelectedInstanceExtensions();
  if (!CreateInstance()) return false;
  LOG(Info, "instance created");
  if (!LoadInstanceProcs()) return false;
  if (has_debug_utils_) {
    if (!CreateDebugMessenger()) return false;
    LOG(Info, "debug messenger created");
  }
  if (!delegate_->CreateSurface(instance_, &surface_, table_)) return false;
  LOG(Info, "surface created");
  if (!EnumeratePhysicalDevices()) return false;
  if (!QueryPhysicalDeviceProperties()) return false;
  if (!SelectPhysicalDevice()) return false;
  if (!SelectDeviceExtensions()) return false;
  if (!delegate_->SelectDeviceExtensions(
          physical_device_properties_[physical_device_id_].extensions,
          selected_device_extensions_))
    return false;
  if (!SelectDeviceFeatures()) return false;
  if (!CreateDevice()) return false;
  if (!LoadDeviceProcs()) return false;
  LOG(Info, "device created");
  GetQueue();
  SelectSurfaceFormat();
  if (!CreateSwapchain()) return false;
  LOG(Info, "swapchain created");
  if (!GetSwapchainImages()) return false;
  if (!CreateSwapchainImageViews()) return false;
  if (!CreateCommandPool()) return false;
  if (!CreateCommandBuffers()) return false;
  if (!CreateRenderFences()) return false;
  if (!CreateSwapchainSemaphores()) return false;
  if (!CreateRenderSemaphores()) return false;
  return true;
}

bool App::LoadGeneralProcs() {
  auto vkGetInstanceProcAddr = table_.vkGetInstanceProcAddr;
#define VULKAN_GENERAL_PROC(name)                                          \
  table_.name =                                                            \
      reinterpret_cast<PFN_##name>(vkGetInstanceProcAddr(nullptr, #name)); \
  if (!table_.name) {                                                      \
    LOG(Error, "cannot load " #name);                                      \
    return false;                                                          \
  }
#include "samples/vulkan_triangle/vulkan_procs.inc"
#undef VULKAN_GENERAL_PROC
  return true;
}

bool App::EnumerateInstanceLayers() {
  uint32_t count;
  auto result = table_.vkEnumerateInstanceLayerProperties(&count, nullptr);
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot enumerate instance layers");
    return false;
  }
  instance_layers_.resize(count);
  result = table_.vkEnumerateInstanceLayerProperties(&count,
                                                     instance_layers_.data());
  instance_layers_.resize(count);
  return true;
}

void App::PrintInstanceLayers() {
  printf("list of instance layers (%zu in total)\n", instance_layers_.size());
  for (int i = 0; i < instance_layers_.size(); i++) {
    printf("  %s\n", instance_layers_[i].layerName);
  }
  printf("\n");
}

bool App::EnumerateInstanceLayerExtensions(char const* layer) {
  uint32_t count;
  std::vector<VkExtensionProperties> exts;
  auto result =
      table_.vkEnumerateInstanceExtensionProperties(layer, &count, nullptr);
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot enumerate instance extensions");
    return false;
  }
  exts.resize(count);
  result =
      table_.vkEnumerateInstanceExtensionProperties(layer, &count, exts.data());
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot enumerate instance extensions");
    return false;
  }
  exts.resize(count);
  instance_extensions_.insert(instance_extensions_.end(), exts.begin(),
                              exts.end());
  return true;
}

bool App::EnumerateInstanceExtensions() {
  // Spec:
  // When pLayerName parameter is NULL, only extensions provided by the Vulkan
  // implementation or by implicitly enabled layers are returned.
  if (!EnumerateInstanceLayerExtensions(nullptr)) return false;
  for (auto const& layer_prop : instance_layers_) {
    if (!EnumerateInstanceLayerExtensions(layer_prop.layerName)) return false;
  }
  return true;
}

void App::PrintInstanceExtensions() {
  printf("list of instance extensions (%zu in total)\n",
         instance_extensions_.size());
  for (int i = 0; i < instance_extensions_.size(); i++) {
    printf("  %s\n", instance_extensions_[i].extensionName);
  }
  printf("\n");
}

bool App::SelectInstanceLayers() {
  has_validation_ =
      SelectLayerByName("VK_LAYER_KHRONOS_validation", instance_layers_,
                        selected_instance_layers_);
  // has_api_dump_ = SelectLayerByName(
  //     "VK_LAYER_LUNARG_api_dump", instance_layers_,
  //     selected_instance_layers_);
  return true;
}

void App::PrintSelectedInstanceLayers() {
  LOG(Info, "list of selected instance layers (",
      selected_instance_layers_.size(), " in total)");
  for (auto name : selected_instance_layers_) {
    LOG(Info, "  ", name);
  }
}

void App::PrintSelectedInstanceExtensions() {
  LOG(Info, "list of selected instance extensions (",
      selected_instance_extensions_.size(), " in total)");
  for (auto name : selected_instance_extensions_) {
    LOG(Info, "  ", name);
  }
}

bool App::SelectInstanceExtensions() {
  has_debug_utils_ =
      SelectExtensionByName("VK_EXT_debug_utils", instance_extensions_,
                            selected_instance_extensions_);
  auto has_surface = SelectExtensionByName(
      "VK_KHR_surface", instance_extensions_, selected_instance_extensions_);
  if (!has_surface) {
    LOG(Error, "cannot find extension VK_KHR_surface");
    return false;
  }
  has_portability_ = SelectExtensionByName("VK_KHR_portability_enumeration",
                                           instance_extensions_,
                                           selected_instance_extensions_);
  return true;
}

bool App::CreateInstance() {
  VkApplicationInfo app_info;
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pNext = nullptr;
  app_info.pApplicationName = "vulkan_triangle";
  app_info.applicationVersion = 0;
  app_info.pEngineName = "spargel";
  app_info.engineVersion = 0;
  app_info.apiVersion = VK_API_VERSION_1_2;
  VkInstanceCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.pApplicationInfo = &app_info;
  info.enabledLayerCount = selected_instance_layers_.size();
  info.ppEnabledLayerNames = selected_instance_layers_.data();
  info.enabledExtensionCount = selected_instance_extensions_.size();
  info.ppEnabledExtensionNames = selected_instance_extensions_.data();

  if (has_portability_) {
    info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  }

  auto result = table_.vkCreateInstance(&info, nullptr, &instance_);
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot create instance");
    return false;
  }
  return true;
}

bool App::LoadInstanceProcs() {
  auto vkGetInstanceProcAddr = table_.vkGetInstanceProcAddr;
#define VULKAN_INSTANCE_PROC(name)                                           \
  table_.name =                                                              \
      reinterpret_cast<PFN_##name>(vkGetInstanceProcAddr(instance_, #name)); \
  if (!table_.name) {                                                        \
    Log(Error, "cannot load " #name);                                        \
    return false;                                                            \
  }
#include "samples/vulkan_triangle/vulkan_procs.inc"
#undef VULKAN_INSTANCE_PROC
  return true;
}

bool App::CreateDebugMessenger() {
  VkDebugUtilsMessengerCreateInfoEXT info;
  info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  info.pNext = nullptr;
  info.flags = 0;
  info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  info.pfnUserCallback = DebugMessageCallback;
  info.pUserData = this;
  auto result = table_.vkCreateDebugUtilsMessengerEXT(instance_, &info, nullptr,
                                                      &debug_messenger_);
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot create debug messenger");
    return false;
  }
  return true;
}

bool App::EnumeratePhysicalDevices() {
  uint32_t count;
  auto result = table_.vkEnumeratePhysicalDevices(instance_, &count, nullptr);
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot enumerate physical devices");
    return false;
  }
  physical_devices_.resize(count);
  result = table_.vkEnumeratePhysicalDevices(instance_, &count,
                                             physical_devices_.data());
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot enumerate physical devices");
    return false;
  }
  physical_devices_.resize(count);
  return true;
}

bool App::QueryDeviceExtensions(size_t idx) {
  auto adapter = physical_devices_[idx];
  auto& prop = physical_device_properties_[idx];
  uint32_t count;
  auto result = table_.vkEnumerateDeviceExtensionProperties(adapter, nullptr,
                                                            &count, nullptr);
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot enumerate device extensions");
    return false;
  }
  prop.extensions.resize(count);
  result = table_.vkEnumerateDeviceExtensionProperties(adapter, nullptr, &count,
                                                       prop.extensions.data());
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot enumerate device extensions");
    return false;
  }
  prop.extensions.resize(count);
  return true;
}

void App::QueryDeviceQueueFamilies(size_t idx) {
  auto adapter = physical_devices_[idx];
  auto& prop = physical_device_properties_[idx];
  uint32_t count;
  table_.vkGetPhysicalDeviceQueueFamilyProperties(adapter, &count, nullptr);
  prop.queue_families.resize(count);
  table_.vkGetPhysicalDeviceQueueFamilyProperties(adapter, &count,
                                                  prop.queue_families.data());
  prop.queue_families.resize(count);
}

bool App::QueryDeviceSurfaceCapabilities(size_t idx) {
  auto adapter = physical_devices_[idx];
  auto& prop = physical_device_properties_[idx];
  auto result = table_.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
      adapter, surface_, &prop.surface_capabilities);
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot query surface capabilities");
    return false;
  }
  return true;
}

bool App::QueryDeviceSurfaceFormats(size_t idx) {
  auto adapter = physical_devices_[idx];
  auto& prop = physical_device_properties_[idx];
  uint32_t count;
  auto result = table_.vkGetPhysicalDeviceSurfaceFormatsKHR(adapter, surface_,
                                                            &count, nullptr);
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot query surface formats");
    return false;
  }
  prop.surface_formats.resize(count);
  result = table_.vkGetPhysicalDeviceSurfaceFormatsKHR(
      adapter, surface_, &count, prop.surface_formats.data());
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot query surface formats");
    return false;
  }
  prop.surface_formats.resize(count);
  return true;
}

bool App::QueryDevicePresentModes(size_t idx) {
  auto adapter = physical_devices_[idx];
  auto& prop = physical_device_properties_[idx];
  uint32_t count;
  auto result = table_.vkGetPhysicalDeviceSurfacePresentModesKHR(
      adapter, surface_, &count, nullptr);
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot query present modes");
    return false;
  }
  prop.present_modes.resize(count);
  result = table_.vkGetPhysicalDeviceSurfacePresentModesKHR(
      adapter, surface_, &count, prop.present_modes.data());
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot query present modes");
    return false;
  }
  prop.present_modes.resize(count);
  return true;
}

bool App::QueryPhysicalDeviceProperties() {
  physical_device_properties_.resize(physical_devices_.size());

  for (int i = 0; i < physical_devices_.size(); i++) {
    auto adapter = physical_devices_[i];
    auto& prop = physical_device_properties_[i];

    table_.vkGetPhysicalDeviceProperties(adapter, &prop.properties);
    table_.vkGetPhysicalDeviceFeatures(adapter, &prop.features);

    if (!QueryDeviceExtensions(i)) return false;
    QueryDeviceQueueFamilies(i);
    if (!QueryDeviceSurfaceCapabilities(i)) return false;
    if (!QueryDeviceSurfaceFormats(i)) return false;
    if (!QueryDevicePresentModes(i)) return false;
  }
  return true;
}

// todo: implement adapter selection logic
bool App::SelectPhysicalDevice() {
  physical_device_id_ = 0;
  physical_device_ = physical_devices_[physical_device_id_];
  return true;
}

bool App::SelectDeviceExtensions() {
  auto available = physical_device_properties_[physical_device_id_].extensions;
  auto has_swapchain = SelectExtensionByName("VK_KHR_swapchain", available,
                                             selected_device_extensions_);
  if (!has_swapchain) {
    LOG(Error, "cannot find extension VK_KHR_swapchain");
    return false;
  }
  auto has_portability_subset_ = SelectExtensionByName(
      "VK_KHR_portability_subset", available, selected_device_extensions_);
  auto has_synchronization2 = SelectExtensionByName(
      "VK_KHR_synchronization2", available, selected_device_extensions_);
  if (!has_synchronization2) {
    LOG(Error, "cannot find extension VK_KHR_synchronization2");
    return false;
  }
  return true;
}

// todo: how to select queues?
bool App::SelectQueues() {
  auto const& families =
      physical_device_properties_[physical_device_id_].queue_families;
  // find one queue family that supports all of graphics, compute, transfer, and
  // present.
  auto expected =
      VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
  for (int i = 0; i < families.size(); i++) {
    auto const& prop = families[i];
    bool valid = (prop.queueFlags & expected) != 0;
    VkBool32 present = VK_FALSE;
    table_.vkGetPhysicalDeviceSurfaceSupportKHR(physical_device_, i, surface_,
                                                &present);
    if (valid && present) {
      queue_family_id_ = i;
      return true;
    }
  }
  return false;
}

bool App::SelectDeviceFeatures() { return true; }

static const float QueuePriorities[] = {1.0, 1.0, 1.0, 1.0};

bool App::CreateDevice() {
  VkPhysicalDeviceSynchronization2Features synchronization2;
  synchronization2.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
  synchronization2.pNext = nullptr;
  synchronization2.synchronization2 = VK_TRUE;

  VkDeviceQueueCreateInfo queue_info;
  queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_info.pNext = nullptr;
  queue_info.flags = 0;
  queue_info.queueFamilyIndex = queue_family_id_;
  queue_info.queueCount = 1;
  queue_info.pQueuePriorities = QueuePriorities;

  VkDeviceCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  info.pNext = &synchronization2;
  info.flags = 0;
  info.queueCreateInfoCount = 1;
  info.pQueueCreateInfos = &queue_info;
  info.enabledLayerCount = 0;
  info.ppEnabledLayerNames = nullptr;
  info.enabledExtensionCount = selected_device_extensions_.size();
  info.ppEnabledExtensionNames = selected_device_extensions_.data();
  info.pEnabledFeatures = &device_features_;

  auto result =
      table_.vkCreateDevice(physical_device_, &info, nullptr, &device_);
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot create device");
    return false;
  }
  return true;
}

bool App::LoadDeviceProcs() {
  auto vkGetDeviceProcAddr = table_.vkGetDeviceProcAddr;
#define VULKAN_DEVICE_PROC(name)                                         \
  table_.name =                                                          \
      reinterpret_cast<PFN_##name>(vkGetDeviceProcAddr(device_, #name)); \
  if (!table_.name) {                                                    \
    Log(Error, "cannot load " #name);                                    \
    return false;                                                        \
  }
#include "samples/vulkan_triangle/vulkan_procs.inc"
#undef VULKAN_DEVICE_PROC
  return true;
}

void App::GetQueue() {
  table_.vkGetDeviceQueue(device_, queue_family_id_, 0, &queue_);
}

void App::SelectSurfaceFormat() {
  auto const& formats =
      physical_device_properties_[physical_device_id_].surface_formats;
  for (auto const& format : formats) {
    if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
        format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      surface_format_ = format;
      return;
    }
  }
  surface_format_ = formats[0];
}

bool App::CreateSwapchain() {
  auto const& prop = physical_device_properties_[physical_device_id_];

  auto min_image_count = prop.surface_capabilities.minImageCount + 1;
  if (prop.surface_capabilities.maxImageCount > 0 &&
      min_image_count > prop.surface_capabilities.maxImageCount) {
    min_image_count = prop.surface_capabilities.maxImageCount;
  }

  VkExtent2D image_extent;
  image_extent.width = delegate_->GetWidth();
  image_extent.height = delegate_->GetHeight();

  VkSwapchainCreateInfoKHR info;
  info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  info.pNext = nullptr;
  info.flags = 0;
  info.surface = surface_;
  info.minImageCount = min_image_count;
  info.imageFormat = surface_format_.format;
  info.imageColorSpace = surface_format_.colorSpace;
  info.imageExtent = image_extent;
  info.imageArrayLayers = 1;
  info.imageUsage =
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  info.queueFamilyIndexCount = 0;
  info.pQueueFamilyIndices = nullptr;
  info.preTransform = prop.surface_capabilities.currentTransform;
  info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
  info.clipped = VK_TRUE;
  info.oldSwapchain = nullptr;
  auto result =
      table_.vkCreateSwapchainKHR(device_, &info, nullptr, &swapchain_);
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot create swapchain");
    return false;
  }
  return true;
}

bool App::GetSwapchainImages() {
  uint32_t count;
  auto result =
      table_.vkGetSwapchainImagesKHR(device_, swapchain_, &count, nullptr);
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot get swapchain images");
    return false;
  }
  swapchain_images_.resize(count);
  result = table_.vkGetSwapchainImagesKHR(device_, swapchain_, &count,
                                          swapchain_images_.data());
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot get swapchain images");
    return false;
  }
  swapchain_images_.resize(count);
  return true;
}

bool App::CreateSwapchainImageViews() {
  swapchain_image_views_.resize(swapchain_images_.size());
  VkImageViewCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  info.format = surface_format_.format;
  info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  info.subresourceRange.baseMipLevel = 0;
  info.subresourceRange.levelCount = 1;
  info.subresourceRange.baseArrayLayer = 0;
  info.subresourceRange.layerCount = 1;
  for (int i = 0; i < swapchain_images_.size(); i++) {
    info.image = swapchain_images_[i];
    auto result = table_.vkCreateImageView(device_, &info, nullptr,
                                           swapchain_image_views_.data() + i);
    if (result != VK_SUCCESS) {
      LOG(Error, "cannot create swapchain image views");
      return false;
    }
  }
  return true;
}

bool App::CreateCommandPool() {
  VkCommandPoolCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  info.queueFamilyIndex = queue_family_id_;
  auto result =
      table_.vkCreateCommandPool(device_, &info, nullptr, &command_pool_);
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot create command pool");
    return false;
  }
  return true;
}

bool App::CreateCommandBuffers() {
  VkCommandBufferAllocateInfo info;
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  info.pNext = nullptr;
  info.commandPool = command_pool_;
  info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  info.commandBufferCount = NumFramesInFlight;
  auto result =
      table_.vkAllocateCommandBuffers(device_, &info, frames_.command_buffer);
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot allocate command buffers");
    return false;
  }
  return true;
}

bool App::CreateFence(VkFlags flags, VkFence* fence) {
  VkFenceCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = flags;
  auto result = table_.vkCreateFence(device_, &info, nullptr, fence);
  if (result != VK_SUCCESS) {
    return false;
  }
  return true;
}

bool App::CreateSemaphore(VkFlags flags, VkSemaphore* semaphore) {
  VkSemaphoreCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = flags;
  auto result = table_.vkCreateSemaphore(device_, &info, nullptr, semaphore);
  if (result != VK_SUCCESS) {
    return false;
  }
  return true;
}

bool App::CreateRenderFences() {
  for (int i = 0; i < NumFramesInFlight; i++) {
    if (!CreateFence(VK_FENCE_CREATE_SIGNALED_BIT, frames_.render_fence + i)) {
      LOG(Error, "cannot create fences");
      return false;
    }
  }
  return true;
}

bool App::CreateSwapchainSemaphores() {
  for (int i = 0; i < NumFramesInFlight; i++) {
    if (!CreateSemaphore(0, frames_.swapchain_semaphore + i)) {
      LOG(Error, "cannot create semaphores");
      return false;
    }
  }
  return true;
}

bool App::CreateRenderSemaphores() {
  for (int i = 0; i < NumFramesInFlight; i++) {
    if (!CreateSemaphore(0, frames_.render_semaphore + i)) {
      LOG(Error, "cannot create semaphores");
      return false;
    }
  }
  return true;
}

VkBool32 App::DebugMessageCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    VkDebugUtilsMessengerCallbackDataEXT const* data, void* user_data) {
  // LOG(Info, "vulkan validation: ", data->pMessage);
  printf("validator: %s\n", data->pMessage);
  // Spec:
  // The application should always return VK_FALSE.
  return VK_FALSE;
}

void App::DestroyInstance() { table_.vkDestroyInstance(instance_, nullptr); }

void App::DestroyDebugMessenger() {
  table_.vkDestroyDebugUtilsMessengerEXT(instance_, debug_messenger_, nullptr);
}

void App::DestroySurface() {
  table_.vkDestroySurfaceKHR(instance_, surface_, nullptr);
}

void App::DestroyDevice() { table_.vkDestroyDevice(device_, nullptr); }

void App::DestroySwapchain() {
  table_.vkDestroySwapchainKHR(device_, swapchain_, nullptr);
}

void App::DestroySwapchainImageViews() {
  for (auto image_view : swapchain_image_views_) {
    table_.vkDestroyImageView(device_, image_view, nullptr);
  }
}

void App::DestroyCommandPool() {
  table_.vkDestroyCommandPool(device_, command_pool_, nullptr);
}

void App::DestroyRenderFences() {
  for (int i = 0; i < NumFramesInFlight; i++) {
    table_.vkDestroyFence(device_, frames_.render_fence[i], nullptr);
  }
}
void App::DestroySwapchainSemaphores() {
  for (int i = 0; i < NumFramesInFlight; i++) {
    table_.vkDestroySemaphore(device_, frames_.swapchain_semaphore[i], nullptr);
  }
}
void App::DestroyRenderSemaphores() {
  for (int i = 0; i < NumFramesInFlight; i++) {
    table_.vkDestroySemaphore(device_, frames_.render_semaphore[i], nullptr);
  }
}

// TODO: delegate deinit
void App::Deinit() {
  DestroyRenderSemaphores();
  DestroySwapchainSemaphores();
  DestroyRenderFences();
  DestroyCommandPool();
  DestroySwapchainImageViews();
  DestroySwapchain();
  DestroyDevice();
  DestroySurface();
  if (has_debug_utils_) DestroyDebugMessenger();
  DestroyInstance();
  delegate_->Deinit();
}

bool App::WaitAndResetFence() {
  VkResult result;
  result = table_.vkWaitForFences(
      device_, 1, frames_.render_fence + current_frame_, VK_TRUE, UINT64_MAX);
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot wait for fence");
    return false;
  }
  result =
      table_.vkResetFences(device_, 1, frames_.render_fence + current_frame_);
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot reset fence");
    return false;
  }
  return true;
}

bool App::AcquireNextImage(uint32_t* id) {
  auto result = table_.vkAcquireNextImageKHR(
      device_, swapchain_, UINT64_MAX,
      frames_.swapchain_semaphore[current_frame_], nullptr, id);
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot acquire next swapchain image");
    return false;
  }
  return true;
}

bool App::BeginCommandBuffer() {
  auto buffer = frames_.command_buffer[current_frame_];
  auto result = table_.vkResetCommandBuffer(buffer, 0);
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot reset command buffer");
    return false;
  }
  VkCommandBufferBeginInfo info;
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  info.pNext = nullptr;
  info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  info.pInheritanceInfo = nullptr;
  result = table_.vkBeginCommandBuffer(buffer, &info);
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot begin command buffer");
    return false;
  }
  return true;
}

bool App::EndCommandBuffer() {
  auto result =
      table_.vkEndCommandBuffer(frames_.command_buffer[current_frame_]);
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot end command buffer");
    return false;
  }
  return true;
}

bool App::TransitionImage(VkCommandBuffer cmd, VkImage image,
                          VkImageLayout cur_layout, VkImageLayout new_layout) {
  VkImageMemoryBarrier2 barrier;
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
  barrier.pNext = nullptr;
  barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
  barrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
  barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
  barrier.dstAccessMask =
      VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
  barrier.oldLayout = cur_layout;
  barrier.newLayout = new_layout;
  barrier.srcQueueFamilyIndex = 0;
  barrier.dstQueueFamilyIndex = 0;
  barrier.image = image;
  barrier.subresourceRange.aspectMask =
      (new_layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
          ? VK_IMAGE_ASPECT_DEPTH_BIT
          : VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  VkDependencyInfo dep;
  dep.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
  dep.pNext = nullptr;
  dep.dependencyFlags = 0;
  dep.memoryBarrierCount = 0;
  dep.pMemoryBarriers = nullptr;
  dep.bufferMemoryBarrierCount = 0;
  dep.pBufferMemoryBarriers = nullptr;
  dep.imageMemoryBarrierCount = 1;
  dep.pImageMemoryBarriers = &barrier;

  table_.vkCmdPipelineBarrier2KHR(cmd, &dep);

  return true;
}

bool App::Submit() {
  auto cmd = frames_.command_buffer[current_frame_];

  VkCommandBufferSubmitInfo cmdbuf_info;
  cmdbuf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
  cmdbuf_info.pNext = nullptr;
  cmdbuf_info.commandBuffer = cmd;
  cmdbuf_info.deviceMask = 0;

  VkSemaphoreSubmitInfo wait_info;
  wait_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
  wait_info.pNext = nullptr;
  wait_info.semaphore = frames_.swapchain_semaphore[current_frame_];
  wait_info.value = 1;
  wait_info.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR;
  wait_info.deviceIndex = 0;

  VkSemaphoreSubmitInfo signal_info;
  signal_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
  signal_info.pNext = nullptr;
  signal_info.semaphore = frames_.render_semaphore[current_frame_];
  signal_info.value = 1;
  signal_info.stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
  signal_info.deviceIndex = 0;

  VkSubmitInfo2 info;
  info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
  info.pNext = nullptr;
  info.waitSemaphoreInfoCount = 1;
  info.pWaitSemaphoreInfos = &wait_info;
  info.signalSemaphoreInfoCount = 1;
  info.pSignalSemaphoreInfos = &signal_info;
  info.commandBufferInfoCount = 1;
  info.pCommandBufferInfos = &cmdbuf_info;

  auto result = table_.vkQueueSubmit2KHR(queue_, 1, &info,
                                         frames_.render_fence[current_frame_]);
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot submit command buffer");
    return false;
  }
  return true;
}

bool App::Present(uint32_t id) {
  VkPresentInfoKHR info = {};
  info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  info.pNext = nullptr;
  info.waitSemaphoreCount = 1;
  info.pWaitSemaphores = frames_.render_semaphore + current_frame_;
  info.swapchainCount = 1;
  info.pSwapchains = &swapchain_;
  info.pImageIndices = &id;
  info.pResults = nullptr;
  auto result = table_.vkQueuePresentKHR(queue_, &info);
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot present");
    return false;
  }
  return true;
}

void App::Run() {
  uint32_t n = 0;
  while (!delegate_->ShouldQuit()) {
    if (!WaitAndResetFence()) return;

    uint32_t swapchain_image_id;
    if (!AcquireNextImage(&swapchain_image_id)) return;
    if (!BeginCommandBuffer()) return;

    auto cmd = frames_.command_buffer[current_frame_];
    auto image = swapchain_images_[swapchain_image_id];

    if (!TransitionImage(cmd, image, VK_IMAGE_LAYOUT_UNDEFINED,
                         VK_IMAGE_LAYOUT_GENERAL))
      return;

    float flash = abs(sin(n / 120.f));
    VkClearColorValue clearValue = {{abs(sin(n / 120.f + 72)), abs(sin(n / 120.f + 36)), flash, 1.0f}};

    VkImageSubresourceRange clear_range;
    clear_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    clear_range.baseMipLevel = 0;
    clear_range.levelCount = 1;
    clear_range.baseArrayLayer = 0;
    clear_range.layerCount = 1;

    table_.vkCmdClearColorImage(cmd, image, VK_IMAGE_LAYOUT_GENERAL,
                                &clearValue, 1, &clear_range);

    if (!TransitionImage(cmd, image, VK_IMAGE_LAYOUT_GENERAL,
                         VK_IMAGE_LAYOUT_PRESENT_SRC_KHR))
      return;

    if (!EndCommandBuffer()) return;
    if (!Submit()) return;
    if (!Present(swapchain_image_id)) return;

    current_frame_ = (current_frame_ + 1) % NumFramesInFlight;
    n++;
    delegate_->PollEvents();
  }
  table_.vkDeviceWaitIdle(device_);
}
