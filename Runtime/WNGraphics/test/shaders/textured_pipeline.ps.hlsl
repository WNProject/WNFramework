struct input {
    float4 Position: SV_POSITION;
    float2 Texcoord: TEXCOORD;
};

struct output {
    float4 Color: SV_TARGET;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

void main(in input IN, out output OUT) {
    OUT.Color = g_texture.Sample(g_sampler, IN.Texcoord.xy);
    OUT.Color.w = 1.0f;
}