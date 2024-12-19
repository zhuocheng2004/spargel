#include <spargel/base/assert.h>
#include <spargel/base/base.h>
#include <spargel/base/logging.h>
#include <spargel/base/object.h>
#include <spargel/base/vector.h>
#include <spargel/config.h>
#include <spargel/gpu/gpu_vulkan.h>
#include <spargel/ui/ui.h>

/* libc */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* platform */
#if SPARGEL_IS_POSIX
#include <dlfcn.h>
#endif

#define VK_NO_PROTOTYPES
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#if SPARGEL_IS_ANDROID
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_android.h>
#endif

#if SPARGEL_IS_LINUX
#include <xcb/xcb.h>
/* after xcb */
#include <vulkan/vulkan_wayland.h>
#include <vulkan/vulkan_xcb.h>
#endif

#if SPARGEL_IS_MACOS
#include <vulkan/vulkan_metal.h>
#endif

#if SPARGEL_IS_WINDOWS
#include <spargel/base/win_procs.h>
#include <vulkan/vulkan_win32.h>
#endif

#if SPARGEL_IS_ANDROID
#define VULKAN_LIB_FILENAME "libvulkan.so"
#elif SPARGEL_IS_LINUX
#define VULKAN_LIB_FILENAME "libvulkan.so.1"
#elif SPARGEL_IS_MACOS
#define VULKAN_LIB_FILENAME "libvulkan.dylib"
#elif SPARGEL_IS_WINDOWS
#define VULKAN_LIB_FILENAME "vulkan-1.dll"
#endif

#define alloc_object(type, name)                                                              \
    type* name = (type*)spargel::base::allocate(sizeof(type), spargel::base::ALLOCATION_GPU); \
    if (!name) spargel_panic_here();                                                          \
    base::construct_at<type>(name);
// memset(name, 0, sizeof(struct type));

#define cast_object(type, name, object) struct type* name = (struct type*)(object);

#define dealloc_object(type, name) \
    base::destruct_at<type>(name); \
    spargel::base::deallocate(name, sizeof(struct type), spargel::base::ALLOCATION_GPU);

namespace spargel::gpu {

    struct vulkan_proc_table {
#define VULKAN_PROC_DECL(name) PFN_##name name;
#define VULKAN_LIBRARY_PROC(name) VULKAN_PROC_DECL(name)
#define VULKAN_GENERAL_PROC(name) VULKAN_PROC_DECL(name)
#define VULKAN_INSTANCE_PROC(name) VULKAN_PROC_DECL(name)
#define VULKAN_DEVICE_PROC(name) VULKAN_PROC_DECL(name)
#include <spargel/gpu/vulkan_procs.inc>
#undef VULKAN_PROC_DECL
    };

    struct vulkan_command_queue {
        int backend;
        VkQueue queue;
    };

    struct vulkan_surface {
        VkSurfaceKHR surface;
    };

    struct vulkan_swapchain;

    struct vulkan_presentable {
        u32 index;
        vulkan_swapchain* swapchain;
    };

    struct vulkan_swapchain {
        VkSwapchainKHR swapchain;
        int width;
        int height;
        /* todo: fix this */
        VkRenderPass render_pass;
        base::vector<VkImage> images;
        base::vector<VkImageView> image_views;
        base::vector<VkFramebuffer> framebuffers;
        /* todo: fix this */
        struct vulkan_presentable presentable;
        VkFence image_available;
        VkSemaphore render_complete;
        VkFence submit_done;
    };

    struct vulkan_command_buffer {
        VkCommandBuffer command_buffer;
        VkQueue queue;
    };

    struct vulkan_render_pass_encoder {
        VkCommandBuffer command_buffer;
    };

    struct vulkan_device {
        int backend;
        void* library;
        VkInstance instance;
        VkDebugUtilsMessengerEXT debug_messenger;
        VkPhysicalDevice physical_device;
        u32 queue_family;
        VkDevice device;
        struct vulkan_command_queue queue;
        VkCommandPool cmd_pool;
        struct vulkan_proc_table procs;
    };

    struct vulkan_shader_function {
        int backend;
        VkShaderModule shader;
        struct vulkan_device* device;
    };

