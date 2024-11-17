#include "modules/ui/private/metal/renderer_mtl.h"

#import <Metal/Metal.h>

#include "modules/ui/private/appkit/window_ns.h"

namespace spargel::ui {

renderer_mtl::renderer_mtl() : frame_id_{0} {}
renderer_mtl::~renderer_mtl() = default;

void renderer_mtl::init() {
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

static const quad_data quads_to_draw[] = {
    {{20, 20}, {200, 50}, {1, 0, 0, 1}},
    {{60, 40}, {50, 200}, {0, 0, 1, 1}},
    {{-10, -10}, {20, 20}, {0, 1, 0, 1}},
};

void renderer_mtl::begin() {
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
  render_encoder_.label = @"MyRenderEncoder";
}

void renderer_mtl::end() {
  simd::uint2 _viewportSize = {(unsigned int)width_, (unsigned int)height_};

  [render_encoder_ setViewport:(MTLViewport){0.0, 0.0, static_cast<double>(_viewportSize.x),
                                             static_cast<double>(_viewportSize.y), 0.0, 1.0}];

  [render_encoder_ setRenderPipelineState:pipeline_state_];

  [render_encoder_ setVertexBytes:unit_vertices
                           length:sizeof(unit_vertices)
                          atIndex:quad_input_index_vertices];

  auto buffer = [device_ newBufferWithBytes:quads_.data()
                                     length:quads_.size() * sizeof(quad_data)
                                    options:MTLResourceStorageModeManaged];

  // [render_encoder_ setVertexBytes:&quads_to_draw
  //                          length:sizeof(quads_to_draw)
  //                         atIndex:quad_input_index_quads];
  [render_encoder_ setVertexBuffer:buffer offset:0 atIndex:quad_input_index_quads];

  [render_encoder_ setVertexBytes:&_viewportSize
                           length:sizeof(_viewportSize)
                          atIndex:quad_input_index_viewport];

  [render_encoder_ drawPrimitives:MTLPrimitiveTypeTriangle
                      vertexStart:0
                      vertexCount:6
                    instanceCount:quads_.size()];

  [render_encoder_ endEncoding];
  [command_buffer_ presentDrawable:current_drawable_];
  [command_buffer_ commit];
  frame_id_++;
}

void renderer_mtl::draw_quad(float x, float y, float width, float height, color3 color) {
  auto rect = window_->to_backing(x, y, width, height);
  quads_.push_back({
      .origin = {(float)rect.origin.x, (float)rect.origin.y},
      .size = {(float)rect.size.width, (float)rect.size.height},
      .color = {color.r, color.g, color.b, 1},
  });
}

void renderer_mtl::set_drawable_size(double width, double height) {
  if (width_ == width && height_ == height) return;
  width_ = width;
  height_ = height;
  layer_.drawableSize = {width, height};
}

CAMetalLayer* renderer_mtl::layer() { return layer_; }

void renderer_mtl::set_window(window_ns* window) { window_ = window; }

renderer* create_metal_renderer() { return new renderer_mtl(); }

}  // namespace spargel::ui
