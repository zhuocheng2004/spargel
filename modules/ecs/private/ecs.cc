#include "modules/ecs/public/ecs.h"

namespace spargel::ecs {

World::World() = default;
World::~World() = default;

Entity World::add_entity() {
  auto e = entity_count_;
  entity_count_++;
  return e;
}

}
