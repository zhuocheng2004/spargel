#include <spargel/renderer/renderer.h>
#include <spargel/renderer/shader_types_mtl.h>
#include <spargel/ui/ui_mac.h>

/* libc */
#include <stdlib.h>

/* platform */
#import <Metal/Metal.h>

struct spargel_renderer {
    id<MTLDevice> device;
    id<MTLLibrary> library;
    id<MTLCommandQueue> command_queue;
    id<MTLRenderPipelineState> pipeline_state;

    struct spargel_ui_window* window;
    CAMetalLayer* layer;

    id<MTLTexture>* textures;
    ssize texture_count;
    ssize texture_capacity;

    struct quad_data* quads;
    ssize quad_count;
    ssize quad_capacity;

    id<MTLBuffer> viewport_buffer;
    id<MTLBuffer> unit_vertices_buffer;
};

/**
 * @brief grow an array
 * @param ptr *ptr points to start of array
 * @param capacity pointer to current capacity
 * @param size item size
 * @param need the min capacity after growing */
static void grow_array(void** ptr, ssize* capacity, ssize size, ssize need) {
    ssize cap2 = *capacity * 2;
    ssize new_cap = cap2 > need ? cap2 : need;
    if (new_cap < 8) new_cap = 8;
    *ptr = realloc(*ptr, new_cap * size);
    *capacity = new_cap;
}

static void push_texture(struct spargel_renderer* renderer, id<MTLTexture> texture) {
    if (renderer->texture_count + 1 > renderer->texture_capacity) {
        grow_array((void**)&renderer->textures, &renderer->texture_capacity, sizeof(id<MTLDevice>),
                   renderer->texture_count + 1);
    }
    renderer->textures[renderer->texture_count] = texture;
    renderer->texture_count++;
}

static const simd_float2 unit_vertices[] = {{0, 0}, {1, 0}, {1, 1}, {1, 1}, {0, 1}, {0, 0}};

spargel_renderer_id spargel_create_renderer(spargel_ui_window_id window) {
    struct spargel_renderer* renderer = malloc(sizeof(struct spargel_renderer));
    renderer->device = MTLCreateSystemDefaultDevice();

    NSError* error;

    NSURL* library_url = [[NSBundle mainBundle] URLForResource:@("shader")
                                                 withExtension:@("metallib")];
    renderer->library = [renderer->device newLibraryWithURL:library_url error:&error];
    renderer->command_queue = [renderer->device newCommandQueue];

    renderer->textures = NULL;
    renderer->texture_count = 0;
    renderer->texture_capacity = 0;
    renderer->quads = NULL;
    renderer->quad_count = 0;
    renderer->quad_capacity = 0;

    window->layer.device = renderer->device;
    renderer->window = window;
    renderer->layer = window->layer;

    id<MTLFunction> vertex_function = [renderer->library newFunctionWithName:@"vertex_shader"];
    id<MTLFunction> fragment_function = [renderer->library newFunctionWithName:@"fragment_shader"];

    MTLRenderPipelineDescriptor* pipelineStateDescriptor =
        [[MTLRenderPipelineDescriptor alloc] init];
    pipelineStateDescriptor.label = @"quad_pipeline";
    pipelineStateDescriptor.vertexFunction = vertex_function;
    pipelineStateDescriptor.fragmentFunction = fragment_function;
    pipelineStateDescriptor.colorAttachments[0].pixelFormat = renderer->layer.pixelFormat;

    renderer->pipeline_state =
        [renderer->device newRenderPipelineStateWithDescriptor:pipelineStateDescriptor
                                                         error:&error];
    [pipelineStateDescriptor release];

    if (renderer->pipeline_state == nil) {
        NSLog(@"cannot create pipeline state");
        if (error.description != nil) {
            NSLog(@"error description: %@", error.description);
        }
    }

    renderer->viewport_buffer = [renderer->device newBufferWithLength:sizeof(simd_float2)
                                                              options:MTLResourceStorageModeShared];
    renderer->unit_vertices_buffer =
        [renderer->device newBufferWithBytes:unit_vertices
                                      length:sizeof(unit_vertices)
                                     options:MTLResourceStorageModePrivate];

    return renderer;
}

