#include <stdio.h>

#include "modules/platform/public/platform.h"
#include "modules/window/public/window.h"

int main() {
  auto platform = spargel::platform::platform::create();
  platform->init();
  printf("debug: xxx\n");
  auto window = spargel::window::window::create({
      .backend = spargel::window::backend_kind::cocoa,
      .width = 500,
      .height = 500,
  });
  window->set_title("Spargel Engine");
  while(true) {
    platform->poll_event();
  }
  return 0;
}
