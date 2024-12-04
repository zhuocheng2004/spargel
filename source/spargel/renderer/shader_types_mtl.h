#pragma once

/* platform */
#include <simd/simd.h>

enum spargel_vertex_input_index {
    SPARGEL_VERTEX_INPUT_INDEX_VERTICES = 0,
    SPARGEL_VERTEX_INPUT_INDEX_QUADS = 1,
    SPARGEL_VERTEX_INPUT_INDEX_VIEWPORT = 2,
};

enum spargel_texture_index {
    SPARGEL_TEXTURE_INDEX_BASE_COLOR = 0,
};

struct quad_data {
    simd_float2 origin;
    simd_float2 size;
    int texture;
};
