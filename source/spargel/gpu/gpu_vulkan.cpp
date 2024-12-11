#include <spargel/base/base.h>
#include <spargel/config.h>
#include <spargel/gpu/gpu_vulkan.h>
#include <spargel/ui/ui.h>

/* libc */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* platform */
#if SPARGEL_IS_LINUX || SPARGEL_IS_MACOS
#include <dlfcn.h>
#endif

#define VK_NO_PROTOTYPES
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#if SPARGEL_IS_MACOS
#include <vulkan/vulkan_metal.h>
#endif

#if SPARGEL_IS_LINUX
#include <xcb/xcb.h>
/* after xcb */
#include <vulkan/vulkan_wayland.h>
#include <vulkan/vulkan_xcb.h>
#endif

#if SPARGEL_IS_LINUX
#define VULKAN_LIB_FILENAME "libvulkan.so.1"
#elif SPARGEL_IS_MACOS
#define VULKAN_LIB_FILENAME "libvulkan.dylib"
#elif SPARGEL_IS_WINDOWS
#define VULKAN_LIB_FILENAME "vulkan-1.dll"
#endif

#define alloc_object(type, name)                                                                 \
    struct type* name = (struct type*)sbase_allocate(sizeof(struct type), SBASE_ALLOCATION_GPU); \
    if (!name) sbase_panic_here();                                                               \
    memset(name, 0, sizeof(struct type));

#define cast_object(type, name, object) struct type* name = (struct type*)(object);

#define dealloc_object(type, name) \
    sbase_deallocate(name, sizeof(struct type), SBASE_ALLOCATION_GPU);

struct sgpu_vulkan_proc_table {
#define VULKAN_PROC_DECL(name) PFN_##name name;
#define VULKAN_LIBRARY_PROC(name) VULKAN_PROC_DECL(name)
#define VULKAN_GENERAL_PROC(name) VULKAN_PROC_DECL(name)
#define VULKAN_INSTANCE_PROC(name) VULKAN_PROC_DECL(name)
#define VULKAN_DEVICE_PROC(name) VULKAN_PROC_DECL(name)
#include <spargel/gpu/vulkan_procs.inc>
#undef VULKAN_PROC_DECL
};

struct sgpu_vulkan_command_queue {
    int backend;
    VkQueue queue;
};

struct sgpu_vulkan_surface {
    VkSurfaceKHR surface;
};

struct sgpu_vulkan_presentable {
    u32 index;
    struct sgpu_vulkan_swapchain* swapchain;
};

struct sgpu_vulkan_swapchain {
    VkSwapchainKHR swapchain;
    int width;
    int height;
    ssize image_count;
    ssize image_capacity;
    /* todo: fix this */
    VkRenderPass render_pass;
    VkImage* images;
    VkImageView* image_views;
    VkFramebuffer* framebuffers;
    /* todo: fix this */
    struct sgpu_vulkan_presentable presentable;
    VkFence image_available;
    VkSemaphore render_complete;
    VkFence submit_done;
};

struct sgpu_vulkan_command_buffer {
    VkCommandBuffer command_buffer;
    VkQueue queue;
};

struct sgpu_vulkan_render_pass_encoder {
    VkCommandBuffer command_buffer;
};

struct sgpu_vulkan_device {
    int backend;
    void* library;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkPhysicalDevice physical_device;
    u32 queue_family;
    VkDevice device;
    struct sgpu_vulkan_command_queue queue;
    VkCommandPool cmd_pool;
    struct sgpu_vulkan_proc_table procs;
};

struct sgpu_vulkan_shader_function {
    int backend;
    VkShaderModule shader;
    struct sgpu_vulkan_device* device;
};

struct sgpu_vulkan_render_pipeline {
    int backend;
    VkPipeline pipeline;
    struct sgpu_vulkan_device* device;
};

#define CHECK_VK_RESULT(expr)                                          \
    do {                                                               \
        VkResult result = expr;                                        \
        if (result != VK_SUCCESS) {                                    \
            sbase_log_fatal(#expr " failed with result = %d", result); \
            sbase_panic_here();                                        \
        }                                                              \
    } while (0)

struct array {
    void* data;
    ssize count;
    ssize capacity;
    ssize stride;
};

static void init_array(struct array* a, ssize stride) {
    a->data = NULL;
    a->count = 0;
    a->capacity = 0;
    a->stride = stride;
}

static void deinit_array(struct array a) {
    if (a.data) {
        sbase_deallocate(a.data, a.stride * a.capacity, SBASE_ALLOCATION_GPU);
    }
}

static void array_reserve(struct array* a, ssize count) {
    if (count > a->capacity) {
        ssize new_cap = a->capacity * 2;
        new_cap = count > new_cap ? count : new_cap;
        new_cap = 8 > new_cap ? 8 : new_cap;
        void* new_data = sbase_reallocate(a->data, a->stride * a->capacity, a->stride * new_cap,
                                          SBASE_ALLOCATION_GPU);
        a->data = new_data;
        a->capacity = new_cap;
    }
}

static void* array_at(struct array a, ssize i) {
    spargel_assert(i >= 0 && i < a.count);
    return (char*)a.data + i * a.stride;
}

static void* array_push(struct array* a) {
    array_reserve(a, a->count + 1);
    ssize i = a->count;
    a->count++;
    return array_at(*a, i);
}

static VkBool32 sgpu_vulkan_debug_messenger_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type,
    VkDebugUtilsMessengerCallbackDataEXT const* data, void* user_data) {
    fprintf(stderr, "validator: %s\n", data->pMessage);
    return VK_FALSE;
}

