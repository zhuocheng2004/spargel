# Vulkan

Important:
- Use `gpu/vulkan/vulkan_headers.h` whenever you want to use the raw vulkan headers.

- Define `VK_NO_PROTOTYPES` before including `vulkan/vulkan.h`!
- Do not use `vulkan/vulkan.h` on Windows!
  Avoid the heavy `windows.h`!
  Include `vulkan/vk_platform.h`, `vulkan/vulkan_core.h`,
  `vulkan/vulkan_win32.h`, and provide a few more typedefs.
