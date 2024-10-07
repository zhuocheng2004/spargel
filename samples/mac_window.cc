#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

using objc_Class = struct objc_class*;
using objc_id = struct objc_object*;
using objc_SEL = struct objc_selector*;

struct NSRect {
  double x1;
  double y1;
  double x2;
  double y2;
};

extern "C" objc_id objc_getClass(char const* name);
extern "C" objc_SEL sel_registerName(char const* str);
extern "C" void objc_msgSend();

// the objective c runtime is automatically available once some framework is linked
// it cannot be loaded using dlopen
//
// tons of undefined behaviors
// c.f. [7.6.1.10/6,8]
struct ObjCRuntime {
  auto GetClass(char const* name) noexcept -> objc_id {
    return objc_getClass(name);
  }
  auto RegisterName(char const* str) noexcept -> objc_SEL {
    return sel_registerName(str);
  }
  template<typename R, typename... Args>
  auto SendMessage(objc_id obj, objc_SEL op, Args... args) noexcept -> R {
    return reinterpret_cast<R(*)(objc_id, objc_SEL, Args...)>(objc_msgSend)(obj, op, args...);
  }
};

auto main() -> int {
  ObjCRuntime objc;

  auto NSApplication = objc.GetClass("NSApplication");
  auto NSWindow = objc.GetClass("NSWindow");
  auto NSString = objc.GetClass("NSString");

  auto sharedApplication = objc.RegisterName("sharedApplication");
  auto setActivationPolicy = objc.RegisterName("setActivationPolicy:");
  auto alloc = objc.RegisterName("alloc");
  auto initWithContentRect = objc.RegisterName("initWithContentRect:styleMask:backing:defer:");
  auto stringWithUTF8String = objc.RegisterName("stringWithUTF8String:");
  auto setTitle = objc.RegisterName("setTitle:");
  auto makeKeyAndOrderFront = objc.RegisterName("makeKeyAndOrderFront:");
  auto run = objc.RegisterName("run");

  auto app = objc.SendMessage<objc_id>(NSApplication, sharedApplication);
  auto result = objc.SendMessage<bool>(app, setActivationPolicy, uint64_t{0});
  auto window = objc.SendMessage<objc_id>(NSWindow, alloc);
  window = objc.SendMessage<objc_id>(window, initWithContentRect,
    NSRect {0, 0, 300, 300}, uint64_t{0b1011}, uint64_t{2}, false);
  auto title = objc.SendMessage<objc_id>(NSString, stringWithUTF8String, "Spargel Engine");
  objc.SendMessage<void>(window, setTitle, title);
  objc.SendMessage<void>(window, makeKeyAndOrderFront, objc_id{nullptr});
  objc.SendMessage<void>(app, run);
  return 0;
}
