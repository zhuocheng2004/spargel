#pragma once

#include <spargel/base/base.h>
#include <spargel/ui/ui.h>

typedef struct spargel_renderer* spargel_renderer_id;
typedef int spargel_renderer_texture_id;

/**
 * @brief create a renderer
 */
spargel_renderer_id spargel_create_renderer(sui_window_id window);

enum spargel_renderer_texture_format {
    TEXTURE_FORMAT_B8G8R8A8_UNORM,
};

/* todo: format; currently it is BGRA8 (unorm) */
struct spargel_renderer_texture_descriptor {
    int width;
    int height;
    u8* data;
};

/**
 * @brief register a texture
 */
spargel_renderer_texture_id spargel_renderer_add_texture(
    spargel_renderer_id renderer, struct spargel_renderer_texture_descriptor const* descriptor);

void spargel_renderer_start_frame(spargel_renderer_id renderer);

/**
 * @brief textured quad
 *
 * coordinate:
 *
 */
void spargel_renderer_add_quad(spargel_renderer_id renderer, float x, float y, float width,
                               float height, spargel_renderer_texture_id texture);

void spargel_renderer_render(spargel_renderer_id renderer);
