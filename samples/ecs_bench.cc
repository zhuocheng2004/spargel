
#include <stdio.h>

#include <chrono>

#include "ecs.h"

struct position {
  float x;
  float y;
  float z;
};

struct velocity {
  float vx;
  float vy;
  float vz;
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
    pos[i].x += vel[i].vx * dt;
    pos[i].y += vel[i].vy * dt;
    pos[i].z += vel[i].vz * dt;
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

void init_entities(ecs_view* view, void* data) {
  struct position* pos = (struct position*)(view->components[0]);
  struct velocity* vel = (struct velocity*)(view->components[1]);
  for (int i = 0; i < view->count; i++) {
    pos[i].x = i;
    pos[i].y = i - 1;
    pos[i].z = i + 1;
    vel[i].vx = 1.0;
    vel[i].vy = 0.9;
    vel[i].vz = 1.1;
  }
}

int main() {
  using namespace std::chrono;

  ecs_world world = ecs_create_world();

  ecs_register_component(world, "position", sizeof(struct position));
  ecs_register_component(world, "velocity", sizeof(struct velocity));

  //printf("info: component registered\n");

  static char const* components[] = {"position", "velocity"};
  struct ecs_spawn_desc desc;
  desc.num_components = 2;
  desc.components = components;
  desc.count = 0x100000;
  desc.callback = init_entities;
  desc.callback_data = 0;
  ecs_spawn_entities(world, &desc);

  //printf("info: entities spawned\n");

  auto t0 = high_resolution_clock::now();

  struct global_data globals;
  globals.dt = 0.01;
  for (int i = 0; i < 0x100U; i++) {
    movement_system(world, &globals);
  }
  auto t1 = high_resolution_clock::now();
  auto time_run = t1 - t0;
  printf("time_run:\t %f\n", duration_cast<milliseconds>(time_run).count() / 1000.0f);

  ecs_destroy_world(world);
  return 0;
}
