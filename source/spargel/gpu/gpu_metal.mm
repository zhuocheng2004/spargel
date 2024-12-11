#include <spargel/gpu/gpu_metal.h>
#include <spargel/ui/ui_mac.h>

/* libc */
#include <stdlib.h>

/* platform */
#import <Metal/Metal.h>

#define alloc_object(type, name)                                                                 \
    struct type* name = (struct type*)sbase_allocate(sizeof(struct type), SBASE_ALLOCATION_GPU); \
    if (!name) sbase_panic_here();

#define cast_object(type, name, object) struct type* name = (struct type*)(object);

#define dealloc_object(type, name) \
    sbase_deallocate(name, sizeof(struct type), SBASE_ALLOCATION_GPU);

/**
 * We only target Apple Silicon.
 * This in particular guarantees that there is only one GPU.
 */

struct sgpu_metal_command_queue {
    id<MTLCommandQueue> queue;
};

struct sgpu_metal_command_buffer {
    id<MTLCommandQueue> queue;
    id<MTLCommandBuffer> buffer;
};

struct sgpu_metal_shader_library {
    id<MTLLibrary> library;
};

struct sgpu_metal_shader_function {
    id<MTLFunction> function;
};

struct sgpu_metal_render_pipeline {
    id<MTLRenderPipelineState> pipeline;
};

struct sgpu_metal_surface {
    CAMetalLayer* layer;
};

struct sgpu_metal_swapchain {
    CAMetalLayer* layer;
};

struct sgpu_metal_texture {
    id<MTLTexture> texture;
};

struct sgpu_metal_presentable {
    id<CAMetalDrawable> drawable;
    struct sgpu_metal_texture texture;
};

struct sgpu_metal_render_pass_encoder {
    id<MTLRenderCommandEncoder> encoder;
};

struct sgpu_metal_device {
    int backend;
    id<MTLDevice> device;
    struct sgpu_metal_presentable presentable;
};

int sgpu_metal_create_default_device(struct sgpu_device_descriptor const* descriptor,
                                     sgpu_device_id* device) {
    alloc_object(sgpu_metal_device, d);
    d->device = MTLCreateSystemDefaultDevice();
    d->backend = SGPU_BACKEND_METAL;
    *device = (sgpu_device_id)d;
    return SGPU_RESULT_SUCCESS;
}

void sgpu_metal_destroy_device(sgpu_device_id device) {
    cast_object(sgpu_metal_device, d, device);
    [d->device release];
    dealloc_object(sgpu_metal_device, d);
}

int sgpu_metal_create_command_queue(sgpu_device_id device, sgpu_command_queue_id* queue) {
    alloc_object(sgpu_metal_command_queue, q);
    cast_object(sgpu_metal_device, d, device);
    q->queue = [d->device newCommandQueue];
    *queue = (sgpu_command_queue_id)q;
    return SGPU_RESULT_SUCCESS;
}

