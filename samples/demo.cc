#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <cstdlib>
#include <ctime>

#include "ecs.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <nanovg.h>
#define NANOVG_GL2_IMPLEMENTATION
#include <nanovg_gl.h>
#include <nanovg_perf.h>

struct window_context {
  GLFWwindow* window = nullptr;
  bool w_pressed = false;
  bool a_pressed = false;
  bool s_pressed = false;
  bool d_pressed = false;
};

struct graphics_context {
  NVGcontext* vg;
  PerfGraph fps;
};

struct global_data {
  window_context* wctx;
  graphics_context* gctx;
  float dt;
  float win_width;
  float win_height;
  ecs_world world;
  bool died = false;
};

struct position {
  float x;
  float y;
};

struct velocity {
  float vx;
  float vy;
};

struct player {
  int blood;
};

struct renderable {
  NVGcolor color;
};


void movement_system(ecs_world world, global_data* data) {
  static char const* components[] = {"position", "velocity"};
  ecs_query_desc desc;
  desc.count = 2;
  desc.components = components;
  desc.callback = [](ecs_view* view, void* data) {
    auto ctx = (global_data*)data;
    float dt = ctx->dt;
    auto pos = (position*)(view->components[0]);
    auto vel = (velocity*)(view->components[1]);

    for (int i = 0; i < view->count; i++) {
      pos[i].x += vel[i].vx * dt;
      pos[i].y += vel[i].vy * dt;
    }
  };
  desc.data = data;
  desc.query_name = "movement/query";
  ecs_query(world, &desc);
}

void bounce_system(ecs_world world, global_data* data) {
  static char const* components[] = {"position", "velocity"};
  ecs_query_desc desc;
  desc.count = 2;
  desc.components = components;
  desc.callback = [](ecs_view* view, void* data) {
    auto ctx = (global_data*)data;
    auto width = ctx->win_width;
    auto height = ctx->win_height;

    auto pos = (position*)(view->components[0]);
    auto vel = (velocity*)(view->components[1]);

    for (int i = 0; i < view->count; i++) {
      float x = pos[i].x;
      float y = pos[i].y;
      if (x < 0) {
        vel[i].vx = abs(vel[i].vx);
      }
      if (x > width) {
        vel[i].vx = -abs(vel[i].vx);
      }
      if (y < 0) {
        vel[i].vy = abs(vel[i].vy);
      }
      if (y > height) {
        vel[i].vy = -abs(vel[i].vy);
      }
    }
  };
  desc.data = data;
  desc.query_name = "bounce/query";

  ecs_query(world, &desc);
}

constexpr int half_size = 5;

void render_system(ecs_world world, void* data) {
  static char const* components[] = {"position", "renderable"};

  ecs_query_desc desc;
  desc.count = 2;
  desc.components = components;
  desc.callback = [](ecs_view* view, void* data) {
    auto ctx = (global_data*)data;
    auto vg = ctx->gctx->vg;

    auto pos = (position*)(view->components[0]);
    auto r = (renderable*)(view->components[1]);

    for (int i = 0; i < view->count; i++) {
      auto x = pos[i].x;
      auto y = pos[i].y;
      nvgBeginPath(vg);
      nvgRect(vg, x - half_size, y - half_size, 10, 10);
      nvgFillColor(vg, r[i].color);
      nvgFill(vg);
    }
  };
  desc.data = data;
  desc.query_name = "render/query";

  ecs_query(world, &desc);
}

void player_velocity_system(ecs_world world, window_context* data) {
  static char const* components[] = {"player", "velocity"};

  ecs_query_desc desc;
  desc.count = 2;
  desc.components = components;
  desc.callback = [](ecs_view* view, void* data) {
    auto wctx = (window_context*)data;
    auto vel = (velocity*)(view->components[1]);

    if (wctx->a_pressed) {
      vel->vx = -50;
    } else if (wctx->d_pressed) {
      vel->vx = 50;
    } else {
      vel->vx = 0;
    }
    if (wctx->w_pressed) {
      vel->vy = -50;
    } else if (wctx->s_pressed) {
      vel->vy = 50;
    } else {
      vel->vy = 0;
    }
  };
  desc.data = data;
  desc.query_name = "player_velocity/query";

  ecs_query(world, &desc);
}

void player_restrict_system(ecs_world world, global_data* data) {
  static char const* components[] = {"position", "player"};

  ecs_query_desc desc;
  desc.count = 2;
  desc.components = components;
  desc.callback = [](ecs_view* view, void* data) {
    global_data* ctx = (global_data*)data;
    float width = ctx->win_width;
    float height = ctx->win_height;

    auto pos = (position*)(view->components[0]);
    float x = pos->x;
    float y = pos->y;
    pos->x = fmin(pos->x, width);
    pos->x = fmax(pos->x, 0);
    pos->y = fmin(pos->y, height);
    pos->y = fmax(pos->y, 0);
  };
  desc.data = data;
  desc.query_name = "player_restrict/query";

  ecs_query(world, &desc);
}

