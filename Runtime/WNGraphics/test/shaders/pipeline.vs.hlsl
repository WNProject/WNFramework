struct input {
    float4 Position: POSITION;
    float4 Texcoord: TEXCOORD;
};

struct output {
    float4 Position: SV_POSITION;
    float4 Texcoord: TEXCOORD;
};

void main(in input IN, out output OUT) {
    OUT.Position = IN.Position;
    OUT.Texcoord = IN.Texcoord;
}