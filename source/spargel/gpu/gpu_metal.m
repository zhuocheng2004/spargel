#include <spargel/gpu/gpu_metal.h>
// #include <spargel/gpu/operations.h>

/* libc */
#include <stdlib.h>

/* platform */
#import <Metal/Metal.h>

#define alloc_object(type, name)                     \
    struct type* name = malloc(sizeof(struct type)); \
    if (!name) return SGPU_RESULT_ALLOCATION_FAILED; \
    name->backend = SGPU_BACKEND_METAL;

#define cast_object(type, name, object) struct type* name = (struct type*)(object);

/**
 * We only target Apple Silicon.
 * This in particular guarantees that there is only one GPU.
 */

struct sgpu_metal_device {
    int backend;
    id<MTLDevice> device;
};

struct sgpu_metal_command_queue {
    int backend;
    id<MTLCommandQueue> queue;
};

struct sgpu_metal_shader_library {
    int backend;
    id<MTLLibrary> library;
};

struct sgpu_metal_shader_function {
    int backend;
    id<MTLFunction> function;
};

int sgpu_metal_create_default_device(sgpu_device_id* device) {
    alloc_object(sgpu_metal_device, d);
    d->device = MTLCreateSystemDefaultDevice();
    *device = (sgpu_device_id)d;
    return SGPU_RESULT_SUCCESS;
}

void sgpu_metal_destroy_device(sgpu_device_id device) {
    cast_object(sgpu_metal_device, d, device);
    [d->device release];
    free(device);
}

int sgpu_metal_create_command_queue(sgpu_device_id device, sgpu_command_queue_id* queue) {
    alloc_object(sgpu_metal_command_queue, q);
    cast_object(sgpu_metal_device, d, device);
    q->queue = [d->device newCommandQueue];
    *queue = (sgpu_command_queue_id)q;
    return SGPU_RESULT_SUCCESS;
}

void sgpu_metal_destroy_command_queue(sgpu_command_queue_id command_queue) {
    cast_object(sgpu_metal_command_queue, q, command_queue);
    [q->queue release];
    free(q);
}

int sgpu_metal_create_shader_library(sgpu_device_id device,
                                     struct sgpu_metal_shader_library_descriptor const* descriptor,
                                     sgpu_metal_shader_library_id* library) {
    alloc_object(sgpu_metal_shader_library, lib);
    cast_object(sgpu_metal_device, d, device);

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

void sgpu_metal_destroy_shader_library(sgpu_metal_shader_library_id shader_library) {
    cast_object(sgpu_metal_shader_library, lib, shader_library);
    [lib->library release];
    free(lib);
}

int sgpu_metal_create_shader_function(
    sgpu_device_id device, struct sgpu_metal_shader_function_descriptor const* descriptor,
    sgpu_shader_function_id* func) {
    alloc_object(sgpu_metal_shader_function, f);
    cast_object(sgpu_metal_shader_library, lib, descriptor->library);

    f->function =
        [lib->library newFunctionWithName:[NSString stringWithUTF8String:descriptor->name]];
    if (f->function == nil) {
        free(f);
        return SGPU_RESULT_CANNOT_CREATE_SHADER_FUNCTION;
    }
    *func = (sgpu_shader_function_id)f;
    return SGPU_RESULT_SUCCESS;
}

void sgpu_metal_destroy_shader_function(sgpu_shader_function_id func) {
    cast_object(sgpu_metal_shader_function, f, func);
    [f->function release];
    free(f);
}

int sgpu_metal_create_render_pipeline(sgpu_device_id device,
                                      struct sgpu_render_pipeline_descriptor const* descriptor,
                                      sgpu_render_pipeline_id* pipeline) {
    sbase_panic_here();
}

void sgpu_metal_destroy_render_pipeline(sgpu_render_pipeline_id pipeline) { sbase_panic_here(); }

int sgpu_metal_create_command_buffer(sgpu_command_queue_id queue,
                                     sgpu_command_buffer_id* command_buffer) {
    sbase_panic_here();
}

void sgpu_metal_destroy_command_buffer(sgpu_command_buffer_id command_buffer) {
    sbase_panic_here();
}