static float const sgpu_vulkan_queue_priorities[64] = {
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
};

int sgpu_vulkan_create_default_device(struct sgpu_device_descriptor const* descriptor,
                                      sgpu_device_id* device) {
    alloc_object(sgpu_vulkan_device, d);
    d->backend = SGPU_BACKEND_VULKAN;
#if SPARGEL_IS_LINUX || SPARGEL_IS_MACOS
    d->library = dlopen(VULKAN_LIB_FILENAME, RTLD_NOW | RTLD_LOCAL);
#else
#error unimplemented
#endif
    if (d->library == NULL) {
        dealloc_object(sgpu_vulkan_device, d);
        return SGPU_RESULT_NO_BACKEND;
    }

    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr =
        (PFN_vkGetInstanceProcAddr)dlsym(d->library, "vkGetInstanceProcAddr");
    d->procs.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
#define VULKAN_GENERAL_PROC(name) d->procs.name = (PFN_##name)vkGetInstanceProcAddr(NULL, #name);
#include <spargel/gpu/vulkan_procs.inc>

    struct sgpu_vulkan_proc_table* procs = &d->procs;

    /* step 1. enumerate layers */
    struct array all_layers;
    init_array(&all_layers, sizeof(struct VkLayerProperties));
    {
        u32 count;
        CHECK_VK_RESULT(procs->vkEnumerateInstanceLayerProperties(&count, NULL));
        array_reserve(&all_layers, count);
        CHECK_VK_RESULT(
            procs->vkEnumerateInstanceLayerProperties(&count, (VkLayerProperties*)all_layers.data));
        all_layers.count = count;
    }

    for (ssize i = 0; i < all_layers.count; i++) {
        sbase_log_info("layer #%ld = %s", i,
                       ((struct VkLayerProperties*)array_at(all_layers, i))->layerName);
    }

    /* step 2. enumerate instance extensions */
    struct array all_exts;
    init_array(&all_exts, sizeof(struct VkExtensionProperties));
    {
        u32 count;
        CHECK_VK_RESULT(procs->vkEnumerateInstanceExtensionProperties(NULL, &count, NULL));
        array_reserve(&all_exts, count);
        CHECK_VK_RESULT(procs->vkEnumerateInstanceExtensionProperties(
            NULL, &count, (VkExtensionProperties*)all_exts.data));
        all_exts.count = count;
    }

    for (ssize i = 0; i < all_exts.count; i++) {
        sbase_log_info("instance extension #%ld = %s", i,
                       ((struct VkExtensionProperties*)array_at(all_exts, i))->extensionName);
    }

    /* step 3. select layers */
    /* we need VK_LAYER_KHRONOS_validation */
    struct array use_layers;
    init_array(&use_layers, sizeof(char const*));
    for (ssize i = 0; i < all_layers.count; i++) {
        if (strcmp(((struct VkLayerProperties*)array_at(all_layers, i))->layerName,
                   "VK_LAYER_KHRONOS_validation") == 0) {
            char const** ptr = (char const**)array_push(&use_layers);
            *ptr = "VK_LAYER_KHRONOS_validation";
            sbase_log_info("use layer VK_LAYER_KHRONOS_validation");
        }
    }

    /* step 4. select instance extensions */
    /**
     * VK_KHR_surface (required);
     * VK_KHR_portability_enumeration (if exists);
     * VK_EXT_debug_utils (debug);
     * VK_EXT_metal_surface (platform);
     */
    /**
     * VK_KHR_portability_enumeration
     *
     * This extension allows applications to control whether devices that expose the
     * VK_KHR_portability_subset extension are included in the results of physical device
     * enumeration. Since devices which support the VK_KHR_portability_subset extension are not
     * fully conformant Vulkan implementations, the Vulkan loader does not report those devices
     * unless the application explicitly asks for them. This prevents applications which may not be
     * aware of non-conformant devices from accidentally using them, as any device which supports
     * the VK_KHR_portability_subset extension mandates that the extension must be enabled if that
     * device is used.
     */
    struct array use_exts;
    init_array(&use_exts, sizeof(char const*));
    bool has_surface = false;
    bool has_portability = false;
    bool has_debug_utils = false;
#if SPARGEL_IS_MACOS
    bool has_metal_surface = false;
#endif
#if SPARGEL_IS_LINUX
    bool has_xcb_surface = false;
    bool has_wayland_surface = false;
#endif
    for (ssize i = 0; i < all_exts.count; i++) {
        char const* name = ((struct VkExtensionProperties*)array_at(all_exts, i))->extensionName;
        if (strcmp(name, "VK_KHR_surface") == 0) {
            char const** ptr = (char const**)array_push(&use_exts);
            *ptr = "VK_KHR_surface";
            has_surface = true;
            sbase_log_info("use instance extension VK_KHR_surface");
        } else if (strcmp(name, "VK_KHR_portability_enumeration") == 0) {
            char const** ptr = (char const**)array_push(&use_exts);
            *ptr = "VK_KHR_portability_enumeration";
            has_portability = true;
            sbase_log_info("use instance extension VK_KHR_portability_enumeration");
        } else if (strcmp(name, "VK_EXT_debug_utils") == 0) {
            char const** ptr = (char const**)array_push(&use_exts);
            *ptr = "VK_EXT_debug_utils";
            has_debug_utils = true;
            sbase_log_info("use instance extension VK_EXT_debug_utils");
        }
#if SPARGEL_IS_MACOS
        else if (strcmp(name, "VK_EXT_metal_surface") == 0) {
            char const** ptr = (char const**)array_push(&use_exts);
            *ptr = "VK_EXT_metal_surface";
            has_metal_surface = true;
            sbase_log_info("use instance extension VK_EXT_metal_surface");
        }
#endif
#if SPARGEL_IS_LINUX
        else if (strcmp(name, "VK_KHR_xcb_surface") == 0) {
            char const** ptr = (char const**)array_push(&use_exts);
            *ptr = "VK_KHR_xcb_surface";
            has_xcb_surface = true;
            sbase_log_info("use instance extension VK_KHR_xcb_surface");
        } else if (strcmp(name, "VK_KHR_wayland_surface") == 0) {
            char const** ptr = (char const**)array_push(&use_exts);
            *ptr = "VK_KHR_wayland_surface";
            has_wayland_surface = true;
            sbase_log_info("use instance extension VK_KHR_wayland_surface");
        }
#endif
    }
    if (!has_surface) {
        sbase_log_fatal("VK_KHR_surface is required");
        sbase_panic_here();
    }
#if SPARGEL_IS_MACOS
    if (!has_metal_surface) {
        sbase_log_fatal("VK_EXT_metal_surface is required");
        sbase_panic_here();
    }
#endif
#if SPARGEL_IS_LINUX
    if (descriptor->platform == SUI_PLATFORM_XCB && !has_xcb_surface) {
        sbase_log_fatal("VK_KHR_xcb_surface is required");
        sbase_panic_here();
    }
    if (descriptor->platform == SUI_PLATFORM_WAYLAND && !has_wayland_surface) {
        sbase_log_fatal("VK_KHR_wayland_surface is required");
        sbase_panic_here();
    }
#endif

    /* step 5. create instance */

    VkInstance instance = NULL;
    {
        VkApplicationInfo app_info;
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pNext = NULL;
        app_info.pApplicationName = NULL;
        app_info.applicationVersion = 0;
        app_info.pEngineName = "Spargel Engine";
        app_info.engineVersion = 0;
        app_info.apiVersion = VK_API_VERSION_1_2;
        VkInstanceCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        info.pNext = NULL;
        info.flags = 0;
        info.pApplicationInfo = &app_info;
        info.enabledLayerCount = use_layers.count;
        info.ppEnabledLayerNames = (char const**)use_layers.data;
        info.enabledExtensionCount = use_exts.count;
        info.ppEnabledExtensionNames = (char const**)use_exts.data;

        /**
         * VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR specifies that the instance will
         * enumerate available Vulkan Portability-compliant physical devices and groups in addition
         * to the Vulkan physical devices and groups that are enumerated by default.
         */
        if (has_portability) {
            info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        }

        CHECK_VK_RESULT(procs->vkCreateInstance(&info, NULL, &instance));
        d->instance = instance;
    }

#define VULKAN_INSTANCE_PROC(name) procs->name = (PFN_##name)vkGetInstanceProcAddr(instance, #name);
#include <spargel/gpu/vulkan_procs.inc>

    deinit_array(use_exts);
    deinit_array(use_layers);
    deinit_array(all_exts);
    deinit_array(all_layers);

    /* step 6. create debug messenger */
    if (has_debug_utils) {
        VkDebugUtilsMessengerCreateInfoEXT info;
        info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        info.pNext = 0;
        info.flags = 0;
        info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        info.pfnUserCallback = sgpu_vulkan_debug_messenger_callback;
        info.pUserData = 0;

        CHECK_VK_RESULT(
            procs->vkCreateDebugUtilsMessengerEXT(instance, &info, 0, &d->debug_messenger));
    }

    /* step 7. enumerate physical devices */

    struct array adapters;
    init_array(&adapters, sizeof(VkPhysicalDevice));
    {
        u32 count;
        CHECK_VK_RESULT(procs->vkEnumeratePhysicalDevices(instance, &count, NULL));
        array_reserve(&adapters, count);
        CHECK_VK_RESULT(
            procs->vkEnumeratePhysicalDevices(instance, &count, (VkPhysicalDevice*)adapters.data));
        adapters.count = count;
    }

    /* step 8. choose a physical device */
    /**
     * Spec:
     *
     * The general expectation is that a physical device groups all queues of matching capabilities
     * into a single family. However, while implementations should do this, it is possible that a
     * physical device may return two separate queue families with the same capabilities.
     *
     *
     * If an implementation exposes any queue family that supports graphics operations, at least one
     * queue family of at least one physical device exposed by the implementation must support both
     * graphics and compute operations.
     *
     *
     * All commands that are allowed on a queue that supports transfer operations are also allowed
     * on a queue that supports either graphics or compute operations. Thus, if the capabilities of
     * a queue family include VK_QUEUE_GRAPHICS_BIT or VK_QUEUE_COMPUTE_BIT, then reporting the
     * VK_QUEUE_TRANSFER_BIT capability separately for that queue family is optional.
     */
    /**
     * We need one graphics queue. So there exists one queue family that supports both graphics and
     * compute.
     */

    struct array queue_families;
    init_array(&queue_families, sizeof(VkQueueFamilyProperties));

    VkPhysicalDevice adapter;
    ssize queue_family_index;

    for (ssize i = 0; i < adapters.count; i++) {
        queue_families.count = 0;

        adapter = *(VkPhysicalDevice*)array_at(adapters, i);

        VkPhysicalDeviceProperties prop;
        procs->vkGetPhysicalDeviceProperties(adapter, &prop);
        sbase_log_info("adapter # %ld %s", i, prop.deviceName);

        /* step 8.1. check queue families */
        /* We use only one queue family for now. */
        {
            u32 count;
            procs->vkGetPhysicalDeviceQueueFamilyProperties(adapter, &count, 0);
            array_reserve(&queue_families, count);
            procs->vkGetPhysicalDeviceQueueFamilyProperties(
                adapter, &count, (VkQueueFamilyProperties*)queue_families.data);
            queue_families.count = count;
        }
        queue_family_index = -1;
        for (ssize j = 0; j < queue_families.count; j++) {
            VkQueueFamilyProperties* prop = (VkQueueFamilyProperties*)array_at(queue_families, j);
            /**
             * Spec:
             * Each queue family must support at least one queue.
             */
            /* advertising VK_QUEUEU_TRANSFER_BIT is optional */
            VkQueueFlags flags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;
            if (prop->queueFlags & flags) {
                /* now check for presentation */
                /* todo: how ??? */
                queue_family_index = j;
                break;
            }
        }
        if (queue_family_index < 0) continue;

        /* no more requirements */
        break;
    }

    d->physical_device = adapter;
    d->queue_family = queue_family_index;

    deinit_array(queue_families);
    deinit_array(adapters);

    /* step 9. enumerate device extensions */
    init_array(&all_exts, sizeof(struct VkExtensionProperties));
    {
        u32 count;
        CHECK_VK_RESULT(procs->vkEnumerateDeviceExtensionProperties(adapter, 0, &count, 0));
        array_reserve(&all_exts, count);
        CHECK_VK_RESULT(procs->vkEnumerateDeviceExtensionProperties(
            adapter, 0, &count, (VkExtensionProperties*)all_exts.data));
        all_exts.count = count;
    }
    for (ssize i = 0; i < all_exts.count; i++) {
        sbase_log_info("device extension #%ld = %s", i,
                       ((struct VkExtensionProperties*)array_at(all_exts, i))->extensionName);
    }

    /* step 10. choose device extensions */

    init_array(&use_exts, sizeof(char const*));

    bool has_swapchain = false;

    for (ssize i = 0; i < all_exts.count; i++) {
        struct VkExtensionProperties* prop = (struct VkExtensionProperties*)array_at(all_exts, i);
        if (strcmp(prop->extensionName, "VK_KHR_swapchain") == 0) {
            char const** ptr = (char const**)array_push(&use_exts);
            *ptr = "VK_KHR_swapchain";
            has_swapchain = true;
            sbase_log_info("use instance extension VK_KHR_surface");
        } else if (strcmp(prop->extensionName, "VK_KHR_portability_subset") == 0) {
            char const** ptr = (char const**)array_push(&use_exts);
            *ptr = "VK_KHR_portability_subset";
            sbase_log_info("use instance extension VK_KHR_portability_subset");
        }
    }

    if (!has_swapchain) {
        sbase_log_fatal("VK_KHR_swapchain is required");
        sbase_panic();
    }

    /* step 10. create device */

    VkDevice dev;

    {
        VkDeviceQueueCreateInfo queue_info;
        queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_info.pNext = 0;
        queue_info.flags = 0;
        queue_info.queueFamilyIndex = queue_family_index;
        queue_info.queueCount = 1;
        queue_info.pQueuePriorities = sgpu_vulkan_queue_priorities;

        VkDeviceCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        info.pNext = 0;
        info.flags = 0;
        info.queueCreateInfoCount = 1;
        info.pQueueCreateInfos = &queue_info;
        info.enabledLayerCount = 0;
        info.ppEnabledLayerNames = 0;
        info.enabledExtensionCount = use_exts.count;
        info.ppEnabledExtensionNames = (char const**)use_exts.data;
        info.pEnabledFeatures = 0;

        CHECK_VK_RESULT(procs->vkCreateDevice(adapter, &info, 0, &dev));
        d->device = dev;
    }

    PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr = procs->vkGetDeviceProcAddr;
#define VULKAN_DEVICE_PROC(name) procs->name = (PFN_##name)vkGetDeviceProcAddr(dev, #name);
#include <spargel/gpu/vulkan_procs.inc>

    deinit_array(use_exts);
    deinit_array(all_exts);

    d->queue.backend = SGPU_BACKEND_VULKAN;

    VkQueue queue;
    procs->vkGetDeviceQueue(dev, queue_family_index, 0, &queue);

    d->queue.queue = queue;

    VkCommandPool cmd_pool;
    {
        VkCommandPoolCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        info.pNext = 0;
        info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        info.queueFamilyIndex = queue_family_index;
        CHECK_VK_RESULT(procs->vkCreateCommandPool(dev, &info, 0, &cmd_pool));
    }
    d->cmd_pool = cmd_pool;

    *device = (sgpu_device_id)d;
    return SGPU_RESULT_SUCCESS;
}

