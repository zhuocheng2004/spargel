#if defined(VULKAN_LIBRARY_PROC)

VULKAN_LIBRARY_PROC(vkGetInstanceProcAddr)

#endif

#if defined(VULKAN_GENERAL_PROC)

VULKAN_GENERAL_PROC(vkEnumerateInstanceLayerProperties)
VULKAN_GENERAL_PROC(vkEnumerateInstanceExtensionProperties)
VULKAN_GENERAL_PROC(vkCreateInstance)

#endif

#if defined(VULKAN_INSTANCE_PROC)

VULKAN_INSTANCE_PROC(vkDestroyInstance)

#endif

#if defined(VULKAN_DEVICE_PROC)

VULKAN_DEVICE_PROC(vkDestroyDevice)

#endif
