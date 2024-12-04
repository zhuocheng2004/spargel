#import <Metal/Metal.h>
#include <spargel/gpu/gpu.h>
#include <spargel/gpu/operations.h>
#include <stdlib.h>

#define alloc_object(type, name)                                   \
    struct type* name = (struct type*)malloc(sizeof(struct type)); \
    if (!name) return SGPU_RESULT_ALLOCATION_FAILED;               \
    name->_ops = &sgpu_mtl_operations;

#define cast_object(type, name, object) struct type* name = (struct type*)(object);

extern const struct sgpu_operations sgpu_mtl_operations;

/**
 * We only target Apple Silicon.
 * This in particular guarantees that there is only one GPU.
 */

struct device {
    const struct sgpu_operations* _ops;
    id<MTLDevice> device;
};

struct command_queue {
    const struct sgpu_operations* _ops;
    id<MTLCommandQueue> queue;
};

struct shader_library {
    const struct sgpu_operations* _ops;
    id<MTLLibrary> library;
};

struct shader_function {
    const struct sgpu_operations* _ops;
    id<MTLFunction> function;
};

static int create_default_device(sgpu_device_id* device)
{
    alloc_object(device, d);
    d->device = MTLCreateSystemDefaultDevice();
    *device = (sgpu_device_id)d;
    return SGPU_RESULT_SUCCESS;
}

static void destroy_device(sgpu_device_id device)
{
    cast_object(device, d, device);
    [d->device release];
    free(device);
}

static int create_command_queue(sgpu_device_id device, sgpu_command_queue_id* queue)
{
    alloc_object(command_queue, q);
    cast_object(device, d, device);
    q->queue = [d->device newCommandQueue];
    *queue = (sgpu_command_queue_id)q;
    return SGPU_RESULT_SUCCESS;
}

static void destroy_command_queue(sgpu_command_queue_id command_queue)
{
    cast_object(command_queue, q, command_queue);
    [q->queue release];
    free(q);
}

static int create_metal_shader_library(
    sgpu_device_id device, struct sgpu_metal_shader_library_descriptor const* descriptor,
    sgpu_metal_shader_library_id* library)
{
    alloc_object(shader_library, lib);
    cast_object(device, d, device);

    dispatch_data_t lib_data =
        dispatch_data_create(descriptor->code, descriptor->size,
                             dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),
                             DISPATCH_DATA_DESTRUCTOR_DEFAULT);

    NSError* error;
    lib->library = [d->device newLibraryWithData:lib_data error:&error];
    if (lib->library == nil) {
        free(lib);
        return SGPU_RESULT_CANNOT_CREATE_METAL_LIBRARY;
    }
    *library = (sgpu_metal_shader_library_id)lib;
    return SGPU_RESULT_SUCCESS;
}

static void destroy_metal_shader_library(sgpu_metal_shader_library_id shader_library)
{
    cast_object(shader_library, lib, shader_library);
    [lib->library release];
    free(lib);
}

static int create_shader_function(sgpu_device_id device,
                                  struct sgpu_shader_function_descriptor const* descriptor,
                                  sgpu_shader_function_id* func)
{
    alloc_object(shader_function, f);
    cast_object(shader_library, lib, descriptor->metal.library);

    f->function =
        [lib->library newFunctionWithName:[NSString stringWithUTF8String:descriptor->metal.name]];
    if (f->function == nil) {
        free(f);
        return SGPU_RESULT_CANNOT_CREATE_SHADER_FUNCTION;
    }
    *func = (sgpu_shader_function_id)f;
    return SGPU_RESULT_SUCCESS;
}

static void destroy_shader_function(sgpu_shader_function_id func)
{
    cast_object(shader_function, f, func);
    [f->function release];
    free(f);
}

const struct sgpu_operations sgpu_mtl_operations = {
    .create_default_device = create_default_device,
    .destroy_device = destroy_device,
    .create_command_queue = create_command_queue,
    .destroy_command_queue = destroy_command_queue,
    .create_shader_function = create_shader_function,
    .destroy_shader_function = destroy_shader_function,
    .create_metal_shader_library = create_metal_shader_library,
    .destroy_metal_shader_library = destroy_metal_shader_library,
};
