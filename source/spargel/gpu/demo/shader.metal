#include <metal_stdlib>
using namespace metal;

struct rasterize_data {
    float4 position [[position]];
    float4 color;
};

struct vertex_data {
    float2 position [[attribute(0)]];
    float3 color [[attribute(1)]];
};

constant float2 positions[3] = {
    {0.0, -0.5},
    {0.5, 0.5},
    {-0.5, 0.5},
};

constant float4 colors[3] = {
    {1.0, 0.0, 0.0, 1.0},
    {0.0, 1.0, 0.0, 1.0},
    {0.0, 0.0, 1.0, 1.0},
};

[[vertex]] rasterize_data vertex_shader(uint vertex_id [[vertex_id]], vertex_data in [[stage_in]]) {
    rasterize_data out;
    out.position = float4(0.0, 0.0, 0.0, 1.0);
    out.position.xy = positions[vertex_id];
    out.color = colors[vertex_id];
    return out;
}

[[fragment]] float4 fragment_shader(rasterize_data in [[stage_in]]) {
  return in.color;
}
