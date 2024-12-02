#include <spargel/renderer/renderer.h>
#include <spargel/base/base.h>

spargel_renderer_id spargel_create_renderer(spargel_ui_window_id window)
{
    sbase_unreachable();
}

spargel_renderer_texture_id spargel_renderer_add_texture(
    spargel_renderer_id renderer,
    struct spargel_renderer_texture_descriptor const* descriptor)
{
    sbase_unreachable();
}

void spargel_renderer_start_frame(spargel_renderer_id renderer)
{
    sbase_unreachable();
}

void spargel_renderer_add_quad(spargel_renderer_id renderer, float x, float y,
                               float width, float height,
                               spargel_renderer_texture_id texture)
{
    sbase_unreachable();
}

void spargel_renderer_render(spargel_renderer_id renderer)
{
    sbase_unreachable();
}
