struct input {
    float2 Position: POSITION;
    float2 Texcoord: TEXCOORD;
};

struct output {
    float4 Position: SV_POSITION;
    float2 Texcoord: TEXCOORD;
};

cbuffer cb : register(b0)
{
    float2 offset;
};

void main(in input IN, out output OUT) {
    OUT.Position = float4(IN.Position.xy + offset , 0, 1);
    OUT.Texcoord = IN.Texcoord;
}