    struct vulkan_render_pipeline {
        int backend;
        VkPipeline pipeline;
        struct vulkan_device* device;
    };

#define CHECK_VK_RESULT(expr)                                            \
    do {                                                                 \
        VkResult result = expr;                                          \
        if (result != VK_SUCCESS) {                                      \
            spargel_log_fatal(#expr " failed with result = %d", result); \
            spargel_panic_here();                                        \
        }                                                                \
    } while (0)

    static VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_debug_messenger_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type,
        VkDebugUtilsMessengerCallbackDataEXT const* data, void* user_data) {
        fprintf(stderr, "validator: %s\n", data->pMessage);
        return VK_FALSE;
    }

    static float const vulkan_queue_priorities[64] = {
        1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    };

    int vulkan_create_default_device(struct device_descriptor const* descriptor,
                                     device_id* device) {
        alloc_object(vulkan_device, d);
        d->backend = BACKEND_VULKAN;
#if SPARGEL_IS_POSIX
        d->library = dlopen(VULKAN_LIB_FILENAME, RTLD_NOW | RTLD_LOCAL);
#elif SPARGEL_IS_WINDOWS
        d->library = LoadLibraryA(VULKAN_LIB_FILENAME);
#else
#error unimplemented
#endif
        if (d->library == NULL) {
            dealloc_object(vulkan_device, d);
            return RESULT_NO_BACKEND;
        }
#if SPARGEL_IS_POSIX
        PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr =
            (PFN_vkGetInstanceProcAddr)dlsym(d->library, "vkGetInstanceProcAddr");
#elif SPARGEL_IS_WINDOWS
        PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr =
            (PFN_vkGetInstanceProcAddr)GetProcAddress(d->library, "vkGetInstanceProcAddr");
#else
#error unimplemented
#endif

        d->procs.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
#define VULKAN_GENERAL_PROC(name) d->procs.name = (PFN_##name)vkGetInstanceProcAddr(NULL, #name);
#include <spargel/gpu/vulkan_procs.inc>

        struct vulkan_proc_table* procs = &d->procs;

        /* step 1. enumerate layers */
        base::vector<VkLayerProperties> all_layers;
        {
            u32 count;
            CHECK_VK_RESULT(procs->vkEnumerateInstanceLayerProperties(&count, NULL));
            all_layers.reserve(count);
            CHECK_VK_RESULT(procs->vkEnumerateInstanceLayerProperties(&count, all_layers.data()));
            all_layers.set_count(count);
        }

        for (usize i = 0; i < all_layers.count(); i++) {
            spargel_log_info("layer #%zu = %s", i, all_layers[i].layerName);
        }

        /* step 2. enumerate instance extensions */
        base::vector<VkExtensionProperties> all_exts;
        {
            u32 count;
            CHECK_VK_RESULT(procs->vkEnumerateInstanceExtensionProperties(NULL, &count, NULL));
            all_exts.reserve(count);
            CHECK_VK_RESULT(
                procs->vkEnumerateInstanceExtensionProperties(NULL, &count, all_exts.data()));
            all_exts.set_count(count);
        }

        for (usize i = 0; i < all_exts.count(); i++) {
            spargel_log_info("instance extension #%zu = %s", i, all_exts[i].extensionName);
        }

        /* step 3. select layers */
        /* we need VK_LAYER_KHRONOS_validation */
        base::vector<char const*> use_layers;
        for (usize i = 0; i < all_layers.count(); i++) {
            if (strcmp(all_layers[i].layerName, "VK_LAYER_KHRONOS_validation") == 0) {
                use_layers.push("VK_LAYER_KHRONOS_validation");
                spargel_log_info("use layer VK_LAYER_KHRONOS_validation");
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
         * unless the application explicitly asks for them. This prevents applications which may not
         * be aware of non-conformant devices from accidentally using them, as any device which
         * supports the VK_KHR_portability_subset extension mandates that the extension must be
         * enabled if that device is used.
         */

        base::vector<char const*> use_exts;
        bool has_surface = false;
        bool has_portability = false;
        bool has_debug_utils = false;
#if SPARGEL_IS_ANDROID
        bool has_android_surface = false;
#endif
#if SPARGEL_IS_LINUX
        bool has_xcb_surface = false;
        bool has_wayland_surface = false;
#endif
#if SPARGEL_IS_MACOS
        bool has_metal_surface = false;
#endif
#if SPARGEL_IS_WINDOWS
        bool has_win32_surface = false;
#endif
        for (usize i = 0; i < all_exts.count(); i++) {
            char const* name = all_exts[i].extensionName;
            if (strcmp(name, "VK_KHR_surface") == 0) {
                use_exts.push("VK_KHR_surface");
                has_surface = true;
                spargel_log_info("use instance extension VK_KHR_surface");
            } else if (strcmp(name, "VK_KHR_portability_enumeration") == 0) {
                use_exts.push("VK_KHR_portability_enumeration");
                has_portability = true;
                spargel_log_info("use instance extension VK_KHR_portability_enumeration");
            } else if (strcmp(name, "VK_EXT_debug_utils") == 0) {
                use_exts.push("VK_EXT_debug_utils");
                has_debug_utils = true;
                spargel_log_info("use instance extension VK_EXT_debug_utils");
            }
#if SPARGEL_IS_ANDROID
            else if (strcmp(name, "VK_KHR_android_surface") == 0) {
                use_exts.push("VK_KHR_android_surface");
                has_android_surface = true;
                spargel_log_info("use instance extension VK_KHR_android_surface");
            }
#endif
#if SPARGEL_IS_LINUX
            else if (strcmp(name, "VK_KHR_xcb_surface") == 0) {
                use_exts.push("VK_KHR_xcb_surface");
                has_xcb_surface = true;
                spargel_log_info("use instance extension VK_KHR_xcb_surface");
            } else if (strcmp(name, "VK_KHR_wayland_surface") == 0) {
                use_exts.push("VK_KHR_wayland_surface");
                has_wayland_surface = true;
                spargel_log_info("use instance extension VK_KHR_wayland_surface");
            }
#endif
#if SPARGEL_IS_MACOS
            else if (strcmp(name, "VK_EXT_metal_surface") == 0) {
                use_exts.push("VK_EXT_metal_surface");
                has_metal_surface = true;
                spargel_log_info("use instance extension VK_EXT_metal_surface");
            }
#endif
#if SPARGEL_IS_WINDOWS
            else if (strcmp(name, "VK_KHR_win32_surface") == 0) {
                use_exts.push("VK_KHR_win32_surface");
                has_win32_surface = true;
                spargel_log_info("use instance extension VK_KHR_win32_surface");
            }
#endif
        }
        if (!has_surface) {
            spargel_log_fatal("VK_KHR_surface is required");
            spargel_panic_here();
        }
#if SPARGEL_IS_ANDROID
        if (!has_android_surface) {
            spargel_log_fatal("VK_KHR_android_surface is required");
            spargel_panic_here();
        }
#endif
#if SPARGEL_IS_LINUX
        if (descriptor->platform == ui::platform_kind::xcb && !has_xcb_surface) {
            spargel_log_fatal("VK_KHR_xcb_surface is required");
            spargel_panic_here();
        }
        if (descriptor->platform == ui::platform_kind::wayland && !has_wayland_surface) {
            spargel_log_fatal("VK_KHR_wayland_surface is required");
            spargel_panic_here();
        }
#endif
#if SPARGEL_IS_MACOS
        if (!has_metal_surface) {
            spargel_log_fatal("VK_EXT_metal_surface is required");
            spargel_panic_here();
        }
#endif
#if SPARGEL_IS_WINDOWS
        if (!has_win32_surface) {
            spargel_log_fatal("VK_KHR_win32_surface is required");
            spargel_panic_here();
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
            info.enabledLayerCount = use_layers.count();
            info.ppEnabledLayerNames = use_layers.data();
            info.enabledExtensionCount = use_exts.count();
            info.ppEnabledExtensionNames = use_exts.data();

            /**
             * VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR specifies that the instance will
             * enumerate available Vulkan Portability-compliant physical devices and groups in
             * addition to the Vulkan physical devices and groups that are enumerated by default.
             */
            if (has_portability) {
                info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
            }

            CHECK_VK_RESULT(procs->vkCreateInstance(&info, NULL, &instance));
            d->instance = instance;
        }

#define VULKAN_INSTANCE_PROC(name) procs->name = (PFN_##name)vkGetInstanceProcAddr(instance, #name);
#include <spargel/gpu/vulkan_procs.inc>

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
            info.pfnUserCallback = vulkan_debug_messenger_callback;
            info.pUserData = 0;

            CHECK_VK_RESULT(
                procs->vkCreateDebugUtilsMessengerEXT(instance, &info, 0, &d->debug_messenger));
        }

        /* step 7. enumerate physical devices */

        base::vector<VkPhysicalDevice> adapters;
        {
            u32 count;
            CHECK_VK_RESULT(procs->vkEnumeratePhysicalDevices(instance, &count, NULL));
            adapters.reserve(count);
            CHECK_VK_RESULT(procs->vkEnumeratePhysicalDevices(instance, &count, adapters.data()));
            adapters.set_count(count);
        }

        /* step 8. choose a physical device */
        /**
         * Spec:
         *
         * The general expectation is that a physical device groups all queues of matching
         * capabilities into a single family. However, while implementations should do this, it is
         * possible that a physical device may return two separate queue families with the same
         * capabilities.
         *
         *
         * If an implementation exposes any queue family that supports graphics operations, at least
         * one queue family of at least one physical device exposed by the implementation must
         * support both graphics and compute operations.
         *
         *
         * All commands that are allowed on a queue that supports transfer operations are also
         * allowed on a queue that supports either graphics or compute operations. Thus, if the
         * capabilities of a queue family include VK_QUEUE_GRAPHICS_BIT or VK_QUEUE_COMPUTE_BIT,
         * then reporting the VK_QUEUE_TRANSFER_BIT capability separately for that queue family is
         * optional.
         */
        /**
         * We need one graphics queue. So there exists one queue family that supports both graphics
         * and compute.
         */

        base::vector<VkQueueFamilyProperties> queue_families;

        VkPhysicalDevice adapter;
        ssize queue_family_index;

        for (usize i = 0; i < adapters.count(); i++) {
            queue_families.clear();

            adapter = adapters[i];

            VkPhysicalDeviceProperties prop;
            procs->vkGetPhysicalDeviceProperties(adapter, &prop);
            spargel_log_info("adapter # %ld %s", i, prop.deviceName);

            /* step 8.1. check queue families */
            /* We use only one queue family for now. */
            {
                u32 count;
                procs->vkGetPhysicalDeviceQueueFamilyProperties(adapter, &count, 0);
                queue_families.reserve(count);
                procs->vkGetPhysicalDeviceQueueFamilyProperties(adapter, &count,
                                                                queue_families.data());
                queue_families.set_count(count);
            }
            queue_family_index = -1;
            for (usize j = 0; j < queue_families.count(); j++) {
                VkQueueFamilyProperties* prop = &queue_families[j];
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

        /* step 9. enumerate device extensions */
        all_exts.clear();
        {
            u32 count;
            CHECK_VK_RESULT(procs->vkEnumerateDeviceExtensionProperties(adapter, 0, &count, 0));
            all_exts.reserve(count);
            CHECK_VK_RESULT(
                procs->vkEnumerateDeviceExtensionProperties(adapter, 0, &count, all_exts.data()));
            all_exts.set_count(count);
        }
        for (usize i = 0; i < all_exts.count(); i++) {
            spargel_log_info("device extension #%ld = %s", i, all_exts[i].extensionName);
        }

        /* step 10. choose device extensions */

        use_exts.clear();

        bool has_swapchain = false;

        for (usize i = 0; i < all_exts.count(); i++) {
            struct VkExtensionProperties* prop = &all_exts[i];
            if (strcmp(prop->extensionName, "VK_KHR_swapchain") == 0) {
                use_exts.push("VK_KHR_swapchain");
                has_swapchain = true;
                spargel_log_info("use instance extension VK_KHR_surface");
            } else if (strcmp(prop->extensionName, "VK_KHR_portability_subset") == 0) {
                use_exts.push("VK_KHR_portability_subset");
                spargel_log_info("use instance extension VK_KHR_portability_subset");
            }
        }

        if (!has_swapchain) {
            spargel_log_fatal("VK_KHR_swapchain is required");
            spargel_panic_here();
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
            queue_info.pQueuePriorities = vulkan_queue_priorities;

            VkDeviceCreateInfo info;
            info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            info.pNext = 0;
            info.flags = 0;
            info.queueCreateInfoCount = 1;
            info.pQueueCreateInfos = &queue_info;
            info.enabledLayerCount = 0;
            info.ppEnabledLayerNames = 0;
            info.enabledExtensionCount = use_exts.count();
            info.ppEnabledExtensionNames = use_exts.data();
            info.pEnabledFeatures = 0;

            CHECK_VK_RESULT(procs->vkCreateDevice(adapter, &info, 0, &dev));
            d->device = dev;
        }

        PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr = procs->vkGetDeviceProcAddr;
#define VULKAN_DEVICE_PROC(name) procs->name = (PFN_##name)vkGetDeviceProcAddr(dev, #name);
#include <spargel/gpu/vulkan_procs.inc>

        d->queue.backend = BACKEND_VULKAN;

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

        *device = (device_id)d;
        return RESULT_SUCCESS;
    }

    void vulkan_destroy_device(device_id device) {
        cast_object(vulkan_device, d, device);
        struct vulkan_proc_table* procs = &d->procs;

        procs->vkDestroyDevice(d->device, 0);
        if (d->debug_messenger)
            procs->vkDestroyDebugUtilsMessengerEXT(d->instance, d->debug_messenger, 0);
        procs->vkDestroyInstance(d->instance, 0);

#if SPARGEL_IS_POSIX
        dlclose(d->library);
#elif SPARGEL_IS_WINDOWS
        FreeLibrary(d->library);
#else
#error unimplemented
#endif
        dealloc_object(vulkan_device, d);
    }

    int vulkan_create_command_queue(device_id device, command_queue_id* queue) {
        cast_object(vulkan_device, d, device);
        *queue = (command_queue_id)&d->queue;
        return RESULT_SUCCESS;
    }

    void vulkan_destroy_command_queue(device_id device, command_queue_id queue) {}

    int vulkan_create_shader_function(device_id device,
                                      struct vulkan_shader_function_descriptor const* descriptor,
                                      shader_function_id* func) {
        cast_object(vulkan_device, d, device);
        alloc_object(vulkan_shader_function, f);

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

        *func = (shader_function_id)f;
        return RESULT_SUCCESS;
    }

    void vulkan_destroy_shader_function(device_id device, shader_function_id func) {
        cast_object(vulkan_shader_function, f, func);
        struct vulkan_device* d = f->device;
        d->procs.vkDestroyShaderModule(d->device, f->shader, 0);
        dealloc_object(vulkan_shader_function, f);
    }

    int vulkan_create_render_pipeline(device_id device,
                                      struct render_pipeline_descriptor const* descriptor,
                                      render_pipeline_id* pipeline) {
        alloc_object(vulkan_render_pipeline, p);

        VkGraphicsPipelineCreateInfo pipeline_info;
        pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.pNext = 0;
        pipeline_info.flags = 0;
        pipeline_info.stageCount = 2;

        *pipeline = (render_pipeline_id)p;
        return RESULT_SUCCESS;
    }

    void vulkan_destroy_render_pipeline(device_id device, render_pipeline_id pipeline) {
        cast_object(vulkan_render_pipeline, p, pipeline);
        dealloc_object(vulkan_render_pipeline, p);
    }

    int vulkan_create_command_buffer(device_id device,
                                     struct command_buffer_descriptor const* descriptor,
                                     command_buffer_id* command_buffer) {
        cast_object(vulkan_device, d, device);
        cast_object(vulkan_command_queue, q, descriptor->queue);
        alloc_object(vulkan_command_buffer, cmdbuf);

        VkCommandBufferAllocateInfo info;
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.pNext = 0;
        info.commandPool = d->cmd_pool;
        info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        info.commandBufferCount = 1;
        CHECK_VK_RESULT(
            d->procs.vkAllocateCommandBuffers(d->device, &info, &cmdbuf->command_buffer));

        cmdbuf->queue = q->queue;

        *command_buffer = (command_buffer_id)cmdbuf;
        return RESULT_SUCCESS;
    }

    void vulkan_destroy_command_buffer(device_id device, command_buffer_id command_buffer) {
        cast_object(vulkan_command_buffer, c, command_buffer);
        dealloc_object(vulkan_command_buffer, c);
    }

    void vulkan_reset_command_buffer(device_id device, command_buffer_id command_buffer) {
        cast_object(vulkan_device, d, device);
        cast_object(vulkan_command_buffer, c, command_buffer);
        CHECK_VK_RESULT(d->procs.vkResetCommandBuffer(c->command_buffer, 0));
    }

    int vulkan_create_surface(device_id device, struct surface_descriptor const* descriptor,
                              surface_id* surface) {
        cast_object(vulkan_device, d, device);
        alloc_object(vulkan_surface, s);

        spargel::ui::window_handle wh = descriptor->window->handle();
        VkSurfaceKHR surf;
#if SPARGEL_IS_ANDROID
        VkAndroidSurfaceCreateInfoKHR info;
        info.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
        info.pNext = 0;
        info.flags = 0;
        info.window = (ANativeWindow*)wh.android.window;
        CHECK_VK_RESULT(d->procs.vkCreateAndroidSurfaceKHR(d->instance, &info, 0, &surf));
#endif
#if SPARGEL_IS_LINUX /* todo: wayland */
        VkXcbSurfaceCreateInfoKHR info;
        info.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
        info.pNext = 0;
        info.flags = 0;
        info.connection = (xcb_connection_t*)wh.xcb.connection;
        info.window = wh.xcb.window;
        CHECK_VK_RESULT(d->procs.vkCreateXcbSurfaceKHR(d->instance, &info, 0, &surf));
#endif
#if SPARGEL_IS_MACOS
        VkMetalSurfaceCreateInfoEXT info;
        info.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
        info.pNext = 0;
        info.flags = 0;
        info.pLayer = wh.apple.layer;
        CHECK_VK_RESULT(d->procs.vkCreateMetalSurfaceEXT(d->instance, &info, 0, &surf));
#endif
#if SPARGEL_IS_WINDOWS
        VkWin32SurfaceCreateInfoKHR info;
        info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        info.pNext = 0;
        info.flags = 0;
        info.hinstance = wh.win32.hinstance;
        info.hwnd = wh.win32.hwnd;
        CHECK_VK_RESULT(d->procs.vkCreateWin32SurfaceKHR(d->instance, &info, 0, &surf));
#endif

        s->surface = surf;

        *surface = (surface_id)s;

        return RESULT_SUCCESS;
    }

    void vulkan_destroy_surface(device_id device, surface_id surface) {
        cast_object(vulkan_device, d, device);
        cast_object(vulkan_surface, s, surface);
        d->procs.vkDestroySurfaceKHR(d->instance, s->surface, 0);
        dealloc_object(vulkan_surface, s);
    }

    int vulkan_create_swapchain(device_id device, struct swapchain_descriptor const* descriptor,
                                swapchain_id* swapchain) {
        cast_object(vulkan_device, d, device);
        cast_object(vulkan_surface, sf, descriptor->surface);
        alloc_object(vulkan_swapchain, sw);

        struct vulkan_proc_table* procs = &d->procs;

        VkSurfaceCapabilitiesKHR surf_caps;
        CHECK_VK_RESULT(procs->vkGetPhysicalDeviceSurfaceCapabilitiesKHR(d->physical_device,
                                                                         sf->surface, &surf_caps));

        u32 min_images = surf_caps.minImageCount + 1;
        if (surf_caps.maxImageCount > 0 && min_images > surf_caps.maxImageCount) {
            min_images = surf_caps.maxImageCount;
        }

        base::vector<VkSurfaceFormatKHR> formats;
        {
            u32 count;
            CHECK_VK_RESULT(procs->vkGetPhysicalDeviceSurfaceFormatsKHR(d->physical_device,
                                                                        sf->surface, &count, 0));
            formats.reserve(count);
            CHECK_VK_RESULT(procs->vkGetPhysicalDeviceSurfaceFormatsKHR(
                d->physical_device, sf->surface, &count, formats.data()));
            formats.set_count(count);
        }

        VkSurfaceFormatKHR* chosen_format = &formats[0];
        for (usize i = 0; i < formats.count(); i++) {
            VkSurfaceFormatKHR* fmt = &formats[i];
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
            sw->images.reserve(count);
            sw->image_views.reserve(count);
            sw->framebuffers.reserve(count);
            CHECK_VK_RESULT(procs->vkGetSwapchainImagesKHR(d->device, sw->swapchain, &count,
                                                           sw->images.data()));
            sw->images.set_count(count);
            sw->image_views.set_count(count);
            sw->framebuffers.set_count(count);
        }

        auto image_count = sw->images.count();

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
            for (usize i = 0; i < image_count; i++) {
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
            for (usize i = 0; i < image_count; i++) {
                info.pAttachments = &sw->image_views[i];
                CHECK_VK_RESULT(
                    procs->vkCreateFramebuffer(d->device, &info, 0, &sw->framebuffers[i]));
            }
        }

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

        *swapchain = (swapchain_id)sw;
        return RESULT_SUCCESS;
    }

    void vulkan_destroy_swapchain(device_id device, swapchain_id swapchain) {
        cast_object(vulkan_device, d, device);
        cast_object(vulkan_swapchain, s, swapchain);

        struct vulkan_proc_table* procs = &d->procs;

        for (auto f : s->framebuffers) {
            procs->vkDestroyFramebuffer(d->device, f, 0);
        }
        for (auto iv : s->image_views) {
            procs->vkDestroyImageView(d->device, iv, 0);
        }
        procs->vkDestroyRenderPass(d->device, s->render_pass, 0);
        procs->vkDestroySwapchainKHR(d->device, s->swapchain, 0);
        dealloc_object(vulkan_swapchain, s);
    }

    int vulkan_acquire_image(device_id device, struct acquire_descriptor const* descriptor,
                             presentable_id* presentable) {
        cast_object(vulkan_device, d, device);
        cast_object(vulkan_swapchain, s, descriptor->swapchain);

        /**
         * Spec:
         *
         * After acquiring a presentable image and before modifying it, the application must use a
         * synchronization primitive to ensure that the presentation engine has finished reading
         * from the image.
         *
         * The presentation engine may not have finished reading from the image at the time it is
         * acquired, so the application must use semaphore and/or fence to ensure that the image
         * layout and contents are not modified until the presentation engine reads have completed.
         */
        int result = d->procs.vkAcquireNextImageKHR(d->device, s->swapchain, UINT64_MAX, 0,
                                                    s->image_available, &s->presentable.index);
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            spargel_log_fatal("cannot acquire next swapchain image");
            spargel_panic_here();
        }

        CHECK_VK_RESULT(
            d->procs.vkWaitForFences(d->device, 1, &s->image_available, VK_TRUE, UINT64_MAX));
        CHECK_VK_RESULT(d->procs.vkResetFences(d->device, 1, &s->image_available));

        s->presentable.swapchain = s;

        *presentable = (presentable_id)&s->presentable;
        return RESULT_SUCCESS;
    }

    void vulkan_begin_render_pass(device_id device, struct render_pass_descriptor const* descriptor,
                                  render_pass_encoder_id* encoder) {
        cast_object(vulkan_device, d, device);
        cast_object(vulkan_command_buffer, cmdbuf, descriptor->command_buffer);
        cast_object(vulkan_swapchain, sw, descriptor->swapchain);
        alloc_object(vulkan_render_pass_encoder, e);

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

            d->procs.vkCmdBeginRenderPass(cmdbuf->command_buffer, &info,
                                          VK_SUBPASS_CONTENTS_INLINE);
        }

        e->command_buffer = cmdbuf->command_buffer;

        *encoder = (render_pass_encoder_id)e;
    }

    void vulkan_end_render_pass(device_id device, render_pass_encoder_id encoder) {
        cast_object(vulkan_device, d, device);
        cast_object(vulkan_render_pass_encoder, e, encoder);
        d->procs.vkCmdEndRenderPass(e->command_buffer);
        CHECK_VK_RESULT(d->procs.vkEndCommandBuffer(e->command_buffer));
        dealloc_object(vulkan_render_pass_encoder, e);
    }

    void vulkan_present(device_id device, struct present_descriptor const* descriptor) {
        cast_object(vulkan_device, d, device);
        cast_object(vulkan_command_buffer, c, descriptor->command_buffer);
        cast_object(vulkan_presentable, p, descriptor->presentable);
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
         * Calls to vkQueuePresentKHR may block, but must return in finite time. The processing of
         * the presentation happens in issue order with other queue operations, but semaphores must
         * be used to ensure that prior rendering and other commands in the specified queue complete
         * before the presentation begins. The presentation command itself does not delay processing
         * of subsequent commands on the queue. However, presentation requests sent to a particular
         * queue are always performed in order. Exact presentation timing is controlled by the
         * semantics of the presentation engine and native platform in use.
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
                spargel_log_fatal("cannot acquire next swapchain image");
                spargel_panic_here();
            }
        }
        /* todo */
        CHECK_VK_RESULT(d->procs.vkWaitForFences(d->device, 1, &p->swapchain->submit_done, VK_TRUE,
                                                 UINT64_MAX));
        CHECK_VK_RESULT(d->procs.vkResetFences(d->device, 1, &p->swapchain->submit_done));
    }

    void vulkan_presentable_texture(device_id device, presentable_id presentable,
                                    texture_id* texture) {
        *texture = 0;
    }

}  // namespace spargel::gpu
