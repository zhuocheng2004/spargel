#include "samples/vulkan_triangle/app.h"

#include <stdio.h>

#include "base/logging/logging.h"
#include "samples/vulkan_triangle/utils.h"

using base::logging::Log;
using enum base::logging::LogLevel;

App::App(AppDelegate* delegate) : delegate_{delegate} {
  delegate_->SetProcTable(&table_);
}

bool App::Init() {
  if (!delegate_->LoadLibrary()) return false;
  if (!delegate_->LoadVkGetInstanceProcAddr()) return false;
  if (!LoadGeneralProcs()) return false;
  if (!EnumerateInstanceLayers()) return false;
  PrintInstanceLayers();
  if (!EnumerateInstanceExtensions()) return false;
  PrintInstanceExtensions();
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
  Log(Info, "instance created");
  if (!LoadInstanceProcs()) return false;
  return true;
}

bool App::LoadGeneralProcs() {
  auto vkGetInstanceProcAddr = table_.vkGetInstanceProcAddr;
#define VULKAN_GENERAL_PROC(name)                                          \
  table_.name =                                                            \
      reinterpret_cast<PFN_##name>(vkGetInstanceProcAddr(nullptr, #name)); \
  if (!table_.name) {                                                      \
    Log(Error, "cannot load " #name);                                      \
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
    Log(Error, "cannot enumerate instance layers");
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
    Log(Error, "cannot enumerate instance extensions");
    return false;
  }
  exts.resize(count);
  result =
      table_.vkEnumerateInstanceExtensionProperties(layer, &count, exts.data());
  if (result != VK_SUCCESS) {
    Log(Error, "cannot enumerate instance extensions");
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
  has_api_dump_ = SelectLayerByName(
      "VK_LAYER_LUNARG_api_dump", instance_layers_, selected_instance_layers_);
  return true;
}

void App::PrintSelectedInstanceLayers() {
  printf("list of selected instance layers (%zu in total)\n",
         selected_instance_layers_.size());
  for (auto name : selected_instance_layers_) {
    printf("  %s\n", name);
  }
  printf("\n");
}

void App::PrintSelectedInstanceExtensions() {
  printf("list of selected instance extensions (%zu in total)\n",
         selected_instance_extensions_.size());
  for (auto name : selected_instance_extensions_) {
    printf("  %s\n", name);
  }
  printf("\n");
}

bool App::SelectInstanceExtensions() {
  has_debug_utils_ =
      SelectExtensionByName("VK_EXT_debug_utils", instance_extensions_,
                            selected_instance_extensions_);
  auto has_surface = SelectExtensionByName(
      "VK_KHR_surface", instance_extensions_, selected_instance_extensions_);
  if (!has_surface) {
    Log(Error, "cannot find extension VK_KHR_surface");
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
    Log(Error, "cannot create instance");
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

void App::DestroyInstance() { table_.vkDestroyInstance(instance_, nullptr); }

// TODO: delegate deinit
void App::Deinit() {
  DestroyInstance();
  delegate_->Deinit();
}

void App::Run() {}