void sgpu_vulkan_destroy_device(sgpu_device_id device) {
    cast_object(sgpu_vulkan_device, d, device);
    struct sgpu_vulkan_proc_table* procs = &d->procs;

    procs->vkDestroyDevice(d->device, 0);
    if (d->debug_messenger)
        procs->vkDestroyDebugUtilsMessengerEXT(d->instance, d->debug_messenger, 0);
    procs->vkDestroyInstance(d->instance, 0);

    dlclose(d->library);
    dealloc_object(sgpu_vulkan_device, d);
}

int sgpu_vulkan_create_command_queue(sgpu_device_id device, sgpu_command_queue_id* queue) {
    cast_object(sgpu_vulkan_device, d, device);
    *queue = (sgpu_command_queue_id)&d->queue;
    return SGPU_RESULT_SUCCESS;
}

void sgpu_vulkan_destroy_command_queue(sgpu_device_id device, sgpu_command_queue_id queue) {}

int sgpu_vulkan_create_shader_function(
    sgpu_device_id device, struct sgpu_vulkan_shader_function_descriptor const* descriptor,
    sgpu_shader_function_id* func) {
    cast_object(sgpu_vulkan_device, d, device);
    alloc_object(sgpu_vulkan_shader_function, f);

    VkShaderModuleCreateInfo info;
    info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.pNext = 0;
    info.flags = 0;
    info.codeSize = descriptor->size;
    info.pCode = (u32*)descriptor->code;

    VkShaderModule mod;
    CHECK_VK_RESULT(d->procs.vkCreateShaderModule(d->device, &info, 0, &mod));
    f->shader = mod;
    f->device = d;

    *func = (sgpu_shader_function_id)f;
    return SGPU_RESULT_SUCCESS;
}

