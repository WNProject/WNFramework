struct input {
    float4 Position: SV_POSITION;
    float4 Texcoord: TEXCOORD;
};

struct output {
    float4 Color: SV_TARGET;
};

void main(in input IN, out output OUT) {
    OUT.Color = float4(IN.Texcoord.x, IN.Texcoord.y, IN.Texcoord.z, 1.0);
}