#include <spargel/base/base.h>
#include <spargel/gpu/gpu_vulkan.h>

/* libc */
#include <stdlib.h>

/* platform */
#include <dlfcn.h>

#define VK_NO_PROTOTYPES
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#if defined(__APPLE__)
#include <vulkan/vulkan_metal.h>
#endif

#define alloc_object(type, name)                                   \
    struct type* name = (struct type*)malloc(sizeof(struct type)); \
    if (!name) return SGPU_RESULT_ALLOCATION_FAILED;               \
    name->backend = SGPU_BACKEND_VULKAN;

#define cast_object(type, name, object) struct type* name = (struct type*)(object);

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
        }                                                              \
    } while (0)

static int sgpu_vulkan_create_instance(struct sgpu_vulkan_device* device) {
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
    info.enabledLayerCount = 0;
    info.ppEnabledLayerNames = NULL;
    info.enabledExtensionCount = 0;
    info.ppEnabledExtensionNames = NULL;

    CHECK_VK_RESULT(device->procs.vkCreateInstance(&info, NULL, &instance));

    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = device->procs.vkGetInstanceProcAddr;
#define VULKAN_INSTANCE_PROC(name) \
    device->procs.name = (PFN_##name)vkGetInstanceProcAddr(instance, #name);
#include <spargel/gpu/vulkan_procs.inc>

    return SGPU_RESULT_SUCCESS;
}

int sgpu_vulkan_create_default_device(sgpu_device_id* device) {
    alloc_object(sgpu_vulkan_device, d);
    d->library = dlopen("libvulkan.dylib", RTLD_NOW | RTLD_LOCAL);
    if (d->library == NULL) {
        free(d);
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
