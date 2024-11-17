#pragma once

#include "modules/base/public/types.h"

namespace spargel::ecs {

using entity = ssize;

class world {
 public:
  world();
  ~world();

  entity add_entity();

 private:
  entity next_entity();

  ssize entity_count_;
};

}
