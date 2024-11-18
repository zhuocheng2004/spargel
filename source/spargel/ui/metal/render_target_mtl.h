#pragma once

#import <QuartzCore/QuartzCore.h>

#include "spargel/ui/render_target.h"
#include "spargel/ui/types.h"

namespace spargel::ui {

class WindowNS;

class RenderTargetMTL final : public RenderTarget {
 public:
  explicit RenderTargetMTL(WindowNS* window);
  ~RenderTargetMTL() override;

  RectSize size() const;
  CAMetalLayer* layer();

  // called by RendererMTL
  Rect toLayer(Rect rect);

  // called by WindowNS
  void setSize(RectSize size);

 private:
  CAMetalLayer* _layer;
  RectSize _size;
  WindowNS* _window;
};

}  // namespace spargel::ui
