#include <spargel/base/base.h>
#include <spargel/config.h>
#include <spargel/gpu/gpu_vulkan.h>

/* libc */
#include <stdlib.h>
#include <string.h>

/* platform */
#include <dlfcn.h>

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

#define alloc_object(type, name)                                                   \
    struct type* name = sbase_allocate(sizeof(struct type), SBASE_ALLOCATION_GPU); \
    if (!name) return SGPU_RESULT_ALLOCATION_FAILED;                               \
    memset(name, 0, sizeof(struct type));                                          \
    name->backend = SGPU_BACKEND_VULKAN;

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

struct sgpu_vulkan_device {
    int backend;
    void* library;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkPhysicalDevice physical_device;
    VkDevice device;
    struct sgpu_vulkan_proc_table procs;
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
    CHECK(i >= 0 && i < a.count);
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
    sbase_log_debug("validator: %s", data->pMessage);
    return VK_FALSE;
}

static int sgpu_vulkan_create_instance(struct sgpu_vulkan_device* device) {
    struct sgpu_vulkan_proc_table* procs = &device->procs;

    /* step 1. enumerate layers */
    struct array all_layers;
    init_array(&all_layers, sizeof(struct VkLayerProperties));
    {
        u32 count;
        CHECK_VK_RESULT(procs->vkEnumerateInstanceLayerProperties(&count, NULL));
        array_reserve(&all_layers, count);
        CHECK_VK_RESULT(procs->vkEnumerateInstanceLayerProperties(&count, all_layers.data));
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
        CHECK_VK_RESULT(procs->vkEnumerateInstanceExtensionProperties(NULL, &count, all_exts.data));
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
            char const** ptr = array_push(&use_layers);
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
            char const** ptr = array_push(&use_exts);
            *ptr = "VK_KHR_surface";
            has_surface = true;
            sbase_log_info("use instance extension VK_KHR_surface");
        } else if (strcmp(name, "VK_KHR_portability_enumeration") == 0) {
            char const** ptr = array_push(&use_exts);
            *ptr = "VK_KHR_portability_enumeration";
            has_portability = true;
            sbase_log_info("use instance extension VK_KHR_portability_enumeration");
        } else if (strcmp(name, "VK_EXT_debug_utils") == 0) {
            char const** ptr = array_push(&use_exts);
            *ptr = "VK_EXT_debug_utils";
            has_debug_utils = true;
            sbase_log_info("use instance extension VK_EXT_debug_utils");
        }
#if SPARGEL_IS_MACOS
        else if (strcmp(name, "VK_EXT_metal_surface") == 0) {
            char const** ptr = array_push(&use_exts);
            *ptr = "VK_EXT_metal_surface";
            has_metal_surface = true;
            sbase_log_info("use instance extension VK_EXT_metal_surface");
        }
#endif
#if SPARGEL_IS_LINUX
        else if (strcmp(name, "VK_KHR_xcb_surface") == 0) {
            char const** ptr = array_push(&use_exts);
            *ptr = "VK_KHR_xcb_surface";
            has_xcb_surface = true;
            sbase_log_info("use instance extension VK_KHR_xcb_surface");
        } else if (strcmp(name, "VK_KHR_wayland_surface") == 0) {
            char const** ptr = array_push(&use_exts);
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
    if (!has_xcb_surface && !has_wayland_surface) {
        sbase_log_fatal("VK_KHR_xcb_surface or VK_KHR_wayland_surface is required");
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
        info.ppEnabledLayerNames = use_layers.data;
        info.enabledExtensionCount = use_exts.count;
        info.ppEnabledExtensionNames = use_exts.data;

        /**
         * VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR specifies that the instance will
         * enumerate available Vulkan Portability-compliant physical devices and groups in addition
         * to the Vulkan physical devices and groups that are enumerated by default.
         */
        if (has_portability) {
            info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        }

        CHECK_VK_RESULT(device->procs.vkCreateInstance(&info, NULL, &instance));
        device->instance = instance;
    }

    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = device->procs.vkGetInstanceProcAddr;
#define VULKAN_INSTANCE_PROC(name) \
    device->procs.name = (PFN_##name)vkGetInstanceProcAddr(instance, #name);
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
            procs->vkCreateDebugUtilsMessengerEXT(instance, &info, 0, &device->debug_messenger));
    }

    /* step 7. enumerate physical devices */

    struct array adapters;
    init_array(&adapters, sizeof(VkPhysicalDevice));
    {
        u32 count;
        CHECK_VK_RESULT(procs->vkEnumeratePhysicalDevices(instance, &count, NULL));
        array_reserve(&adapters, count);
        CHECK_VK_RESULT(procs->vkEnumeratePhysicalDevices(instance, &count, adapters.data));
        adapters.count = count;
    }

    /* step 8. choose a physical device */
    /**
     * The general expectation is that a physical device groups all queues of matching capabilities
     * into a single family. However, while implementations should do this, it is possible that a
     * physical device may return two separate queue families with the same capabilities.
     */
    /**
     * If an implementation exposes any queue family that supports graphics operations, at least one
     * queue family of at least one physical device exposed by the implementation must support both
     * graphics and compute operations.
     */
    /**
     * All commands that are allowed on a queue that supports transfer operations are also allowed
     * on a queue that supports either graphics or compute operations. Thus, if the capabilities of
     * a queue family include VK_QUEUE_GRAPHICS_BIT or VK_QUEUE_COMPUTE_BIT, then reporting the
     * VK_QUEUE_TRANSFER_BIT capability separately for that queue family is optional.
     */

    deinit_array(adapters);

    return SGPU_RESULT_SUCCESS;
}

int sgpu_vulkan_create_default_device(sgpu_device_id* device) {
    alloc_object(sgpu_vulkan_device, d);
    d->library = dlopen(VULKAN_LIB_FILENAME, RTLD_NOW | RTLD_LOCAL);
    if (d->library == NULL) {
        dealloc_object(sgpu_vulkan_device, d);
        return SGPU_RESULT_NO_BACKEND;
    }

    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dlsym(d->library, "vkGetInstanceProcAddr");
    d->procs.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
#define VULKAN_GENERAL_PROC(name) d->procs.name = (PFN_##name)vkGetInstanceProcAddr(NULL, #name);
#include <spargel/gpu/vulkan_procs.inc>

    sgpu_vulkan_create_instance(d);
    *device = (sgpu_device_id)d;
    return SGPU_RESULT_SUCCESS;
}

void sgpu_vulkan_destroy_device(sgpu_device_id device) {
    cast_object(sgpu_vulkan_device, d, device);
    struct sgpu_vulkan_proc_table* procs = &d->procs;

    if (d->debug_messenger)
        procs->vkDestroyDebugUtilsMessengerEXT(d->instance, d->debug_messenger, 0);
    procs->vkDestroyInstance(d->instance, 0);

    dlclose(d->library);
    dealloc_object(sgpu_vulkan_device, d);
}

int sgpu_vulkan_create_command_queue(sgpu_device_id device, sgpu_command_queue_id* queue) {
    sbase_panic_here();
}

void sgpu_vulkan_destroy_command_queue(sgpu_command_queue_id queue) { sbase_panic_here(); }

void sgpu_vulkan_destroy_shader_function(sgpu_shader_function_id func) { sbase_panic_here(); }

int sgpu_vulkan_create_shader_function(
    sgpu_device_id device, struct sgpu_vulkan_shader_function_descriptor const* descriptor,
    sgpu_shader_function_id* func) {
    sbase_panic_here();
}
