#pragma once

#include <spargel/base/base.h>

typedef struct spargel_ecs_world_impl* spargel_ecs_world;
typedef struct spargel_ecs_view spargel_ecs_view;

typedef void (*spargel_ecs_view_callback_t)(spargel_ecs_view* view, void* data);

struct spargel_ecs_spawn_desc {
  int num_components;
  char const** components;
  /* number of entities */
  int count;
  spargel_ecs_view_callback_t callback;
  void* callback_data;
};

struct spargel_ecs_query_desc {
  int count;
  char const** components;
  spargel_ecs_view_callback_t callback;
  void* data;
  char const* query_name;
};

struct spargel_ecs_view {
  ssize count;
  void** components;
  /* internal use */
  struct spargel_ecs_archetype* type_;
};

spargel_ecs_world spargel_ecs_create_world();
void spargel_ecs_destroy_world(spargel_ecs_world world);
void spargel_ecs_register_component(spargel_ecs_world world, char const* name,
                                    ssize size);
void spargel_ecs_spawn_entities(spargel_ecs_world world,
                                struct spargel_ecs_spawn_desc* desc);
void spargel_ecs_query(spargel_ecs_world world, struct spargel_ecs_query_desc* desc);
void spargel_ecs_view_delete(spargel_ecs_view* view, int id);
