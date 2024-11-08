#include <stdbool.h>
#include <stdio.h>

#include "ecs.h"

struct position {
  float x;
};

struct velocity {
  float v;
};

struct global_data {
  float dt;
};

void movement_update(ecs_view* view, void* data) {
  struct global_data* globals = (struct global_data*)data;
  float dt = globals->dt;

  struct position* pos = (struct position*)(view->components[0]);
  struct velocity* vel = (struct velocity*)(view->components[1]);
  for (int i = 0; i < view->count; i++) {
    pos[i].x += vel[i].v * dt;
  }
}

void movement_system(ecs_world world, void* data) {
  static char const* components[] = {"position", "velocity"};

  struct ecs_query_desc desc;
  desc.count = 2;
  desc.components = components;
  desc.callback = movement_update;
  desc.data = data;

  ecs_query(world, &desc);
}

void print_callback(ecs_view* view, void* data) {
  struct position* pos = (struct position*)(view->components[0]);
  for (int i = 0; i < view->count; i++) {
    printf("position = %.2f\n", pos[i].x);
  }
}

void print_system(ecs_world world, void* data) {
  printf("========\n");

  static char const* components[] = {"position"};

  struct ecs_query_desc desc;
  desc.count = 1;
  desc.components = components;
  desc.callback = print_callback;
  desc.data = 0;

  ecs_query(world, &desc);
}

void init_entities(ecs_view* view, void* data) {
  struct position* pos = (struct position*)(view->components[0]);
  struct velocity* vel = (struct velocity*)(view->components[1]);
  for (int i = 0; i < view->count; i++) {
    pos[i].x = i;
    vel[i].v = 1.0;
  }
}

int main() {
  ecs_world world = ecs_create_world();

  ecs_register_component(world, "position", sizeof(struct position));
  ecs_register_component(world, "velocity", sizeof(struct velocity));

  printf("info: component registered\n");

  static char const* components[] = {"position", "velocity"};
  struct ecs_spawn_desc desc;
  desc.num_components = 2;
  desc.components = components;
  desc.count = 10;  // spawn 10 entities
  desc.callback = init_entities;
  desc.callback_data = 0;
  ecs_spawn_entities(world, &desc);

  printf("info: entities spawned\n");

  struct global_data globals;

  globals.dt = 1.0;

  print_system(world, 0);
  movement_system(world, &globals);
  globals.dt = 2;
  print_system(world, 0);
  movement_system(world, &globals);
  print_system(world, 0);

  ecs_destroy_world(world);
  return 0;
}