spargel_renderer_texture_id spargel_renderer_add_texture(
    spargel_renderer_id renderer, struct spargel_renderer_texture_descriptor const* descriptor) {
    MTLTextureDescriptor* desc = [[MTLTextureDescriptor alloc] init];
    desc.pixelFormat = MTLPixelFormatBGRA8Unorm;
    desc.width = descriptor->width;
    desc.height = descriptor->height;
    id<MTLTexture> texture = [renderer->device newTextureWithDescriptor:desc];
    [desc release];

    NSUInteger row_bytes = 4 * descriptor->width;
    MTLRegion region = {
        {0, 0, 0},
        {(NSUInteger)descriptor->width, (NSUInteger)descriptor->height, 1},
    };
    [texture replaceRegion:region mipmapLevel:0 withBytes:descriptor->data bytesPerRow:row_bytes];

    push_texture(renderer, texture);
    return renderer->texture_count;
}

void spargel_renderer_start_frame(spargel_renderer_id renderer) { renderer->quad_count = 0; }

void spargel_renderer_add_quad(spargel_renderer_id renderer, float x, float y, float width,
                               float height, spargel_renderer_texture_id texture) {
    if (renderer->quad_count + 1 > renderer->quad_capacity) {
        grow_array((void**)&renderer->quads, &renderer->quad_capacity, sizeof(struct quad_data),
                   renderer->quad_count + 1);
    }
    struct quad_data* data = &renderer->quads[renderer->quad_count];
    renderer->quad_count++;

    data->origin = (simd_float2){x, y};
    data->size = (simd_float2){width, height};
    data->texture = texture;
}

void spargel_renderer_render(spargel_renderer_id renderer) {
    id<MTLCommandBuffer> command_buffer = [renderer->command_queue commandBuffer];
    command_buffer.label = @"quad command";

    id<CAMetalDrawable> current_drawable = [renderer->layer nextDrawable];
    if (current_drawable == nil) {
        return;
    }

    MTLRenderPassDescriptor* render_pass_desc = [MTLRenderPassDescriptor new];
    render_pass_desc.colorAttachments[0].loadAction = MTLLoadActionClear;
    render_pass_desc.colorAttachments[0].storeAction = MTLStoreActionStore;
    render_pass_desc.colorAttachments[0].clearColor = MTLClearColorMake(0, 1, 1, 1);
    render_pass_desc.colorAttachments[0].texture = current_drawable.texture;

    id<MTLRenderCommandEncoder> render_encoder =
        [command_buffer renderCommandEncoderWithDescriptor:render_pass_desc];
    [render_pass_desc release];
    render_encoder.label = @"MyRenderEncoder";

    simd_float2 viewport = {
        renderer->window->drawable_width,
        renderer->window->drawable_height,
    };
    memcpy(renderer->viewport_buffer.contents, &viewport, sizeof(viewport));

    id<MTLBuffer> quads_buffer =
        [renderer->device newBufferWithBytes:renderer->quads
                                      length:sizeof(struct quad_data) * renderer->quad_count
                                     options:MTLResourceStorageModePrivate];

    [render_encoder setViewport:(MTLViewport){0.0, 0.0, viewport.x, viewport.y, 0.0, 1.0}];

    /* for each quad */

    [render_encoder setRenderPipelineState:renderer->pipeline_state];
    /* set vertex buffer */

    [render_encoder setVertexBuffer:renderer->unit_vertices_buffer
                             offset:0
                            atIndex:SPARGEL_VERTEX_INPUT_INDEX_VERTICES];
    [render_encoder setVertexBuffer:renderer->viewport_buffer
                             offset:0
                            atIndex:SPARGEL_VERTEX_INPUT_INDEX_VIEWPORT];

    [render_encoder setVertexBuffer:quads_buffer offset:0 atIndex:SPARGEL_VERTEX_INPUT_INDEX_QUADS];

    /* todo */
    [render_encoder setFragmentTexture:renderer->textures[0]
                               atIndex:SPARGEL_TEXTURE_INDEX_BASE_COLOR];

    [render_encoder drawPrimitives:MTLPrimitiveTypeTriangle
                       vertexStart:0
                       vertexCount:6
                     instanceCount:renderer->quad_count];

    [render_encoder endEncoding];

    [command_buffer presentDrawable:current_drawable];
    [command_buffer commit];
}
