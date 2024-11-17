#pragma once

#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>

#include <vector>

#include "modules/ui/private/metal/shader_types.h"
#include "modules/ui/public/renderer.h"

namespace spargel::ui {

class window_ns;

class renderer_mtl final : public renderer {
 public:
  renderer_mtl();
  ~renderer_mtl();

  void init() override;
  void begin() override;
  void end() override;

  void draw_quad(float x, float y, float width, float height,
                 color3 color) override;

  void set_drawable_size(double width, double height);

  CAMetalLayer* layer();

  void set_window(window_ns* window);

 private:
  id<MTLDevice> device_;
  id<MTLLibrary> library_;
  id<MTLRenderPipelineState> pipeline_state_;
  id<MTLCommandQueue> command_queue_;
  id<MTLCommandBuffer> command_buffer_;
  id<MTLRenderCommandEncoder> render_encoder_;

  CAMetalLayer* layer_;
  id<CAMetalDrawable> current_drawable_;

  window_ns* window_;

  double width_;
  double height_;

  std::vector<quad_data> quads_;

  int frame_id_;
};

renderer* create_metal_renderer();

}  // namespace spargel::ui
