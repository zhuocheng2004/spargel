#include <spargel/base/base.h>
#include <spargel/gpu/gpu_vulkan.h>

/* libc */
#include <stdlib.h>
#include <string.h>

/* platform */
#include <dlfcn.h>

#define VK_NO_PROTOTYPES
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#if defined(__APPLE__)
#include <vulkan/vulkan_metal.h>
#endif

#define alloc_object(type, name)                                                   \
    struct type* name = sbase_allocate(sizeof(struct type), SBASE_ALLOCATION_GPU); \
    if (!name) return SGPU_RESULT_ALLOCATION_FAILED;                               \
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

static int sgpu_vulkan_create_instance(struct sgpu_vulkan_device* device) {
    struct sgpu_vulkan_proc_table* procs = &device->procs;

    /* step 1. enumerate layers */
    struct array all_layers;
    init_array(&all_layers, sizeof(struct VkLayerProperties));

    u32 count;
    CHECK_VK_RESULT(procs->vkEnumerateInstanceLayerProperties(&count, NULL));
    array_reserve(&all_layers, count);
    CHECK_VK_RESULT(procs->vkEnumerateInstanceLayerProperties(&count, all_layers.data));
    all_layers.count = count;

    for (ssize i = 0; i < all_layers.count; i++) {
        sbase_log_info("layer #%ld = %s", i,
                       ((struct VkLayerProperties*)array_at(all_layers, i))->layerName);
    }

    /* step 2. enumerate instance extensions */
    struct array all_inst_exts;
    init_array(&all_inst_exts, sizeof(struct VkExtensionProperties));
    CHECK_VK_RESULT(procs->vkEnumerateInstanceExtensionProperties(NULL, &count, NULL));
    array_reserve(&all_inst_exts, count);
    CHECK_VK_RESULT(
        procs->vkEnumerateInstanceExtensionProperties(NULL, &count, all_inst_exts.data));
    all_inst_exts.count = count;

    for (ssize i = 0; i < all_inst_exts.count; i++) {
        sbase_log_info("instance extension #%ld = %s", i,
                       ((struct VkExtensionProperties*)array_at(all_inst_exts, i))->extensionName);
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
    struct array use_inst_exts;
    init_array(&use_inst_exts, sizeof(char const*));
    bool has_surface = false;
    bool has_portability = false;
    bool has_debug_report = false;
#if SPARGEL_IS_MACOS
    bool has_metal_surface = false;
#endif
    for (ssize i = 0; i < all_inst_exts.count; i++) {
        char const* name =
            ((struct VkExtensionProperties*)array_at(all_inst_exts, i))->extensionName;
        if (strcmp(name, "VK_KHR_surface") == 0) {
            char const** ptr = array_push(&use_inst_exts);
            *ptr = "VK_KHR_surface";
            has_surface = true;
            sbase_log_info("use instance extension VK_KHR_surface");
        } else if (strcmp(name, "VK_KHR_portability_enumeration") == 0) {
            char const** ptr = array_push(&use_inst_exts);
            *ptr = "VK_KHR_portability_enumeration";
            has_portability = true;
            sbase_log_info("use instance extension VK_KHR_portability_enumeration");
        } else if (strcmp(name, "VK_EXT_debug_utils") == 0) {
            char const** ptr = array_push(&use_inst_exts);
            *ptr = "VK_EXT_debug_utils";
            has_debug_report = true;
            sbase_log_info("use instance extension VK_EXT_debug_utils");
        }
#if SPARGEL_IS_MACOS
        else if (strcmp(name, "VK_EXT_metal_surface") == 0) {
            char const** ptr = array_push(&use_inst_exts);
            *ptr = "VK_EXT_metal_surface";
            has_metal_surface = true;
            sbase_log_info("use instance extension VK_EXT_metal_surface");
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

    VkInstance instance = NULL;

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
    info.enabledExtensionCount = use_inst_exts.count;
    info.ppEnabledExtensionNames = use_inst_exts.data;

    if (has_portability) {
        info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    }

    CHECK_VK_RESULT(device->procs.vkCreateInstance(&info, NULL, &instance));
    device->instance = instance;

    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = device->procs.vkGetInstanceProcAddr;
#define VULKAN_INSTANCE_PROC(name) \
    device->procs.name = (PFN_##name)vkGetInstanceProcAddr(instance, #name);
#include <spargel/gpu/vulkan_procs.inc>

    deinit_array(use_inst_exts);
    deinit_array(use_layers);
    deinit_array(all_inst_exts);
    deinit_array(all_layers);

    return SGPU_RESULT_SUCCESS;
}

int sgpu_vulkan_create_default_device(sgpu_device_id* device) {
    alloc_object(sgpu_vulkan_device, d);
    d->library = dlopen("libvulkan.dylib", RTLD_NOW | RTLD_LOCAL);
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
    dlclose(d->library);
    dealloc_object(sgpu_vulkan_device, d);

    sbase_panic_here();
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
