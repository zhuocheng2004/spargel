#include <spargel/ui/ui.h>

int main() {
  spargel_ui_init_platform();
  spargel_ui_window_id window = spargel_ui_create_window(500, 500);
  spargel_ui_window_set_title(window, "Spargel Editor");
  spargel_ui_platform_run();
  return 0;
}
