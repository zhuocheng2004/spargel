#include "ecs.h"

#include <stddef.h>
#include <string.h>

#include <algorithm>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <vector>

struct chunk {
  std::string name;
  // size of every slot
  size_t size;
  void* data;

  ~chunk() {
    if (data != nullptr) {
      free(data);
    }
  }
};

struct archetype {
  std::set<uint32_t> components;
  // number of entities;
  size_t count;
  size_t capacity;
  std::vector<chunk> chunks;
};

struct ecs_world_impl {
  // component id map
  std::map<std::string, uint32_t> cid;
  std::map<std::string, size_t> csize;
  // number of component
  uint32_t cnum = 0;
  std::vector<std::pair<ecs_system_t, void*>> systems;
  std::vector<archetype> archetypes;
};

ecs_world_impl* ecs_create_world() {
  ecs_world_impl* world = new ecs_world_impl;
  return world;
}

void ecs_destroy_world(ecs_world_impl* world) { delete world; }

void ecs_register_component(ecs_world_impl* world, char const* name,
                            size_t size) {
  if (world->cid.contains(name)) {
    return;
  }
  world->cid.emplace(name, world->cnum);
  world->csize.emplace(name, size);
  world->cnum++;
}

void ecs_run(ecs_world_impl* world) {
  for (auto [system, data] : world->systems) {
    system(world, data);
  }
}

void ecs_spawn_entities(ecs_world world, ecs_spawn_desc* desc) {
  // step 1. build set of component id
  std::set<uint32_t> cset;
  for (int i = 0; i < desc->num_components; i++) {
    auto iter = world->cid.find(desc->components[i]);
    if (iter == world->cid.end()) {
      return;
    }
    cset.insert(iter->second);
  }
  // step 2. try to find existing archetype
  auto iter = std::find_if(
      world->archetypes.begin(), world->archetypes.end(),
      [&](auto const& archetype) { return archetype.components == cset; });
  ecs_view view;
  view.components = new void*[desc->num_components];
  // offset (not bytes)
  size_t offset = 0;
  if (iter != world->archetypes.end()) {
    auto need = iter->count + desc->count;
    if (need > iter->capacity) {
      // allocate more storage
      iter->capacity = need * 2;
      for (auto& c : iter->chunks) {
        void* new_data = malloc(c.size * iter->capacity);
        memcpy(new_data, c.data, c.size * iter->count);
        free(c.data);
        c.data = new_data;
      }
    }
    offset = iter->count;
    iter->count += desc->count;
  } else {
    // no existing component
    auto capacity = desc->count * 2;
    std::vector<chunk> chunks(desc->num_components);
    for (int i = 0; i < desc->num_components; i++) {
      chunks[i].name = desc->components[i];
      chunks[i].size = world->csize[desc->components[i]];
      chunks[i].data = malloc(capacity * chunks[i].size);
    }
    world->archetypes.emplace_back(std::move(cset), desc->count, capacity,
                                   std::move(chunks));
    iter = world->archetypes.end() - 1;
  }
  // fill ptrs
  for (int i = 0; i < desc->num_components; i++) {
    auto chunk_iter = std::find_if(
        iter->chunks.begin(), iter->chunks.end(),
        [=](auto const& c) { return c.name == desc->components[i]; });
    view.components[i] = (char*)chunk_iter->data + chunk_iter->size * offset;
  }
  view.count = desc->count;
  desc->callback(&view, desc->callback_data);
  delete[] view.components;
}

void ecs_query(ecs_world world, struct ecs_query_desc* desc) {
  // step 1. build set of required component ids
  std::set<uint32_t> cset;
  for (int i = 0; i < desc->count; i++) {
    auto iter = world->cid.find(desc->components[i]);
    if (iter == world->cid.end()) {
      return;
    }
    cset.insert(iter->second);
  }
  // step 2. find archetype
  ecs_view view;
  view.components = new void*[desc->count];
  for (auto& archetype : world->archetypes) {
    if (std::includes(archetype.components.begin(), archetype.components.end(),
                      cset.begin(), cset.end())) {
      // step 3. found one archetype; construct view
      // fill component pointers
      for (int i = 0; i < desc->count; i++) {
        auto iter = std::find_if(
            archetype.chunks.begin(), archetype.chunks.end(),
            [=](auto const& c) { return c.name == desc->components[i]; });
        view.components[i] = iter->data;
      }
      view.count = archetype.count;
      // step 4. process data
      desc->callback(&view, desc->data);
    }
  }
  delete[] view.components;
}
