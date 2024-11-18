#pragma once

#include "spargel/base/types.h"

namespace spargel::ecs {

using Entity = ssize;

template <typename T>
struct ComponentInfo;

//
class World {
 public:
  World();
  ~World();

  Entity addEntity();

  template <typename T>
  void addComponent(Entity entity, T const* data) {}

 private:
  ssize _entity_count;
};

}  // namespace spargel::ecs
