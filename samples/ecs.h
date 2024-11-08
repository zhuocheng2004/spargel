#ifndef ECS_H_
#define ECS_H_

#include <stddef.h>

typedef struct ecs_world_impl* ecs_world;
typedef struct ecs_view ecs_view;

typedef void (*ecs_system_t)(ecs_world world, void* data);
typedef void (*ecs_view_callback_t)(ecs_view* view, void* data);

struct ecs_spawn_desc {
  int num_components;
  char const** components;
  // number of entities;
  int count;
  ecs_view_callback_t callback;
  void* callback_data;
};

struct ecs_query_desc {
  int count;
  char const** components;
  ecs_view_callback_t callback;
  void* data;
};

struct ecs_view {
  size_t count;
  void** components;
};

#ifdef __cplusplus
extern "C" {
#endif

ecs_world ecs_create_world();
void ecs_destroy_world(ecs_world world);
void ecs_register_component(ecs_world world, char const* name, size_t size);
void ecs_spawn_entities(ecs_world world, struct ecs_spawn_desc* desc);
void ecs_query(ecs_world world, struct ecs_query_desc* desc);

#ifdef __cplusplus
}
#endif

#endif
