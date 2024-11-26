#include <spargel/ecs/ecs.h>
#include <stdlib.h>

struct chunk {};

struct component_info {
  struct sbase_string name;
  u32 name_hash;
  ssize size;
  secs_component_id id;
};

struct secs_world {
  struct {
    struct component_info* infos;
    /* the number of all known components */
    ssize count;
    ssize capacity;
  } components;
};

secs_world_id secs_create_world() {
  struct secs_world* world = malloc(sizeof(struct secs_world));
  world->components.infos = NULL;
  world->components.count = 0;
  world->components.capacity = 0;
  return world;
}

void secs_destroy_world(secs_world_id world) { free(world); }

/**
 * @brief grow an array
 * @param ptr *ptr points to start of array
 * @param capacity pointer to current capacity
 * @param stride item size
 * @param need the min capacity after growing */
static void grow_array(void** ptr, ssize* capacity, ssize stride, ssize need) {
  ssize cap2 = *capacity * 2;
  ssize new_cap = cap2 > need ? cap2 : need;
  if (new_cap < 8) new_cap = 8;
  *ptr = realloc(*ptr, new_cap * stride);
  *capacity = new_cap;
}

static u32 hash_string(struct sbase_string str) {
  u32 hash = 2166136261; /* FNV offset base */
  for (ssize i = 0; i < str.length; i++) {
    hash ^= str.data[i];
    hash *= 16777619; /* FNV prime */
  }
  return hash;
}

int secs_register_component(
    secs_world_id world, struct secs_component_descriptor const* descriptor) {
  if (world->components.count + 1 > world->components.capacity) {
    grow_array((void**)&world->components.infos, &world->components.capacity,
               sizeof(struct component_info), world->components.count + 1);
  }
  ssize i = world->components.count;
  world->components.count++;
  struct component_info* info = &world->components.infos[i];
  info->name = descriptor->name;
  info->name_hash = hash_string(descriptor->name);
  info->size = descriptor->size;
  info->id = i;
  return SECS_RESULT_SUCCESS;
}
