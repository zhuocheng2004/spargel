#include <spargel/gpu/gpu_metal.h>
#include <spargel/ui/ui_mac.h>

/* libc */
#include <stdlib.h>

/* platform */
#import <Metal/Metal.h>

#define alloc_object(type, name)                                                                   \
    struct type* name =                                                                            \
        (struct type*)spargel::base::allocate(sizeof(struct type), spargel::base::ALLOCATION_GPU); \
    if (!name) spargel_panic_here();

#define cast_object(type, name, object) struct type* name = (struct type*)(object);

#define dealloc_object(type, name) \
    spargel::base::deallocate(name, sizeof(struct type), spargel::base::ALLOCATION_GPU);

namespace spargel::gpu {

    /**
     * We only target Apple Silicon.
     * This in particular guarantees that there is only one GPU.
     */

    struct metal_command_queue {
        id<MTLCommandQueue> queue;
    };

    struct metal_command_buffer {
        id<MTLCommandQueue> queue;
        id<MTLCommandBuffer> buffer;
    };

    struct metal_shader_library {
        id<MTLLibrary> library;
    };

    struct metal_shader_function {
        id<MTLFunction> function;
    };

    struct metal_render_pipeline {
        id<MTLRenderPipelineState> pipeline;
    };

    struct metal_surface {
        CAMetalLayer* layer;
    };

    struct metal_swapchain {
        CAMetalLayer* layer;
    };

    struct metal_texture {
        id<MTLTexture> texture;
    };

    struct metal_presentable {
        id<CAMetalDrawable> drawable;
        struct metal_texture texture;
    };

    struct metal_render_pass_encoder {
        id<MTLRenderCommandEncoder> encoder;
    };

    struct metal_device {
        int backend;
        id<MTLDevice> device;
        struct metal_presentable presentable;
    };

    int metal_create_default_device(struct device_descriptor const* descriptor, device_id* device) {
        alloc_object(metal_device, d);
        d->device = MTLCreateSystemDefaultDevice();
        d->backend = BACKEND_METAL;
        *device = (device_id)d;
        return RESULT_SUCCESS;
    }

    void metal_destroy_device(device_id device) {
        cast_object(metal_device, d, device);
        [d->device release];
        dealloc_object(metal_device, d);
    }

    int metal_create_command_queue(device_id device, command_queue_id* queue) {
        alloc_object(metal_command_queue, q);
        cast_object(metal_device, d, device);
        q->queue = [d->device newCommandQueue];
        *queue = (command_queue_id)q;
        return RESULT_SUCCESS;
    }

    void metal_destroy_command_queue(device_id device, command_queue_id command_queue) {
        cast_object(metal_command_queue, q, command_queue);
        [q->queue release];
        dealloc_object(metal_command_queue, q);
    }

    int metal_create_shader_library(device_id device,
                                    struct metal_shader_library_descriptor const* descriptor,
                                    metal_shader_library_id* library) {
        alloc_object(metal_shader_library, lib);
        cast_object(metal_device, d, device);

        dispatch_data_t lib_data =
            dispatch_data_create(descriptor->code, descriptor->size,
                                 dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),
                                 DISPATCH_DATA_DESTRUCTOR_DEFAULT);

