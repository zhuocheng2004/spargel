#include <metal_stdlib>

#include "modules/ui/private/metal/shader_types.h"

using namespace metal;

struct quad_vertex_output {
  float4 position [[position]];
  float4 color;
};

vertex quad_vertex_output quad_vertex(
    uint vertex_id [[vertex_id]],
    uint quad_id [[instance_id]],
    constant float2* vertices [[buffer(quad_input_index_vertices)]],
    constant quad_data* quads [[buffer(quad_input_index_quads)]],
    constant uint2* viewport  [[buffer(quad_input_index_viewport)]]) {
  quad_vertex_output out;

  float2 pixelSpacePosition = vertices[vertex_id] * quads[quad_id].size + quads[quad_id].origin;
  float2 viewportSize = float2(*viewport);

  out.position = float4(0.0, 0.0, 0.0, 1.0);
  out.position.xy = pixelSpacePosition / (viewportSize / 2.0);

  out.color = quads[quad_id].color;

  return out;
}

fragment float4 quad_fragment(quad_vertex_output input [[stage_in]]) {
  return input.color;
}
