#include <spargel/gpu/gpu.h>
#include <spargel/renderer/renderer.h>
#include <spargel/ui/ui.h>

import spargel.codec;

static void render(spargel_renderer_id renderer) {
  spargel_renderer_start_frame(renderer);
  spargel_renderer_add_quad(renderer, 0, 0, 100, 100, 0);
  spargel_renderer_add_quad(renderer, -200, -200, 150, 150, 0);
  spargel_renderer_render(renderer);
}

int main(int argc, char* argv[]) {
  if (argc < 2) return 1;
  spargel_ui_init_platform();
  spargel_ui_window_id window = spargel_ui_create_window(500, 500);
  spargel_ui_window_set_title(window, "Spargel Editor");

#if __APPLE__
  int gpu_backend = SGPU_BACKEND_METAL;
#else
  int gpu_backend = SGPU_BACKEND_VULKAN;
#endif
  sgpu_device_id device;
  sgpu_create_default_device(gpu_backend, &device);

  struct spargel_codec_image image;
  spargel_codec_load_ppm_image(argv[1], &image);
  struct spargel_renderer_texture_descriptor texture_desc = {
      image.width, image.height, (u8*)image.pixels};

  spargel_renderer_id renderer = spargel_create_renderer(window);
  spargel_renderer_add_texture(renderer, &texture_desc);

  spargel_ui_window_set_render_callback(window, (void (*)(void*))render,
                                        renderer);

  spargel_ui_platform_run();
  return 0;
}
