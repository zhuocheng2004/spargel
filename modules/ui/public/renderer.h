#pragma once

#include "modules/ui/public/types.h"

namespace spargel::ui {

class RenderTarget;

class Renderer {
 public:
  virtual ~Renderer();

  virtual void init() = 0;

  virtual void setRenderTarget(RenderTarget* target) = 0;

  virtual void begin() = 0;
  virtual void end() = 0;

  virtual void drawQuad(Rect rect, Color3 color) = 0;
};

}  // namespace spargel::ui
