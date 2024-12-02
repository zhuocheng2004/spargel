// #include <spargel/base/base.h>
#include <spargel/gpu/gpu.h>
#include <spargel/gpu/operations.h>
#include <stdlib.h>

#define VK_NO_PROTOTYPES
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#if defined(__APPLE__)
#include <vulkan/vulkan_metal.h>
#endif

import spargel.base.c;

#define alloc_object(type, name)                                   \
    struct type* name = (struct type*)malloc(sizeof(struct type)); \
    if (!name) return SGPU_RESULT_ALLOCATION_FAILED;               \
    name->_ops = &sgpu_vk_operations;

extern const struct sgpu_operations sgpu_vk_operations;

static int create_default_device(sgpu_device_id* device)
{
    sbase_unreachable();
}

static void destroy_device(sgpu_device_id device)
{
    sbase_unreachable();
}

const struct sgpu_operations sgpu_vk_operations = {
    .create_default_device = create_default_device,
    .destroy_device = destroy_device,
};
