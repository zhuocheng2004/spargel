#include "modules/ui/private/metal/render_target_mtl.h"

#include "modules/ui/private/appkit/window_ns.h"

namespace spargel::ui {

RenderTargetMTL::RenderTargetMTL(WindowNS* window) {
  _window = window;
  _layer = [[CAMetalLayer alloc] init];
}
RenderTargetMTL::~RenderTargetMTL() { [_layer release]; }

RectSize RenderTargetMTL::size() const { return _size; }
CAMetalLayer* RenderTargetMTL::layer() { return _layer; }

Rect RenderTargetMTL::toLayer(Rect rect) { return _window->toBacking(rect); }

void RenderTargetMTL::setSize(RectSize size) {
  if (_size == size) return;
  _size = size;
  _layer.drawableSize = {size.width, size.height};
}

}  // namespace spargel::ui
