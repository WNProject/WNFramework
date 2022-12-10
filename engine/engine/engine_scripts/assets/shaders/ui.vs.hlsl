#include "include/renderpass.hlsl"

struct input {
  [[vk::location(0)]]
  float2 Position: POSITION;
  [[vk::location(1)]]
  float4 Color:    COLOR;
  [[vk::location(2)]]
  float2 Texcoord: TEXCOORD;
};

struct output {
    [[vk::location(0)]]
    float2 Texcoord : TEXCOORD;
    [[vk::location(1)]]
    float4 Color    : COLOR;
};

struct constants {
    float2 flip_multiplier;
    float2 offset;
};

[[vk::push_constant]]
ConstantBuffer<constants> push_constants: register(b0, space0);

void main(in input IN, out output OUT, out float4 pos: SV_POSITION) {
    //float2 normalized_pos = (() * float2(2.0f, -2.0f)) - float2(1.0f, 1.0f);//((( * float2(2.0, -2.0)) * - 1.0f) * push_constants.flip_multiplier;
    float2 normalized_pos = ((((IN.Position + push_constants.offset) / rp_constants.renderpass_size) * 2.0f - 1.0f) * float2(1.0f, -1.0f)) * push_constants.flip_multiplier;
    pos = float4(normalized_pos, 0.0, 1.0);
    OUT.Color = IN.Color;
    OUT.Texcoord = IN.Texcoord;
}