void sgpu_vulkan_destroy_shader_function(sgpu_device_id device, sgpu_shader_function_id func) {
    cast_object(sgpu_vulkan_shader_function, f, func);
    struct sgpu_vulkan_device* d = f->device;
    d->procs.vkDestroyShaderModule(d->device, f->shader, 0);
    dealloc_object(sgpu_vulkan_shader_function, f);
}

int sgpu_vulkan_create_render_pipeline(sgpu_device_id device,
                                       struct sgpu_render_pipeline_descriptor const* descriptor,
                                       sgpu_render_pipeline_id* pipeline) {
    alloc_object(sgpu_vulkan_render_pipeline, p);

    VkGraphicsPipelineCreateInfo pipeline_info;
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.pNext = 0;
    pipeline_info.flags = 0;
    pipeline_info.stageCount = 2;

    *pipeline = (sgpu_render_pipeline_id)p;
    return SGPU_RESULT_SUCCESS;
}

void sgpu_vulkan_destroy_render_pipeline(sgpu_device_id device, sgpu_render_pipeline_id pipeline) {
    cast_object(sgpu_vulkan_render_pipeline, p, pipeline);
    dealloc_object(sgpu_vulkan_render_pipeline, p);
}

int sgpu_vulkan_create_command_buffer(sgpu_device_id device,
                                      struct sgpu_command_buffer_descriptor const* descriptor,
                                      sgpu_command_buffer_id* command_buffer) {
    cast_object(sgpu_vulkan_device, d, device);
    cast_object(sgpu_vulkan_command_queue, q, descriptor->queue);
    alloc_object(sgpu_vulkan_command_buffer, cmdbuf);

    VkCommandBufferAllocateInfo info;
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.pNext = 0;
    info.commandPool = d->cmd_pool;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    info.commandBufferCount = 1;
    CHECK_VK_RESULT(d->procs.vkAllocateCommandBuffers(d->device, &info, &cmdbuf->command_buffer));

    cmdbuf->queue = q->queue;

    *command_buffer = (sgpu_command_buffer_id)cmdbuf;
    return SGPU_RESULT_SUCCESS;
}

