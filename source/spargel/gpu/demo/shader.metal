#include <metal_stdlib>
using namespace metal;

struct rasterize_data {
    float4 position [[position]];
    float4 color;
};

struct vertex_data {
    float2 position [[attribute(0)]];
    float4 color [[attribute(1)]];
};

[[vertex]] rasterize_data vertex_shader(uint vertex_id [[vertex_id]], vertex_data in [[stage_in]]) {
    rasterize_data out;
    out.position = float4(0.0, 0.0, 0.0, 1.0);
    out.position.xy = in.position;
    out.color = in.color;
    return out;
}

[[fragment]] float4 fragment_shader(rasterize_data in [[stage_in]]) {
  return in.color;
}
