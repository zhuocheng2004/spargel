#include "ecs.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <algorithm>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <vector>

// tracy
#include <tracy/Tracy.hpp>

struct chunk {
  chunk() = default;
  chunk(std::string name, size_t size, void* data)
      : name{std::move(name)}, size{size}, data{data} {}
  chunk(chunk const&) = delete;
  chunk(chunk&& that)
      : name{std::move(that.name)}, size{that.size}, data{that.data} {
    that.data = nullptr;
  }

  chunk& operator=(chunk const&) = delete;

  ~chunk() {
    if (data != nullptr) {
      free(data);
    }
  }

  std::string name;
  // size of every slot
  size_t size = 0;
  void* data = nullptr;
};

struct archetype {
  archetype() = default;
  archetype(std::set<uint32_t> components, size_t count, size_t capacity,
            std::vector<chunk> chunks)
      : components{std::move(components)},
        count{count},
        capacity{capacity},
        chunks{std::move(chunks)} {}
  archetype(archetype const&) = delete;
  archetype(archetype&&) = default;

  archetype& operator=(archetype const&) = delete;

  std::set<uint32_t> components;
  // number of entities;
  size_t count;
  size_t capacity;
  std::vector<chunk> chunks;
};

struct component_info {
  uint32_t id;
  size_t size;
};

struct ecs_world_impl {
  ecs_world_impl() = default;
  ecs_world_impl(ecs_world_impl const&) = delete;
  std::map<std::string, component_info> components;
  std::vector<archetype> archetypes;
};

ecs_world_impl* ecs_create_world() {
  ecs_world_impl* world = new ecs_world_impl;
  return world;
}

void ecs_destroy_world(ecs_world_impl* world) { delete world; }

void ecs_register_component(ecs_world_impl* world, char const* name,
                            size_t size) {
  if (world->components.contains(name)) {
    printf("error: ecs: duplicate component\n");
    return;
  }
  auto id = world->components.size();
  world->components.try_emplace(name, id, size);
}

void ecs_spawn_entities(ecs_world world, ecs_spawn_desc* desc) {
  ZoneScoped;
  // step 1. build set of component id
  std::set<uint32_t> cset;
  for (int i = 0; i < desc->num_components; i++) {
    auto iter = world->components.find(desc->components[i]);
    if (iter == world->components.end()) {
      printf("error: ecs: unknown component\n");
      return;
    }
    cset.insert(iter->second.id);
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
        if (c.size > 0) {
          void* new_data = malloc(c.size * iter->capacity);
          memcpy(new_data, c.data, c.size * iter->count);
          free(c.data);
          c.data = new_data;
        }
      }
    }
    offset = iter->count;
    iter->count += desc->count;
  } else {
    // no existing component
    auto capacity = desc->count * 2;
    std::vector<chunk> chunks(desc->num_components);
    for (int i = 0; i < desc->num_components; i++) {
      std::string name = desc->components[i];
      auto size = world->components[name].size;
      chunks[i].name = std::move(name);
      chunks[i].size = size;
      if (size > 0) {
        chunks[i].data = malloc(capacity * size);
      } else {
        chunks[i].data = nullptr;
      }
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

void ecs_query(ecs_world world, ecs_query_desc* desc) {
  ZoneScoped;
  // step 1. build set of required component ids
  std::set<uint32_t> cset;
  {
    ZoneScopedN("ecs/build_idset");
    for (int i = 0; i < desc->count; i++) {
      auto iter = world->components.find(desc->components[i]);
      if (iter == world->components.end()) {
        printf("error: ecs: unknown component\n");
        return;
      }
      cset.insert(iter->second.id);
    }
  }
  // step 2. find archetype
  ecs_view view;
  view.components = new void*[desc->count];
  for (auto& archetype : world->archetypes) {
    if (std::includes(archetype.components.cbegin(),
                      archetype.components.cend(), cset.cbegin(),
                      cset.cend())) {
      {
        ZoneScopedN("ecs/construct_view");
        // step 3. found one archetype; construct view
        // fill component pointers
        for (int i = 0; i < desc->count; i++) {
          auto iter = std::find_if(
              archetype.chunks.begin(), archetype.chunks.end(),
              [=](auto const& c) { return c.name == desc->components[i]; });
          view.components[i] = iter->data;
        }
        view.count = archetype.count;
      }
      // step 4. process data
      desc->callback(&view, desc->data);
    }
  }
  delete[] view.components;
}
