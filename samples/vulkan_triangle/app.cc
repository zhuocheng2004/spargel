#include "samples/vulkan_triangle/app.h"

#include "base/logging/logging.h"

using base::logging::Log;
using enum base::logging::LogLevel;

App::App(AppDelegate* delegate) : delegate_{delegate} {
  delegate_->SetProcTable(&table_);
}

bool App::Init() {
  if (!delegate_->LoadLibrary()) return false;
  if (!delegate_->LoadVkGetInstanceProcAddr()) return false;
  if (!CreateInstance()) return false;
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
  info.pNext = nullptr; // TODO: fix this
  info.flags = 0;
  info.pApplicationInfo = &app_info;
  info.enabledLayerCount = 0;
  info.ppEnabledLayerNames = nullptr;
  info.enabledExtensionCount = 0;
  info.ppEnabledExtensionNames = nullptr;
  Log(Error, "cannot create instance");
  return false;
}

void App::Deinit() { delegate_->Deinit(); }

void App::Run() {}
