#include <spargel/codec/codec.h>
// #include <spargel/renderer/renderer.h>
#include <spargel/ui/ui.h>

// static void render(spargel_renderer_id renderer) {
//     spargel_renderer_start_frame(renderer);
//     spargel_renderer_add_quad(renderer, 0, 0, 100, 100, 0);
//     spargel_renderer_add_quad(renderer, -200, -200, 150, 150, 0);
//     spargel_renderer_render(renderer);
// }

int main(int argc, char* argv[]) {
    if (argc < 2) return 1;
    spargel::ui::init_platform();
    spargel::ui::window_id window = spargel::ui::create_window(500, 500);
    spargel::ui::window_set_title(window, "Spargel Editor");

    struct spargel_codec_image image;
    spargel_codec_load_ppm_image(argv[1], &image);
    // struct spargel_renderer_texture_descriptor texture_desc = {image.width, image.height,
    //                                                            (u8*)image.pixels};

    // spargel_renderer_id renderer = spargel_create_renderer(window);
    // spargel_renderer_add_texture(renderer, &texture_desc);

    // spargel::ui::window_set_render_callback(window, (void (*)(void*))render, renderer);

    spargel::ui::platform_run();
    return 0;
}
