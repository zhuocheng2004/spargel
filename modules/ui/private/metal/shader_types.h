#pragma once

#include <simd/simd.h>

struct quad_data {
  simd::float2 origin;
  simd::float2 size;
  simd::float4 color;
};

enum {
  quad_input_index_vertices = 0,
  quad_input_index_quads = 1,
  quad_input_index_viewport = 2,
};