        NSError* error;
        lib->library = [d->device newLibraryWithData:lib_data error:&error];
        if (lib->library == nil) {
            free(lib);
            return RESULT_CANNOT_CREATE_METAL_LIBRARY;
        }
        *library = (metal_shader_library_id)lib;
        return RESULT_SUCCESS;
    }

    void metal_destroy_shader_library(device_id device, metal_shader_library_id shader_library) {
        cast_object(metal_shader_library, lib, shader_library);
        [lib->library release];
        dealloc_object(metal_shader_library, lib);
    }

    int metal_create_shader_function(device_id device,
                                     struct metal_shader_function_descriptor const* descriptor,
                                     shader_function_id* func) {
        alloc_object(metal_shader_function, f);
        cast_object(metal_shader_library, lib, descriptor->library);

        f->function =
            [lib->library newFunctionWithName:[NSString stringWithUTF8String:descriptor->name]];
        if (f->function == nil) {
            free(f);
            return RESULT_CANNOT_CREATE_SHADER_FUNCTION;
        }
        *func = (shader_function_id)f;
        return RESULT_SUCCESS;
    }

    void metal_destroy_shader_function(device_id device, shader_function_id func) {
        cast_object(metal_shader_function, f, func);
        [f->function release];
        dealloc_object(metal_shader_function, f);
    }

    static MTLPixelFormat metal_translate_format(int format) {
        switch (format) {
        case FORMAT_BRGA8_UNORM:
            return MTLPixelFormatBGRA8Unorm;
        case FORMAT_BGRA8_SRGB:
            return MTLPixelFormatBGRA8Unorm_sRGB;
        default:
            spargel_panic_here();
        }
    }

    int metal_create_render_pipeline(device_id device,
                                     struct render_pipeline_descriptor const* descriptor,
                                     render_pipeline_id* pipeline) {
        cast_object(metal_device, d, device);
        cast_object(metal_shader_function, vs, descriptor->vertex_function);
        cast_object(metal_shader_function, fs, descriptor->fragment_function);
        alloc_object(metal_render_pipeline, p);

        spargel_assert(d != 0);
        spargel_assert(vs != 0);
        spargel_assert(fs != 0);

        MTLRenderPipelineDescriptor* desc = [[MTLRenderPipelineDescriptor alloc] init];
        desc.vertexFunction = vs->function;
        desc.fragmentFunction = fs->function;
        desc.colorAttachments[0].pixelFormat = metal_translate_format(descriptor->target_format);

        NSError* err;
        id<MTLRenderPipelineState> pstate = [d->device newRenderPipelineStateWithDescriptor:desc
                                                                                      error:&err];
        [desc release];

        if (pstate == nil) {
            dealloc_object(metal_render_pipeline, p);
            return RESULT_CANNOT_CREATE_RENDER_PIPELINE;
        }

        p->pipeline = pstate;

        *pipeline = (render_pipeline_id)p;

        return RESULT_SUCCESS;
    }

    void metal_destroy_render_pipeline(device_id device, render_pipeline_id pipeline) {
        cast_object(metal_render_pipeline, p, pipeline);
        [p->pipeline release];
        dealloc_object(metal_render_pipeline, p);
    }

    int metal_create_command_buffer(device_id device,
                                    struct command_buffer_descriptor const* descriptor,
                                    command_buffer_id* command_buffer) {
        cast_object(metal_command_queue, q, descriptor->queue);
        alloc_object(metal_command_buffer, cmdbuf);

        cmdbuf->queue = q->queue;
        cmdbuf->buffer = [q->queue commandBuffer];

        *command_buffer = (command_buffer_id)cmdbuf;
        return RESULT_SUCCESS;
    }

    void metal_destroy_command_buffer(device_id device, command_buffer_id command_buffer) {
        cast_object(metal_command_buffer, c, command_buffer);
        dealloc_object(metal_command_buffer, c);
    }

    void metal_reset_command_buffer(device_id device, command_buffer_id command_buffer) {
        cast_object(metal_command_buffer, c, command_buffer);
        c->buffer = [c->queue commandBuffer];
    }

    int metal_create_surface(device_id device, struct surface_descriptor const* descriptor,
                             surface_id* surface) {
        cast_object(metal_device, d, device);
        alloc_object(metal_surface, s);
        s->layer = descriptor->window->layer;
        [s->layer retain];
        s->layer.device = d->device;
        *surface = (surface_id)s;
        return RESULT_SUCCESS;
    }

    void metal_destroy_surface(device_id device, surface_id surface) {
        cast_object(metal_surface, s, surface);
        [s->layer release];
        dealloc_object(metal_surface, s);
    }

    int metal_create_swapchain(device_id device, struct swapchain_descriptor const* descriptor,
                               swapchain_id* swapchain) {
        cast_object(metal_surface, s, descriptor->surface);
        alloc_object(metal_swapchain, sc);
        sc->layer = s->layer;
        *swapchain = (swapchain_id)sc;
        return RESULT_SUCCESS;
    }

    void metal_destroy_swapchain(device_id device, swapchain_id swapchain) {
        cast_object(metal_swapchain, s, swapchain);
        dealloc_object(metal_swapchain, s);
    }

    int metal_acquire_image(device_id device, struct acquire_descriptor const* descriptor,
                            presentable_id* presentable) {
        cast_object(metal_device, d, device);
        cast_object(metal_swapchain, s, descriptor->swapchain);
        d->presentable.drawable = [s->layer nextDrawable];
        d->presentable.texture.texture = d->presentable.drawable.texture;
        *presentable = (presentable_id)&d->presentable;
        return RESULT_SUCCESS;
    }

    void metal_begin_render_pass(device_id device, struct render_pass_descriptor const* descriptor,
                                 render_pass_encoder_id* encoder) {
        cast_object(metal_command_buffer, c, descriptor->command_buffer);
        cast_object(metal_texture, t, descriptor->color_attachment);
        alloc_object(metal_render_pass_encoder, e);

        struct color4 const* clear_color = &descriptor->clear_color;

        MTLRenderPassDescriptor* render_pass_desc = [MTLRenderPassDescriptor new];
        render_pass_desc.colorAttachments[0].loadAction = MTLLoadActionClear;
        render_pass_desc.colorAttachments[0].storeAction = MTLStoreActionStore;
        render_pass_desc.colorAttachments[0].clearColor =
            MTLClearColorMake(clear_color->r, clear_color->g, clear_color->b, clear_color->a);
        render_pass_desc.colorAttachments[0].texture = t->texture;

        e->encoder = [c->buffer renderCommandEncoderWithDescriptor:render_pass_desc];
        [render_pass_desc release];

        *encoder = (render_pass_encoder_id)e;
    }

    void metal_end_render_pass(device_id device, render_pass_encoder_id encoder) {
        cast_object(metal_render_pass_encoder, e, encoder);
        [e->encoder endEncoding];
        dealloc_object(metal_render_pass_encoder, e);
    }

    void metal_present(device_id device, struct present_descriptor const* descriptor) {
        cast_object(metal_command_buffer, c, descriptor->command_buffer);
        cast_object(metal_presentable, p, descriptor->presentable);
        [c->buffer presentDrawable:p->drawable];
        [c->buffer commit];
    }

    void metal_presentable_texture(device_id device, presentable_id presentable,
                                   texture_id* texture) {
        cast_object(metal_presentable, p, presentable);
        *texture = (texture_id)&p->texture;
    }

}  // namespace spargel::gpu
