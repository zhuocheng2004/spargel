#include <spargel/codec/codec.h>
#include <spargel/gpu/gpu.h>
#include <spargel/renderer/renderer.h>
#include <spargel/ui/ui.h>

static void render(spargel_renderer_id renderer) {
  spargel_renderer_start_frame(renderer);
  spargel_renderer_add_quad(renderer, 0, 0, 100, 100, 0);
  spargel_renderer_add_quad(renderer, -200, -200, 150, 150, 0);
  spargel_renderer_render(renderer);
}

int main(int argc, char* argv[]) {
  // if (argc < 2) return 1;
  spargel_ui_init_platform();
  spargel_ui_window_id window = spargel_ui_create_window(500, 500);
  spargel_ui_window_set_title(window, "Spargel Editor");
  sgpu_instance_id instance;
#if __APPLE__
  sgpu_create_instance(&(struct sgpu_instance_descriptor){SGPU_BACKEND_METAL},
                       &instance);
#else
  sgpu_create_instance(&(struct sgpu_instance_descriptor){SGPU_BACKEND_VULKAN},
                       &instance);
#endif
  spargel_renderer_id renderer = spargel_create_renderer(window);
  struct spargel_codec_image image;
  spargel_codec_load_ppm_image(argv[1], &image);
  struct spargel_renderer_texture_descriptor texture_desc = {
      image.width, image.height, (u8*)image.pixels};
  spargel_renderer_add_texture(renderer, &texture_desc);
  spargel_ui_window_set_render_callback(window, (void (*)(void*))render,
                                        renderer);
  spargel_ui_platform_run();
  return 0;
}
