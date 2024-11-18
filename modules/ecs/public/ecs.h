#pragma once

#include "modules/base/public/types.h"

namespace spargel::ecs {

using Entity = ssize;

class World {
 public:
  World();
  ~World();

  Entity add_entity();

 private:
  Entity next_entity();

  ssize entity_count_;
};

}
