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

void main(in input IN, out output OUT) {
    OUT.Color = IN.Color;
}
