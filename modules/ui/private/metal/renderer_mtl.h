#pragma once

#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>

#include <vector>

#include "modules/ui/private/metal/shader_types.h"
#include "modules/ui/public/renderer.h"

namespace spargel::ui {

class WindowNS;

class RendererMTL final : public Renderer {
 public:
  RendererMTL();
  ~RendererMTL();

  void init() override;
  void begin() override;
  void end() override;

  void drawQuad(Rect rect, Color3 color) override;

  void setDrawableSize(float width, float height);

  CAMetalLayer* layer();

  void setWindow(WindowNS* window);

 private:
  id<MTLDevice> device_;
  id<MTLLibrary> library_;
  id<MTLRenderPipelineState> pipeline_state_;
  id<MTLCommandQueue> command_queue_;
  id<MTLCommandBuffer> command_buffer_;
  id<MTLRenderCommandEncoder> render_encoder_;

  CAMetalLayer* layer_;
  id<CAMetalDrawable> current_drawable_;

  WindowNS* window_;

  float width_;
  float height_;

  std::vector<QuadData> quads_;
};

Renderer* createMetalRenderer();

}  // namespace spargel::ui
