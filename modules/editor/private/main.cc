#include "modules/ui/public/platform.h"
#include "modules/ui/public/window.h"

int main() {
  auto platform = spargel::ui::platform::create();
  platform->init();
  auto window = platform->create_window();
  window->init(500, 500);
  window->set_title("Spargel Editor");
  platform->run();
  return 0;
}
