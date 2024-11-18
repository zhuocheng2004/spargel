#include <metal_stdlib>

#include "spargel/ui/metal/shader_types.h"

using namespace metal;

struct QuadVertexOutput {
  float4 position [[position]];
  float4 color;
};

vertex QuadVertexOutput quad_vertex(
    uint vertex_id [[vertex_id]],
    uint quad_id [[instance_id]],
    constant float2* vertices [[buffer(quad_input_index_vertices)]],
    constant QuadData* quads [[buffer(quad_input_index_quads)]],
    constant float2* viewport  [[buffer(quad_input_index_viewport)]]) {
  QuadVertexOutput out;

  float2 pixelSpacePosition = vertices[vertex_id] * quads[quad_id].size + quads[quad_id].origin;

  out.position = float4(0.0, 0.0, 0.0, 1.0);
  out.position.xy = pixelSpacePosition / (*viewport / 2.0);

  out.color = quads[quad_id].color;

  return out;
}

fragment float4 quad_fragment(QuadVertexOutput input [[stage_in]]) {
  return input.color;
}
