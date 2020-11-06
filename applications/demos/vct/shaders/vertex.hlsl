struct input {
    [[vk::location(0)]]
    float3 Position: POSITION;
    [[vk::location(1)]]
    float3 Normal : NORMAL;
    [[vk::location(2)]]
    float2 Texcoord: TEXCOORD;
    [[vk::location(3)]]
    float4 ITangent: TANGENT;
};

struct output {
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

[[vk::binding(0, 0)]]
cbuffer cb : register(b0)
{
    float4 offset;
};

[[vk::binding(0, 1)]]
cbuffer cb2 : register(b20)
{
    column_major float4x4 projection;
    column_major float4x4 view;
    column_major float4x4 normal_object;
    float4 light_position;
};

void main(in input IN, out output OUT) {
    OUT.Position = float4(IN.Position.xyz + offset.xyz, 1);
    OUT.Position = mul(projection, mul(view, OUT.Position));

    OUT.Texcoord = IN.Texcoord;

    OUT.OTangent = IN.ITangent.xyz;
    OUT.BTangent = cross(IN.Normal, IN.ITangent.xyz)* IN.ITangent.w;

    float3x3 nv = normal_object;
    float3x3 normal_basis = float3x3(OUT.BTangent, OUT.OTangent, IN.Normal);

    OUT.Texcoord = IN.Texcoord;
    OUT.OTangent = normal_basis[0];
    OUT.BTangent = normal_basis[1];
    OUT.Normal = normal_basis[2];
}