void sgpu_metal_destroy_command_queue(sgpu_device_id device, sgpu_command_queue_id command_queue) {
    cast_object(sgpu_metal_command_queue, q, command_queue);
    [q->queue release];
    dealloc_object(sgpu_metal_command_queue, q);
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

void sgpu_metal_destroy_shader_library(sgpu_device_id device,
                                       sgpu_metal_shader_library_id shader_library) {
    cast_object(sgpu_metal_shader_library, lib, shader_library);
    [lib->library release];
    dealloc_object(sgpu_metal_shader_library, lib);
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

void sgpu_metal_destroy_shader_function(sgpu_device_id device, sgpu_shader_function_id func) {
    cast_object(sgpu_metal_shader_function, f, func);
    [f->function release];
    dealloc_object(sgpu_metal_shader_function, f);
}

static MTLPixelFormat sgpu_metal_translate_format(int format) {
    switch (format) {
    case SGPU_FORMAT_BRGA8_UNORM:
        return MTLPixelFormatBGRA8Unorm;
    case SGPU_FORMAT_BGRA8_SRGB:
        return MTLPixelFormatBGRA8Unorm_sRGB;
    default:
        sbase_panic_here();
    }
}

int sgpu_metal_create_render_pipeline(sgpu_device_id device,
                                      struct sgpu_render_pipeline_descriptor const* descriptor,
                                      sgpu_render_pipeline_id* pipeline) {
    cast_object(sgpu_metal_device, d, device);
    cast_object(sgpu_metal_shader_function, vs, descriptor->vertex_function);
    cast_object(sgpu_metal_shader_function, fs, descriptor->fragment_function);
    alloc_object(sgpu_metal_render_pipeline, p);

    spargel_assert(d != 0);
    spargel_assert(vs != 0);
    spargel_assert(fs != 0);

    MTLRenderPipelineDescriptor* desc = [[MTLRenderPipelineDescriptor alloc] init];
    desc.vertexFunction = vs->function;
    desc.fragmentFunction = fs->function;
    desc.colorAttachments[0].pixelFormat = sgpu_metal_translate_format(descriptor->target_format);

    NSError* err;
    id<MTLRenderPipelineState> pstate = [d->device newRenderPipelineStateWithDescriptor:desc
                                                                                  error:&err];
    [desc release];

    if (pstate == nil) {
        dealloc_object(sgpu_metal_render_pipeline, p);
        return SGPU_RESULT_CANNOT_CREATE_RENDER_PIPELINE;
    }

    p->pipeline = pstate;

    *pipeline = (sgpu_render_pipeline_id)p;

    return SGPU_RESULT_SUCCESS;
}

void sgpu_metal_destroy_render_pipeline(sgpu_device_id device, sgpu_render_pipeline_id pipeline) {
    cast_object(sgpu_metal_render_pipeline, p, pipeline);
    [p->pipeline release];
    dealloc_object(sgpu_metal_render_pipeline, p);
}

int sgpu_metal_create_command_buffer(sgpu_device_id device,
                                     struct sgpu_command_buffer_descriptor const* descriptor,
                                     sgpu_command_buffer_id* command_buffer) {
    cast_object(sgpu_metal_command_queue, q, descriptor->queue);
    alloc_object(sgpu_metal_command_buffer, cmdbuf);

    cmdbuf->queue = q->queue;
    cmdbuf->buffer = [q->queue commandBuffer];

    *command_buffer = (sgpu_command_buffer_id)cmdbuf;
    return SGPU_RESULT_SUCCESS;
}

void sgpu_metal_destroy_command_buffer(sgpu_device_id device,
                                       sgpu_command_buffer_id command_buffer) {
    cast_object(sgpu_metal_command_buffer, c, command_buffer);
    dealloc_object(sgpu_metal_command_buffer, c);
}

void sgpu_metal_reset_command_buffer(sgpu_device_id device, sgpu_command_buffer_id command_buffer) {
    cast_object(sgpu_metal_command_buffer, c, command_buffer);
    c->buffer = [c->queue commandBuffer];
}

int sgpu_metal_create_surface(sgpu_device_id device,
                              struct sgpu_surface_descriptor const* descriptor,
                              sgpu_surface_id* surface) {
    cast_object(sgpu_metal_device, d, device);
    alloc_object(sgpu_metal_surface, s);
    s->layer = descriptor->window->layer;
    [s->layer retain];
    s->layer.device = d->device;
    *surface = (sgpu_surface_id)s;
    return SGPU_RESULT_SUCCESS;
}

void sgpu_metal_destroy_surface(sgpu_device_id device, sgpu_surface_id surface) {
    cast_object(sgpu_metal_surface, s, surface);
    [s->layer release];
    dealloc_object(sgpu_metal_surface, s);
}

int sgpu_metal_create_swapchain(sgpu_device_id device,
                                struct sgpu_swapchain_descriptor const* descriptor,
                                sgpu_swapchain_id* swapchain) {
    cast_object(sgpu_metal_surface, s, descriptor->surface);
    alloc_object(sgpu_metal_swapchain, sc);
    sc->layer = s->layer;
    *swapchain = (sgpu_swapchain_id)sc;
    return SGPU_RESULT_SUCCESS;
}

void sgpu_metal_destroy_swapchain(sgpu_device_id device, sgpu_swapchain_id swapchain) {
    cast_object(sgpu_metal_swapchain, s, swapchain);
    dealloc_object(sgpu_metal_swapchain, s);
}

int sgpu_metal_acquire_image(sgpu_device_id device,
                             struct sgpu_acquire_descriptor const* descriptor,
                             sgpu_presentable_id* presentable) {
    cast_object(sgpu_metal_device, d, device);
    cast_object(sgpu_metal_swapchain, s, descriptor->swapchain);
    d->presentable.drawable = [s->layer nextDrawable];
    d->presentable.texture.texture = d->presentable.drawable.texture;
    *presentable = (sgpu_presentable_id)&d->presentable;
    return SGPU_RESULT_SUCCESS;
}

void sgpu_metal_begin_render_pass(sgpu_device_id device,
                                  struct sgpu_render_pass_descriptor const* descriptor,
                                  sgpu_render_pass_encoder_id* encoder) {
    cast_object(sgpu_metal_command_buffer, c, descriptor->command_buffer);
    cast_object(sgpu_metal_texture, t, descriptor->color_attachment);
    alloc_object(sgpu_metal_render_pass_encoder, e);

    struct sgpu_color4 const* clear_color = &descriptor->clear_color;

    MTLRenderPassDescriptor* render_pass_desc = [MTLRenderPassDescriptor new];
    render_pass_desc.colorAttachments[0].loadAction = MTLLoadActionClear;
    render_pass_desc.colorAttachments[0].storeAction = MTLStoreActionStore;
    render_pass_desc.colorAttachments[0].clearColor =
        MTLClearColorMake(clear_color->r, clear_color->g, clear_color->b, clear_color->a);
    render_pass_desc.colorAttachments[0].texture = t->texture;

    e->encoder = [c->buffer renderCommandEncoderWithDescriptor:render_pass_desc];
    [render_pass_desc release];

    *encoder = (sgpu_render_pass_encoder_id)e;
}

void sgpu_metal_end_render_pass(sgpu_device_id device, sgpu_render_pass_encoder_id encoder) {
    cast_object(sgpu_metal_render_pass_encoder, e, encoder);
    [e->encoder endEncoding];
    dealloc_object(sgpu_metal_render_pass_encoder, e);
}

void sgpu_metal_present(sgpu_device_id device, struct sgpu_present_descriptor const* descriptor) {
    cast_object(sgpu_metal_command_buffer, c, descriptor->command_buffer);
    cast_object(sgpu_metal_presentable, p, descriptor->presentable);
    [c->buffer presentDrawable:p->drawable];
    [c->buffer commit];
}

void sgpu_metal_presentable_texture(sgpu_device_id device, sgpu_presentable_id presentable,
                                    sgpu_texture_id* texture) {
    cast_object(sgpu_metal_presentable, p, presentable);
    *texture = (sgpu_texture_id)&p->texture;
}
