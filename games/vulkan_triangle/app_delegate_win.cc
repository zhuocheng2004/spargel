#include "samples/vulkan_triangle/app_delegate_win.h"

#include "samples/vulkan_triangle/logging.h"
#include "samples/vulkan_triangle/utils.h"
#include "vulkan/vulkan_win32.h"

using base::logging::Log;
using enum base::logging::LogLevel;

AppDelegateWin::AppDelegateWin() { DoCreateWindow(); }
AppDelegateWin::~AppDelegateWin() {}

bool AppDelegateWin::LoadVkLibrary() {
  module_ = ::LoadLibraryA("vulkan-1.dll");

  if (!module_) {
    Log(Error, "cannot load libvulkan.dylib");
    return false;
  }
  return true;
}

bool AppDelegateWin::LoadVkGetInstanceProcAddr() {
  table_->vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(
      GetProcAddress(module_, "vkGetInstanceProcAddr"));
  if (!table_->vkGetInstanceProcAddr) {
    Log(Error, "cannot load vkGetInstanceProcAddr");
    return false;
  }
  return true;
}

bool AppDelegateWin::SelectInstanceLayers(
    std::vector<VkLayerProperties> const& available,
    std::vector<char const*>& selected) {
  return true;
}

bool AppDelegateWin::SelectInstanceExtensions(
    std::vector<VkExtensionProperties> const& available,
    std::vector<char const*>& selected) {
  auto has_surface =
      SelectExtensionByName("VK_KHR_win32_surface", available, selected);
  if (!has_surface) {
    Log(Error, "cannot find extension VK_KHR_win32_surface");
    return false;
  }
  return true;
}

bool AppDelegateWin::SelectDeviceExtensions(
    std::vector<VkExtensionProperties> const& available,
    std::vector<char const*>& selected) {
  return true;
}

bool AppDelegateWin::CreateSurface(VkInstance instance, VkSurfaceKHR* surface,
                                   ProcTable const& table) {
  VkWin32SurfaceCreateInfoKHR info;
  info.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
  info.pNext = nullptr;
  info.flags = 0;
  info.hinstance = GetModuleHandle(NULL);
  info.hwnd = hwnd_;
  auto result =
      table.vkCreateWin32SurfaceKHR(instance, &info, nullptr, surface);
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot create win32 surface");
    return false;
  }
  return true;
}

void AppDelegateWin::Deinit() { FreeLibrary(module_); }

void AppDelegateWin::SetShouldQuit(bool v) { should_quit_ = v; }

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                            LPARAM lParam) {
  switch (uMsg) {
    case WM_DESTROY:
      PostQuitMessage(1);
    default:
      break;
  }
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

bool AppDelegateWin::DoCreateWindow() {
  const char CLASS_NAME[] = "Spargel Class";

  WNDCLASS wc = {};

  wc.lpfnWndProc = WindowProc;
  wc.hInstance = GetModuleHandle(NULL);
  ;
  wc.lpszClassName = CLASS_NAME;

  RegisterClass(&wc);

  hwnd_ = CreateWindowExA(0, CLASS_NAME, "Spargel Engine", WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT, 500, 500, NULL, NULL,
                          GetModuleHandle(NULL), NULL);
  if (hwnd_ == NULL) {
    return 0;
  }

  ShowWindow(hwnd_, 1);

  return true;
}

uint32_t AppDelegateWin::GetWidth() { return 500; }
uint32_t AppDelegateWin::GetHeight() { return 500; }

void AppDelegateWin::PollEvents() {
  MSG msg = {};
  while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
    if (msg.message == WM_QUIT) {
      should_quit_ = true;
    } else {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
    }
  }
}

bool AppDelegateWin::ShouldQuit() { return should_quit_; }