void render_blood_system(ecs_world world, global_data* data) {
  static char const* components[] = {"player"};
  ecs_query_desc desc;
  desc.count = 1;
  desc.components = components;
  desc.callback = [](ecs_view* view, void* data) {
    auto ctx = (global_data*)data;
    auto vg = ctx->gctx->vg;
    auto p = (player*)view->components[0];

    char str[64];
    if (ctx->died) {
      snprintf(str, 64, "You died.");
    } else {
      snprintf(str, 64, "Blood: %d", p->blood);
    }

    nvgBeginPath(vg);
    nvgFontFace(vg, "sans");
    nvgFontSize(vg, 16.0f);
    nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);
    nvgFillColor(vg, nvgRGB(255, 0, 0));
    nvgText(vg, ctx->win_width - 10, 10, str, NULL);
  };
  desc.data = data;
  desc.query_name = "render_blood/query";

  ecs_query(world, &desc);
}

struct battle_loop_data {
  player* p;
  position* pos;
  global_data* globals;
};

bool intersect(position* p1, position* p2) {
  constexpr int full_size = 2 * half_size;
  if (p1->x + full_size <= p2->x || p2->x + full_size <= p1->x) {
    return false;
  }
  if (p1->y + full_size <= p2->y || p2->y + full_size <= p1->y) {
    return false;
  }
  return true;
}

void battle_system(ecs_world world, global_data* data) {
  static char const* components[] = {"player", "position"};
  ecs_query_desc desc;
  desc.count = 2;
  desc.components = components;
  desc.callback = [](ecs_view* view, void* data) {
    auto ctx = (global_data*)data;
    auto p = (player*)view->components[0];
    auto pos = (position*)view->components[1];

    battle_loop_data loop_data{p, pos, ctx};

    static char const* components[] = {"enemy", "position"};
    ecs_query_desc desc;
    desc.count = 2;
    desc.components = components;
    desc.callback = [](ecs_view* view, void* data) {
      auto loop_data = (battle_loop_data*)data;
      auto pos = (position*)view->components[1];
      for (int i = 0; i < view->count; i++) {
        if (intersect(loop_data->pos, pos + i)) {
          if (!loop_data->globals->died) {
            loop_data->p->blood--;
            loop_data->pos->x = 300;
            loop_data->pos->y = 300;
            if (loop_data->p->blood == 0) {
              loop_data->globals->died = true;
            }
            break;
          }
        }
      }
    };
    desc.data = &loop_data;
    desc.query_name = "battle_loop/query";

    ecs_query(ctx->world, &desc);
  };
  desc.data = data;
  desc.query_name = "battle/query";

  ecs_query(world, &desc);
}

