#include "modules/ecs/public/ecs.h"

namespace spargel::ecs {

world::world() = default;
world::~world() = default;

entity world::add_entity() {
  auto e = entity_count_;
  entity_count_++;
  return e;
}

}
