
#include <stdint.h>
#include <stdlib.h>

#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <tracy/Tracy.hpp>
#include <vector>

#include "bit_array.h"
#include "ecs.h"

struct archetype {
  bit_array bits;

  archetype() : bits(0) {}

  archetype(const archetype& that) : bits(that.bits) {}

  bool contains(const archetype& that) const {
    return bits.lies_over(that.bits);
  }

  archetype& operator=(const archetype& that) {
    bits = that.bits;
    return *this;
  }

  bool operator==(const archetype& that) const { return bits == that.bits; }
  bool operator>(const archetype& that) const { return bits > that.bits; }
  bool operator<(const archetype& that) const { return bits < that.bits; }
};

struct component_storage {
  archetype type;
  int size;
  std::set<uint32_t> ids;  // component ids that appear in the archetype
  int count;
  int capacity;
  void** data;  // sparse, only non-null at the component ids

  component_storage(archetype& t) : type(t), count(0), capacity(1) {
    size = t.bits.get_size();
    data = new void*[size];
    for (int i = 0; i < size; i++) {
      if (type.bits[i]) {
        ids.emplace(i);
        data[i] = malloc(1);
      } else {
        data[i] = nullptr;
      }
    }
  }

  component_storage(const component_storage& that) = delete;

  ~component_storage() {
    for (auto i : ids) free(data[i]);
    delete[] data;
  }
};

struct component_info {
  uint32_t id;
  size_t size;
};

struct ecs_world_impl {
  std::map<std::string, component_info> name2component;
  std::map<uint32_t, component_info> id2component;

  std::map<archetype, component_storage*> storages;

  // this will fill infos in the order of the names
  archetype make_archetype(int count, const char** component_names,
                           component_info* infos /*nullable*/) {
    archetype t;
    for (int i = 0; i < count; i++) {
      auto name = component_names[i];
      if (!name2component.contains(name))
        throw std::invalid_argument("ecs: unknown component");
      auto info = name2component[name];
      if (infos) infos[i] = info;
      t.bits.set_bit(info.id);
    }
    return t;
  }

  component_storage& get_or_create_storage(archetype t) {
    if (storages.contains(t)) {
      return *storages[t];
    } else {
      // FIXME
      auto storage = new component_storage(t);
      storages.emplace(t, storage);
      return *storage;
    }
  }

  void clear_storage() {
    for (auto& pair : storages) {
      delete pair.second;
    }
    storages.clear();
  }
};

ecs_world_impl* ecs_create_world() {
  auto world = new ecs_world_impl;
  return world;
}

void ecs_destroy_world(ecs_world_impl* world) {
  world->clear_storage();
  delete world;
}

void ecs_register_component(ecs_world_impl* world, char const* name,
                            size_t size) {
  if (world->name2component.contains(name))
    throw std::invalid_argument("ecs: duplicate component");

  auto id = (uint32_t)world->name2component.size();
  component_info info{.id = id, .size = size};
  world->name2component.emplace(name, info);
  world->id2component.emplace(id, info);
}

void ecs_spawn_entities(ecs_world world, ecs_spawn_desc* desc) {
  ZoneScoped;
  auto* infos = new component_info[desc->num_components];
  archetype t =
      world->make_archetype(desc->num_components, desc->components, infos);

  component_storage& storage = world->get_or_create_storage(t);

  int offset = storage.count;
  int count = storage.count + desc->count;

  // resize storage if necessary
  int new_capacity = storage.capacity;
  if (count > new_capacity) {
    // grow
    while (new_capacity < 2 * count) new_capacity *= 2;
  }

  for (auto i : storage.ids) {
    size_t c_size = world->id2component[i].size;
    storage.data[i] = realloc(storage.data[i], c_size * new_capacity);
  }

  storage.capacity = new_capacity;
  storage.count = count;
  // resize finished

  ecs_view view;
  view.components = new void*[desc->num_components];
  for (int i = 0; i < desc->num_components; i++) {
    component_info& info = infos[i];
    view.components[i] = (char*)storage.data[info.id] + info.size * offset;
  }
  view.count = desc->count;
  desc->callback(&view, desc->callback_data);

  delete[] view.components;

  delete[] infos;
}

void ecs_query(ecs_world world, ecs_query_desc* desc) {
  ZoneScoped;
  auto* infos = new component_info[desc->count];
  archetype t;
  {
    ZoneScopedN("ecs/build_idset");
    t = world->make_archetype(desc->count, desc->components, infos);
  }

  ecs_view view;
  view.components = new void*[desc->count];

  // find archetypes that contain the querying archetype
  for (auto& pair : world->storages) {
    const archetype& type = pair.first;
    if (!type.contains(t)) continue;

    {
      ZoneScopedN("ecs/construct_view");
      component_storage* storage = pair.second;
      for (int i = 0; i < desc->count; i++) {
        view.components[i] = storage->data[infos[i].id];
      }
      view.count = storage->count;
    }

    desc->callback(&view, desc->data);
  }

  delete[] view.components;

  delete[] infos;
}
