#include <spargel/base/base.h>
#include <spargel/renderer/renderer.h>

spargel_renderer_id spargel_create_renderer(sui_window_id window) { sbase_panic_here(); }

spargel_renderer_texture_id spargel_renderer_add_texture(
    spargel_renderer_id renderer, struct spargel_renderer_texture_descriptor const* descriptor) {
    sbase_panic_here();
}

void spargel_renderer_start_frame(spargel_renderer_id renderer) { sbase_panic_here(); }

void spargel_renderer_add_quad(spargel_renderer_id renderer, float x, float y, float width,
                               float height, spargel_renderer_texture_id texture) {
    sbase_panic_here();
}

void spargel_renderer_render(spargel_renderer_id renderer) { sbase_panic_here(); }
