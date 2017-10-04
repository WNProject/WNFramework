struct input {
    float4 Position: POSITION;
    float4 Texcoord: TEXCOORD;
};

struct output {
    float4 Position: SV_POSITION;
    float4 Texcoord: TEXCOORD;
};

struct PushConstants {
    float2 offset;
};

ConstantBuffer<PushConstants> push_constants: register(b0, space0);

void main(in input IN, out output OUT) {
    OUT.Position = IN.Position;
    OUT.Position.xy += push_constants.offset;
    OUT.Texcoord = IN.Texcoord;
}