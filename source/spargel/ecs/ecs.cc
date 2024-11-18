#include "spargel/ecs/ecs.h"

namespace spargel::ecs {

World::World() {
  _entity_count = 0;
}
World::~World() {}

Entity World::addEntity() {
  auto e = _entity_count;
  _entity_count++;
  return e;
}

}  // namespace spargel::ecs