struct game {
  bool init() {
    if (!init_glfw()) return false;
    if (!init_window()) return false;
    if (!init_graphics()) return false;
    if (!init_ecs()) return false;
    register_components();
    spawn_entities();
    return true;
  }
  bool init_glfw() {
    if (!glfwInit()) {
      printf("error: cannot init glfw\n");
      return false;
    }
    glfwSetErrorCallback(glfw_error_callback);
    return true;
  }
  bool init_window() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SAMPLES, 4);
    wctx.window = glfwCreateWindow(window_width, window_height,
                                   "Spargel Engine", nullptr, nullptr);
    if (!wctx.window) {
      printf("error: cannot create window\n");
      return false;
    }
    glfwSetWindowUserPointer(wctx.window, &wctx);
    glfwSetKeyCallback(wctx.window, glfw_key_callback);
    glfwMakeContextCurrent(wctx.window);
    return true;
  }
  bool init_graphics() {
    gladLoadGL(glfwGetProcAddress);
    gctx.vg = nvgCreateGL2(NVG_STENCIL_STROKES | NVG_DEBUG);
    if (!gctx.vg) {
      printf("error; cannot init nanovg\n");
      return false;
    }
    auto result =
        nvgCreateFont(gctx.vg, "sans", "../samples/Roboto-Regular.ttf");
    if (result == -1) {
      printf("error: cannot load font\n");
      return false;
    }
    initGraph(&gctx.fps, GRAPH_RENDER_FPS, "Frame Time");
    return true;
  }
  bool init_ecs() {
    world = ecs_create_world();
    return true;
  }
  void register_components() {
    ecs_register_component(world, "position", sizeof(position));
    ecs_register_component(world, "velocity", sizeof(velocity));
    ecs_register_component(world, "renderable", sizeof(renderable));
    ecs_register_component(world, "player", sizeof(player));
    ecs_register_component(world, "enemy", 0);
  }
  void spawn_entities() {
    spawn_player();
    spawn_enemies();
  }
  void spawn_enemies() {
    static char const* components[] = {"position", "velocity", "renderable",
                                       "enemy"};
    ecs_spawn_desc desc;
    desc.num_components = 4;
    desc.components = components;
    desc.count = 10;  // spawn 10 entities
    desc.callback = [](ecs_view* view, void* data) {
      position* pos = (position*)(view->components[0]);
      velocity* vel = (velocity*)(view->components[1]);
      renderable* r = (renderable*)(view->components[2]);

      for (int i = 0; i < view->count; i++) {
        auto id = i + 1;
        pos[i].x = id * 20;
        pos[i].y = id * 20;
        vel[i].vx = 1.0 * 10;
        vel[i].vy = id * 10;
        r[i].color = nvgRGB(255, 0, 0);
      }
    };
    desc.callback_data = 0;
    ecs_spawn_entities(world, &desc);
  }
  void spawn_player() {
    static char const* player_components[] = {"position", "player",
                                              "renderable", "velocity"};
    ecs_spawn_desc desc;
    desc.num_components = 4;
    desc.components = player_components;
    desc.count = 1;
    desc.callback = [](ecs_view* view, void* data) {
      position* pos = (position*)(view->components[0]);
      player* p = (player*)(view->components[1]);
      renderable* r = (renderable*)(view->components[2]);
      auto vel = (velocity*)(view->components[3]);

      pos->x = 300;
      pos->y = 300;
      p->blood = 5;
      r->color = nvgRGB(0, 255, 0);
      vel->vx = 0;
      vel->vy = 0;
    };
    desc.callback_data = 0;
    ecs_spawn_entities(world, &desc);
  }

  void deinit() {
    ecs_destroy_world(world);
    nvgDeleteGL2(gctx.vg);
    glfwDestroyWindow(wctx.window);
    glfwTerminate();
  }

  void run() {
    glfwSwapInterval(1);
    glfwSetTime(0);
    double t0 = glfwGetTime();

    global_data globals;
    globals.wctx = &wctx;
    globals.gctx = &gctx;
    globals.world = world;

    while (!glfwWindowShouldClose(wctx.window)) {
      double t = glfwGetTime();
      auto dt = t - t0;
      t0 = t;

      updateGraph(&gctx.fps, dt);

      int win_width;
      int win_height;
      int fb_width;
      int fb_height;
      glfwGetWindowSize(wctx.window, &win_width, &win_height);
      glfwGetFramebufferSize(wctx.window, &fb_width, &fb_height);
      float px_ratio = (float)fb_width / (float)win_width;

      globals.dt = dt;
      globals.win_width = win_width;
      globals.win_height = win_height;

      player_velocity_system(world, &wctx);
      movement_system(world, &globals);
      player_restrict_system(world, &globals);
      bounce_system(world, &globals);

      battle_system(world, &globals);

      glViewport(0, 0, fb_width, fb_height);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
              GL_STENCIL_BUFFER_BIT);

      nvgBeginFrame(gctx.vg, win_width, win_height, px_ratio);

      render_system(world, &globals);
      render_blood_system(world, &globals);

      renderGraph(gctx.vg, 5, 5, &gctx.fps);

      nvgEndFrame(gctx.vg);

      glfwSwapBuffers(wctx.window);
      glfwPollEvents();
    }
  }

  static void glfw_error_callback(int e, const char* msg) {
    printf("error: glfw: %s\n", msg);
  }
  static void glfw_key_callback(GLFWwindow* window, int key, int scancode,
                                int action, int mods) {
    auto ctx = (window_context*)glfwGetWindowUserPointer(window);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (key == GLFW_KEY_A) {
      if (action == GLFW_PRESS) {
        ctx->a_pressed = true;
      }
      if (action == GLFW_RELEASE) {
        ctx->a_pressed = false;
      }
    }
    if (key == GLFW_KEY_S) {
      if (action == GLFW_PRESS) {
        ctx->s_pressed = true;
      }
      if (action == GLFW_RELEASE) {
        ctx->s_pressed = false;
      }
    }
    if (key == GLFW_KEY_D) {
      if (action == GLFW_PRESS) {
        ctx->d_pressed = true;
      }
      if (action == GLFW_RELEASE) {
        ctx->d_pressed = false;
      }
    }
    if (key == GLFW_KEY_W) {
      if (action == GLFW_PRESS) {
        ctx->w_pressed = true;
      }
      if (action == GLFW_RELEASE) {
        ctx->w_pressed = false;
      }
    }
  }

  static constexpr int window_width = 500;
  static constexpr int window_height = 500;
  window_context wctx;
  graphics_context gctx;
  ecs_world world;
};

int main() {
  srand(time(nullptr));

  game g;
  if (!g.init()) {
    printf("error: failed to init\n");
    return 1;
  }
  g.run();
  g.deinit();
  return 0;
}