void sgpu_vulkan_destroy_command_buffer(sgpu_device_id device,
                                        sgpu_command_buffer_id command_buffer) {
    cast_object(sgpu_vulkan_command_buffer, c, command_buffer);
    dealloc_object(sgpu_vulkan_command_buffer, c);
}

void sgpu_vulkan_reset_command_buffer(sgpu_device_id device,
                                      sgpu_command_buffer_id command_buffer) {
    cast_object(sgpu_vulkan_device, d, device);
    cast_object(sgpu_vulkan_command_buffer, c, command_buffer);
    CHECK_VK_RESULT(d->procs.vkResetCommandBuffer(c->command_buffer, 0));
}

int sgpu_vulkan_create_surface(sgpu_device_id device,
                               struct sgpu_surface_descriptor const* descriptor,
                               sgpu_surface_id* surface) {
    cast_object(sgpu_vulkan_device, d, device);
    alloc_object(sgpu_vulkan_surface, s);

    spargel::ui::window_handle wh = spargel::ui::window_get_handle(descriptor->window);
    VkSurfaceKHR surf;
#if SPARGEL_IS_MACOS
    VkMetalSurfaceCreateInfoEXT info;
    info.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
    info.pNext = 0;
    info.flags = 0;
    info.pLayer = wh.apple.layer;
    CHECK_VK_RESULT(d->procs.vkCreateMetalSurfaceEXT(d->instance, &info, 0, &surf));
#elif SPARGEL_IS_LINUX /* todo: wayland */
    VkXcbSurfaceCreateInfoKHR info;
    info.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    info.pNext = 0;
    info.flags = 0;
    info.connection = wh.xcb.connection;
    info.window = wh.xcb.window;
    CHECK_VK_RESULT(d->procs.vkCreateXcbSurfaceKHR(d->instance, &info, 0, &surf));
#endif

    s->surface = surf;

    *surface = (sgpu_surface_id)s;

    return SGPU_RESULT_SUCCESS;
}

