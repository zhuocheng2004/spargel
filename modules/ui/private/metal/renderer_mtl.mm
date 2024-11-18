#include "modules/ui/private/metal/renderer_mtl.h"

#import <Metal/Metal.h>

#include "modules/ui/private/appkit/window_ns.h"

namespace spargel::ui {

RendererMTL::RendererMTL() {}
RendererMTL::~RendererMTL() {}

void RendererMTL::init() {
  device_ = MTLCreateSystemDefaultDevice();
  layer_ = [[CAMetalLayer alloc] init];
  [layer_ setDevice:device_];
  // default values:
  // pixelFormat is BGRA8Unorm
  // maximumDrawableCount is 3

  NSError* error;

  auto library_url = [[NSBundle mainBundle] URLForResource:@("shader") withExtension:@("metallib")];
  if (library_url == nil) {
    NSLog(@"cannot load shader.metallib");
    return;
  }
  library_ = [device_ newLibraryWithURL:library_url error:&error];
  if (library_ == nil) {
    NSLog(@"cannot create shader library");
    if (error.description != nil) {
      NSLog(@"error description: %@", error.description);
    }
    return;
  }

  id<MTLFunction> vertex_function = [library_ newFunctionWithName:@"quad_vertex"];
  id<MTLFunction> fragment_function = [library_ newFunctionWithName:@"quad_fragment"];

  MTLRenderPipelineDescriptor* pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
  pipelineStateDescriptor.label = @"quad_pipeline";
  pipelineStateDescriptor.vertexFunction = vertex_function;
  pipelineStateDescriptor.fragmentFunction = fragment_function;
  pipelineStateDescriptor.colorAttachments[0].pixelFormat = layer_.pixelFormat;

  pipeline_state_ = [device_ newRenderPipelineStateWithDescriptor:pipelineStateDescriptor
                                                            error:&error];
  [pipelineStateDescriptor release];
  if (pipeline_state_ == nil) {
    NSLog(@"cannot create pipeline state");
    if (error.description != nil) {
      NSLog(@"error description: %@", error.description);
    }
    return;
  }

  command_queue_ = [device_ newCommandQueue];
}

static const vector_float2 unit_vertices[] = {
    {0, 0}, {1, 0}, {1, 1}, {1, 1}, {0, 1}, {0, 0},
};

void RendererMTL::begin() {
  quads_.clear();
  command_buffer_ = [command_queue_ commandBuffer];
  command_buffer_.label = @"MyCommand";

  current_drawable_ = [layer_ nextDrawable];
  if (current_drawable_ == nil) {
    return;
  }
  auto render_pass_desc = [MTLRenderPassDescriptor new];
  render_pass_desc.colorAttachments[0].loadAction = MTLLoadActionClear;
  render_pass_desc.colorAttachments[0].storeAction = MTLStoreActionStore;
  render_pass_desc.colorAttachments[0].clearColor = MTLClearColorMake(0, 1, 1, 1);
  render_pass_desc.colorAttachments[0].texture = current_drawable_.texture;

  render_encoder_ = [command_buffer_ renderCommandEncoderWithDescriptor:render_pass_desc];
  [render_pass_desc release];
  render_encoder_.label = @"MyRenderEncoder";
}

void RendererMTL::end() {
  simd::float2 viewport = {width_, height_};

  [render_encoder_ setViewport:(MTLViewport){0.0, 0.0, viewport.x, viewport.y, 0.0, 1.0}];

  [render_encoder_ setRenderPipelineState:pipeline_state_];

  [render_encoder_ setVertexBytes:unit_vertices
                           length:sizeof(unit_vertices)
                          atIndex:quad_input_index_vertices];

  auto buffer = [device_ newBufferWithBytes:quads_.data()
                                     length:quads_.size() * sizeof(QuadData)
                                    options:MTLResourceStorageModeManaged];

  [render_encoder_ setVertexBuffer:buffer offset:0 atIndex:quad_input_index_quads];

  [render_encoder_ setVertexBytes:&viewport
                           length:sizeof(viewport)
                          atIndex:quad_input_index_viewport];

  [render_encoder_ drawPrimitives:MTLPrimitiveTypeTriangle
                      vertexStart:0
                      vertexCount:6
                    instanceCount:quads_.size()];

  [render_encoder_ endEncoding];
  [command_buffer_ presentDrawable:current_drawable_];
  [command_buffer_ commit];

  [buffer release];
}

void RendererMTL::drawQuad(Rect rect, Color3 color) {
  auto backing_rect = window_->toBacking(rect);
  quads_.push_back({
      .origin = {backing_rect.origin.x, backing_rect.origin.y},
      .size = {backing_rect.size.width, backing_rect.size.height},
      .color = {color.r, color.g, color.b, 1},
  });
}

void RendererMTL::setDrawableSize(float width, float height) {
  if (width_ == width && height_ == height) return;
  width_ = width;
  height_ = height;
  layer_.drawableSize = {width, height};
}

CAMetalLayer* RendererMTL::layer() { return layer_; }

void RendererMTL::setWindow(WindowNS* window) { window_ = window; }

Renderer* createMetalRenderer() { return new RendererMTL(); }

}  // namespace spargel::ui
