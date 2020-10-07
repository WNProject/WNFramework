struct input {
    [[vk::location(0)]]
    float2 Texcoord : TEXCOORD;
    [[vk::location(1)]]
    float4 Color    : COLOR;
};

struct output {
    [[vk::location(0)]]
    float4 Color: SV_TARGET;
};

[[vk::binding(0)]]
SamplerState g_sampler : register(s0);
[[vk::binding(1)]]
Texture2D g_texture : register(t0);

void main(in input IN, out output OUT) {
    OUT.Color = g_texture.Sample(g_sampler, IN.Texcoord.xy) * IN.Color;
}
