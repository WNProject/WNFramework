
struct input {
    [[vk::location(0)]]
    float3 Normal: NORMAL;
    [[vk::location(1)]]
    float2 Texcoord: TEXCOORD;
    [[vk::location(2)]]
    float4 Position: SV_POSITION;
    [[vk::location(3)]]
    float3 OTangent: OTANGENT;
    [[vk::location(4)]]
    float3 BTangent: BTANGENT;
};

struct output {
    [[vk::location(0)]]
    float4 Color: SV_TARGET;
};

[[vk::binding(0, 1)]]
cbuffer cb2 : register(b20)
{
    column_major float4x4 projection;
    column_major float4x4 view;
    column_major float4x4 normal_object;
    float4 light_position;
};



#ifdef HAS_DIFFUSE
[[vk::binding(1)]]
SamplerState g_diffuse_sampler : register(s0);
[[vk::binding(2)]]
Texture2D g_diffuse_tex : register(t0);
#endif

#ifdef HAS_SPEC
[[vk::binding(3)]]
SamplerState g_spec_sampler : register(s1);
[[vk::binding(4)]]
Texture2D g_spec_tex : register(t1);
#endif

#ifdef HAS_MASK
[[vk::binding(5)]]
SamplerState g_mask_sampler : register(s2);
[[vk::binding(6)]]
Texture2D g_mask_tex : register(t2);
#endif

#ifdef HAS_NORMAL
[[vk::binding(7)]]
SamplerState g_normal_sampler : register(s3);
[[vk::binding(8)]]
Texture2D g_normal_tex : register(t3);
#endif

void main(in input IN, out output OUT) {
    OUT.Color.x = 0.5;
    OUT.Color.y = 0.5;
    OUT.Color.z = 0.5;
    OUT.Color.w = 1.0f;

    float3 l = -normalize(light_position.xyz);
    float3 n = normalize(IN.Normal);

#ifdef HAS_MASK
    if (g_mask_tex.Sample(g_mask_sampler, IN.Texcoord.xy).x < 0.1) {
        discard;
    }
#endif

#ifdef HAS_NORMAL
    float3 norm = normalize(2.0 * (g_normal_tex.Sample(g_normal_sampler, IN.Texcoord.xy).xyz) - 1.0);
    float3x3 tf = transpose(float3x3(IN.BTangent, IN.OTangent, IN.Normal));
    n = mul(tf, norm);
#endif

    float ndotl = max(dot(n, l), 0.07);
#ifdef HAS_DIFFUSE
    OUT.Color = g_diffuse_tex.Sample(g_diffuse_sampler, IN.Texcoord.xy);
#endif
    OUT.Color *= ndotl;
}