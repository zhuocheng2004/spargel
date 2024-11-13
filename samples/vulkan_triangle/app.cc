#include "samples/vulkan_triangle/app.h"

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

bool App::QueryPhysicalDeviceProperties() {
  physical_device_properties_.resize(physical_devices_.size());

  for (int i = 0; i < physical_devices_.size(); i++) {
    auto adapter = physical_devices_[i];
    auto& prop = physical_device_properties_[i];

    table_.vkGetPhysicalDeviceProperties(adapter, &prop.properties);
    table_.vkGetPhysicalDeviceFeatures(adapter, &prop.features);

    if (!QueryDeviceExtensions(i)) return false;
    QueryDeviceQueueFamilies(i);
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
  VkDeviceQueueCreateInfo queue_info;
  queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_info.pNext = nullptr;
  queue_info.flags = 0;
  queue_info.queueFamilyIndex = queue_family_id_;
  queue_info.queueCount = 1;
  queue_info.pQueuePriorities = QueuePriorities;

  VkDeviceCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  info.pNext = nullptr;
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

// TODO: delegate deinit
void App::Deinit() {
  DestroyDevice();
  DestroySurface();
  if (has_debug_utils_) DestroyDebugMessenger();
  DestroyInstance();
  delegate_->Deinit();
}

void App::Run() {}