void sgpu_vulkan_destroy_surface(sgpu_device_id device, sgpu_surface_id surface) {
    cast_object(sgpu_vulkan_device, d, device);
    cast_object(sgpu_vulkan_surface, s, surface);
    d->procs.vkDestroySurfaceKHR(d->instance, s->surface, 0);
    dealloc_object(sgpu_vulkan_surface, s);
}

int sgpu_vulkan_create_swapchain(sgpu_device_id device,
                                 struct sgpu_swapchain_descriptor const* descriptor,
                                 sgpu_swapchain_id* swapchain) {
    cast_object(sgpu_vulkan_device, d, device);
    cast_object(sgpu_vulkan_surface, sf, descriptor->surface);
    alloc_object(sgpu_vulkan_swapchain, sw);

    struct sgpu_vulkan_proc_table* procs = &d->procs;

    VkSurfaceCapabilitiesKHR surf_caps;
    CHECK_VK_RESULT(procs->vkGetPhysicalDeviceSurfaceCapabilitiesKHR(d->physical_device,
                                                                     sf->surface, &surf_caps));

    u32 min_images = surf_caps.minImageCount + 1;
    if (surf_caps.maxImageCount > 0 && min_images > surf_caps.maxImageCount) {
        min_images = surf_caps.maxImageCount;
    }

    struct array formats;
    init_array(&formats, sizeof(VkSurfaceFormatKHR));
    {
        u32 count;
        CHECK_VK_RESULT(procs->vkGetPhysicalDeviceSurfaceFormatsKHR(d->physical_device, sf->surface,
                                                                    &count, 0));
        array_reserve(&formats, count);
        CHECK_VK_RESULT(procs->vkGetPhysicalDeviceSurfaceFormatsKHR(
            d->physical_device, sf->surface, &count, (VkSurfaceFormatKHR*)formats.data));
        formats.count = count;
    }

    VkSurfaceFormatKHR* chosen_format = (VkSurfaceFormatKHR*)array_at(formats, 0);
    for (ssize i = 0; i < formats.count; i++) {
        VkSurfaceFormatKHR* fmt = (VkSurfaceFormatKHR*)array_at(formats, i);
        if (fmt->format == VK_FORMAT_B8G8R8A8_SRGB &&
            fmt->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            chosen_format = fmt;
            break;
        }
    }

    VkSwapchainCreateInfoKHR info;
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.pNext = 0;
    info.flags = 0;
    info.surface = sf->surface;
    info.minImageCount = min_images;
    info.imageFormat = chosen_format->format;
    info.imageColorSpace = chosen_format->colorSpace;
    info.imageExtent.width = descriptor->width;
    info.imageExtent.height = descriptor->height;
    info.imageArrayLayers = 1;
    info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.queueFamilyIndexCount = 0;
    info.pQueueFamilyIndices = 0;
    info.preTransform = surf_caps.currentTransform;
    info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    info.clipped = VK_TRUE;
    info.oldSwapchain = 0;

    CHECK_VK_RESULT(procs->vkCreateSwapchainKHR(d->device, &info, 0, &sw->swapchain));

    {
        u32 count;
        CHECK_VK_RESULT(procs->vkGetSwapchainImagesKHR(d->device, sw->swapchain, &count, 0));
        sw->image_capacity = count;
        sw->images = (VkImage*)sbase_allocate(sizeof(VkImage) * count, SBASE_ALLOCATION_GPU);
        sw->image_views =
            (VkImageView*)sbase_allocate(sizeof(VkImageView) * count, SBASE_ALLOCATION_GPU);
        sw->framebuffers =
            (VkFramebuffer*)sbase_allocate(sizeof(VkFramebuffer) * count, SBASE_ALLOCATION_GPU);
        CHECK_VK_RESULT(
            procs->vkGetSwapchainImagesKHR(d->device, sw->swapchain, &count, sw->images));
        sw->image_count = count;
    }

    {
        VkImageViewCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.pNext = 0;
        info.flags = 0;
        info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        info.format = chosen_format->format;
        info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        info.subresourceRange.baseMipLevel = 0;
        info.subresourceRange.levelCount = 1;
        info.subresourceRange.baseArrayLayer = 0;
        info.subresourceRange.layerCount = 1;
        for (ssize i = 0; i < sw->image_count; i++) {
            info.image = sw->images[i];
            CHECK_VK_RESULT(procs->vkCreateImageView(d->device, &info, 0, &sw->image_views[i]));
        }
    }

    /* create a dummy render pass */

    VkRenderPass rp;
    {
        VkAttachmentDescription attachment;
        attachment.flags = 0;
        attachment.format = chosen_format->format;
        attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference attachment_ref;
        attachment_ref.attachment = 0;
        attachment_ref.layout = VK_IMAGE_LAYOUT_GENERAL;

        VkSubpassDescription subpass;
        subpass.flags = 0;
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.inputAttachmentCount = 0;
        subpass.pInputAttachments = 0;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &attachment_ref;
        subpass.pResolveAttachments = 0;
        subpass.pDepthStencilAttachment = 0;
        subpass.preserveAttachmentCount = 0;
        subpass.pPreserveAttachments = 0;

        VkSubpassDependency dependency;
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependency.dependencyFlags = 0;

        VkRenderPassCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        info.pNext = 0;
        info.flags = 0;
        info.attachmentCount = 1;
        info.pAttachments = &attachment;
        info.subpassCount = 1;
        info.pSubpasses = &subpass;
        info.dependencyCount = 1;
        info.pDependencies = &dependency;
        CHECK_VK_RESULT(procs->vkCreateRenderPass(d->device, &info, 0, &rp));
    }
    sw->render_pass = rp;

    {
        VkFramebufferCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info.pNext = 0;
        info.flags = 0;
        info.renderPass = rp;
        info.attachmentCount = 1;
        info.width = descriptor->width;
        info.height = descriptor->height;
        info.layers = 1;
        for (ssize i = 0; i < sw->image_count; i++) {
            info.pAttachments = &sw->image_views[i];
            CHECK_VK_RESULT(procs->vkCreateFramebuffer(d->device, &info, 0, &sw->framebuffers[i]));
        }
    }

    deinit_array(formats);

    sw->width = descriptor->width;
    sw->height = descriptor->height;

    {
        VkFenceCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        info.pNext = 0;
        info.flags = 0;
        CHECK_VK_RESULT(procs->vkCreateFence(d->device, &info, 0, &sw->submit_done));
    }
    {
        VkFenceCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        info.pNext = 0;
        info.flags = 0;
        CHECK_VK_RESULT(procs->vkCreateFence(d->device, &info, 0, &sw->image_available));
    }
    {
        VkSemaphoreCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        info.pNext = 0;
        info.flags = 0;
        CHECK_VK_RESULT(procs->vkCreateSemaphore(d->device, &info, 0, &sw->render_complete));
    }

    *swapchain = (sgpu_swapchain_id)sw;
    return SGPU_RESULT_SUCCESS;
}

