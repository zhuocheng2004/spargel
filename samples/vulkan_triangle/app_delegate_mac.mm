#include "samples/vulkan_triangle/app_delegate_mac.h"

#include <dlfcn.h>

#include <Carbon/Carbon.h>

#import <AppKit/NSWindow.h>
#import <QuartzCore/CAMetalLayer.h>

#include "base/logging/logging.h"
#include "samples/vulkan_triangle/utils.h"

using base::logging::Log;
using enum base::logging::LogLevel;

@interface CustomAppDelegate : NSObject <NSApplicationDelegate>
@end
@implementation CustomAppDelegate
- (void)applicationDidFinishLaunching:(NSNotification*)notification {
  [NSApp stop:nil];
}
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender {
  return YES;
}
@end

@interface WindowDelegate : NSObject <NSWindowDelegate> {
  AppDelegateMac* mac_app;
}
- (instancetype)initWithAppDelegateMac:(AppDelegateMac*)delegate;
@end
@implementation WindowDelegate
- (instancetype)initWithAppDelegateMac:(AppDelegateMac*)delegate {
  self = [super init];
  mac_app = delegate;
  return self;
}
- (void)windowWillClose:(NSNotification*)notification {
  mac_app->SetShouldQuit(true);
}
@end

struct AppDelegateMac::ObjcData {
  CustomAppDelegate* delegate;
  NSWindow* window;
  WindowDelegate* window_delegate;
  NSView* view;
  CAMetalLayer* layer;
};

AppDelegateMac::AppDelegateMac() {
  data_ = new ObjcData;
  [NSApplication sharedApplication];
  [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
  data_->delegate = [[CustomAppDelegate alloc] init];
  [NSApp setDelegate:data_->delegate];
  CreateWindow();
  [NSApp run];
}
AppDelegateMac::~AppDelegateMac() { delete data_; }

bool AppDelegateMac::LoadLibrary() {
  // library_ = dlopen("libMoltenVK.dylib", RTLD_NOW | RTLD_LOCAL);
  library_ = dlopen("libvulkan.dylib", RTLD_NOW | RTLD_LOCAL);
  if (!library_) {
    Log(Error, "cannot load libvulkan.dylib");
    return false;
  }
  return true;
}

bool AppDelegateMac::LoadVkGetInstanceProcAddr() {
  table_->vkGetInstanceProcAddr =
      reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(library_, "vkGetInstanceProcAddr"));
  if (!table_->vkGetInstanceProcAddr) {
    Log(Error, "cannot load vkGetInstanceProcAddr");
    return false;
  }
  return true;
}

bool AppDelegateMac::SelectInstanceLayers(std::vector<VkLayerProperties> const& available,
                                          std::vector<char const*>& selected) {
  return true;
}

bool AppDelegateMac::SelectInstanceExtensions(std::vector<VkExtensionProperties> const& available,
                                              std::vector<char const*>& selected) {
  auto has_surface = SelectExtensionByName("VK_EXT_metal_surface", available, selected);
  if (!has_surface) {
    Log(Error, "cannot find extension VK_EXT_metal_surface");
    return false;
  }
  return true;
}

bool AppDelegateMac::SelectDeviceExtensions(std::vector<VkExtensionProperties> const& available,
                                            std::vector<char const*>& selected) {
  return true;
}

bool AppDelegateMac::CreateSurface(VkInstance instance, VkSurfaceKHR* surface,
                                   ProcTable const& table) {
  VkMetalSurfaceCreateInfoEXT info;
  info.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
  info.pNext = nullptr;
  info.flags = 0;
  info.pLayer = data_->layer;
  auto result = table.vkCreateMetalSurfaceEXT(instance, &info, nullptr, surface);
  if (result != VK_SUCCESS) {
    LOG(Error, "cannot create metal surface");
    return false;
  }
  return true;
}

void AppDelegateMac::Deinit() { dlclose(library_); }

@interface AppContentView : NSView {
  AppDelegateMac* mac_app;
}
- (instancetype)initWithAppDelegateMac:(AppDelegateMac*)delegate;
@end
@implementation AppContentView
- (instancetype)initWithAppDelegateMac:(AppDelegateMac*)delegate {
  self = [super init];
  mac_app = delegate;
  return self;
}
- (BOOL)wantsUpdateLayer {
  return YES;
}
- (void)dealloc {
  [super dealloc];
}
- (void)keyDown:(NSEvent*)event {
  LOG(Info, "keyDown");
  auto keyCode = [event keyCode];
  if (keyCode == kVK_Escape) {
    LOG(Info, "esc pressed");
    mac_app->SetShouldQuit(true);
  }
  [self interpretKeyEvents:@[ event ]];
}
- (BOOL)canBecomeKeyView {
  return YES;
}

- (BOOL)acceptsFirstResponder {
  return YES;
}
@end

void AppDelegateMac::SetShouldQuit(bool v) { should_quit_ = v; }

bool AppDelegateMac::CreateWindow() {
  auto style = NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable |
               NSWindowStyleMaskResizable | NSWindowStyleMaskTitled;
  auto rect = NSMakeRect(0, 0, 500, 500);
  data_->window = [[NSWindow alloc] initWithContentRect:rect
                                              styleMask:style
                                                backing:NSBackingStoreBuffered
                                                  defer:NO];
  if (!data_->window) {
    LOG(Error, "cannot create window");
    return false;
  }

  data_->view = [[AppContentView alloc] initWithAppDelegateMac:this];
  if (!data_->view) {
    LOG(Error, "cannot create view");
    return false;
  }

  data_->layer = [CAMetalLayer layer];
  if (!data_->layer) {
    LOG(Error, "cannot create layer");
    return false;
  }
  [data_->view setWantsLayer:YES];
  [data_->view setLayer:data_->layer];

  [data_->window setContentView:data_->view];
  [data_->window setTitle:[NSString stringWithUTF8String:"Spargel Engine"]];
  [data_->window makeKeyAndOrderFront:NSApp];

  data_->window_delegate = [[WindowDelegate alloc] initWithAppDelegateMac:this];
  [data_->window setDelegate:data_->window_delegate];
  return true;
}

uint32_t AppDelegateMac::GetWidth() { return data_->view.bounds.size.width; }
uint32_t AppDelegateMac::GetHeight() { return data_->view.bounds.size.height; }

void AppDelegateMac::PollEvents() {
  for (;;) {
    @autoreleasepool {
      NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                          untilDate:[NSDate distantPast]
                                             inMode:NSDefaultRunLoopMode
                                            dequeue:YES];
      if (event == nil) break;
      [NSApp sendEvent:event];
    }
  }
}

bool AppDelegateMac::ShouldQuit() { return should_quit_; }
