#include <spargel/renderer/shader_types_mtl.h>
#include <metal_stdlib>

using namespace metal;

struct rasterize_data {
  float4 position [[position]];
  float2 texture_coord;
  // int texture_id;
};

vertex rasterize_data vertex_shader(
  uint vertex_id [[vertex_id]],
  uint quad_id [[instance_id]],
  constant float2* vertices [[buffer(SPARGEL_VERTEX_INPUT_INDEX_VERTICES)]],
  constant quad_data* quads [[buffer(SPARGEL_VERTEX_INPUT_INDEX_QUADS)]],
  constant float2* viewport [[buffer(SPARGEL_VERTEX_INPUT_INDEX_VIEWPORT)]]) {

  rasterize_data out;

  float2 pixel_position = vertices[vertex_id] * quads[quad_id].size + quads[quad_id].origin;
  // float2 pixel_position = vertices[vertex_id];

  out.position = float4(0.0, 0.0, 0.0, 1.0);
  out.position.xy = pixel_position / (*viewport / 2.0);
  out.texture_coord = vertices[vertex_id];
  // out.texture_id = quads[quad_id].texture;

  return out;
}

fragment float4 fragment_shader(
  rasterize_data in [[stage_in]],
  texture2d<half> color_texture [[texture(SPARGEL_TEXTURE_INDEX_BASE_COLOR)]]) {

  constexpr sampler texture_sampler(mag_filter::linear, min_filter::linear);

  const half4 color_sample = color_texture.sample(texture_sampler, in.texture_coord);

  return float4(color_sample);
}