void sgpu_vulkan_destroy_swapchain(sgpu_device_id device, sgpu_swapchain_id swapchain) {
    cast_object(sgpu_vulkan_device, d, device);
    cast_object(sgpu_vulkan_swapchain, s, swapchain);

    struct sgpu_vulkan_proc_table* procs = &d->procs;

    for (ssize i = 0; i < s->image_count; i++) {
        procs->vkDestroyFramebuffer(d->device, s->framebuffers[i], 0);
    }
    for (ssize i = 0; i < s->image_count; i++) {
        procs->vkDestroyImageView(d->device, s->image_views[i], 0);
    }
    procs->vkDestroyRenderPass(d->device, s->render_pass, 0);
    procs->vkDestroySwapchainKHR(d->device, s->swapchain, 0);

    sbase_deallocate(s->framebuffers, sizeof(VkFramebuffer) * s->image_capacity,
                     SBASE_ALLOCATION_GPU);
    sbase_deallocate(s->image_views, sizeof(VkImageView) * s->image_capacity, SBASE_ALLOCATION_GPU);
    sbase_deallocate(s->images, sizeof(VkImage) * s->image_capacity, SBASE_ALLOCATION_GPU);

    dealloc_object(sgpu_vulkan_swapchain, s);
}

int sgpu_vulkan_acquire_image(sgpu_device_id device,
                              struct sgpu_acquire_descriptor const* descriptor,
                              sgpu_presentable_id* presentable) {
    cast_object(sgpu_vulkan_device, d, device);
    cast_object(sgpu_vulkan_swapchain, s, descriptor->swapchain);

    /**
     * Spec:
     *
     * After acquiring a presentable image and before modifying it, the application must use a
     * synchronization primitive to ensure that the presentation engine has finished reading from
     * the image.
     *
     * The presentation engine may not have finished reading from the image at the time it is
     * acquired, so the application must use semaphore and/or fence to ensure that the image layout
     * and contents are not modified until the presentation engine reads have completed.
     */
    int result = d->procs.vkAcquireNextImageKHR(d->device, s->swapchain, UINT64_MAX, 0,
                                                s->image_available, &s->presentable.index);
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        sbase_log_fatal("cannot acquire next swapchain image");
        sbase_panic_here();
    }

    CHECK_VK_RESULT(
        d->procs.vkWaitForFences(d->device, 1, &s->image_available, VK_TRUE, UINT64_MAX));
    CHECK_VK_RESULT(d->procs.vkResetFences(d->device, 1, &s->image_available));

    s->presentable.swapchain = s;

    *presentable = (sgpu_presentable_id)&s->presentable;
    return SGPU_RESULT_SUCCESS;
}

void sgpu_vulkan_begin_render_pass(sgpu_device_id device,
                                   struct sgpu_render_pass_descriptor const* descriptor,
                                   sgpu_render_pass_encoder_id* encoder) {
    cast_object(sgpu_vulkan_device, d, device);
    cast_object(sgpu_vulkan_command_buffer, cmdbuf, descriptor->command_buffer);
    cast_object(sgpu_vulkan_swapchain, sw, descriptor->swapchain);
    alloc_object(sgpu_vulkan_render_pass_encoder, e);

    {
        VkCommandBufferBeginInfo info;
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.pNext = 0;
        info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        info.pInheritanceInfo = 0;
        CHECK_VK_RESULT(d->procs.vkBeginCommandBuffer(cmdbuf->command_buffer, &info));
    }
    {
        VkClearValue clear;
        /* bug: double to float */
        clear.color.float32[0] = descriptor->clear_color.r;
        clear.color.float32[1] = descriptor->clear_color.g;
        clear.color.float32[2] = descriptor->clear_color.b;
        clear.color.float32[3] = descriptor->clear_color.a;

        VkRenderPassBeginInfo info;
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.pNext = 0;
        /* !!! */
        info.renderPass = sw->render_pass;
        /* !!! */
        info.framebuffer = sw->framebuffers[sw->presentable.index];
        info.renderArea.offset.x = 0;
        info.renderArea.offset.y = 0;
        info.renderArea.extent.width = sw->width;
        info.renderArea.extent.height = sw->height;
        info.clearValueCount = 1;
        info.pClearValues = &clear;

        d->procs.vkCmdBeginRenderPass(cmdbuf->command_buffer, &info, VK_SUBPASS_CONTENTS_INLINE);
    }

    e->command_buffer = cmdbuf->command_buffer;

    *encoder = (sgpu_render_pass_encoder_id)e;
}

void sgpu_vulkan_end_render_pass(sgpu_device_id device, sgpu_render_pass_encoder_id encoder) {
    cast_object(sgpu_vulkan_device, d, device);
    cast_object(sgpu_vulkan_render_pass_encoder, e, encoder);
    d->procs.vkCmdEndRenderPass(e->command_buffer);
    CHECK_VK_RESULT(d->procs.vkEndCommandBuffer(e->command_buffer));
    dealloc_object(sgpu_vulkan_render_pass_encoder, e);
}

void sgpu_vulkan_present(sgpu_device_id device, struct sgpu_present_descriptor const* descriptor) {
    cast_object(sgpu_vulkan_device, d, device);
    cast_object(sgpu_vulkan_command_buffer, c, descriptor->command_buffer);
    cast_object(sgpu_vulkan_presentable, p, descriptor->presentable);
    {
        VkSubmitInfo info;
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.pNext = 0;
        info.waitSemaphoreCount = 0;
        info.pWaitSemaphores = 0;
        VkPipelineStageFlags stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        info.pWaitDstStageMask = &stage;
        info.commandBufferCount = 1;
        info.pCommandBuffers = &c->command_buffer;
        info.signalSemaphoreCount = 1;
        info.pSignalSemaphores = &p->swapchain->render_complete;
        CHECK_VK_RESULT(d->procs.vkQueueSubmit(c->queue, 1, &info, p->swapchain->submit_done));
    }
    /**
     * Spec:
     *
     * Calls to vkQueuePresentKHR may block, but must return in finite time. The processing of the
     * presentation happens in issue order with other queue operations, but semaphores must be used
     * to ensure that prior rendering and other commands in the specified queue complete before the
     * presentation begins. The presentation command itself does not delay processing of subsequent
     * commands on the queue. However, presentation requests sent to a particular queue are always
     * performed in order. Exact presentation timing is controlled by the semantics of the
     * presentation engine and native platform in use.
     */
    {
        VkPresentInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.pNext = 0;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &p->swapchain->render_complete;
        info.swapchainCount = 1;
        info.pSwapchains = &p->swapchain->swapchain;
        info.pImageIndices = &p->index;
        info.pResults = 0;
        int result = d->procs.vkQueuePresentKHR(c->queue, &info);
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            sbase_log_fatal("cannot acquire next swapchain image");
            sbase_panic_here();
        }
    }
    /* todo */
    CHECK_VK_RESULT(
        d->procs.vkWaitForFences(d->device, 1, &p->swapchain->submit_done, VK_TRUE, UINT64_MAX));
    CHECK_VK_RESULT(d->procs.vkResetFences(d->device, 1, &p->swapchain->submit_done));
}

void sgpu_vulkan_presentable_texture(sgpu_device_id device, sgpu_presentable_id presentable,
                                     sgpu_texture_id* texture) {
    *texture = 0;
}
