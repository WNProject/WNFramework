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
    float2 offset;
};

[[vk::push_constant]]
ConstantBuffer<constants> push_constants: register(b0, space0);

void main(in input IN, out output OUT, out float4 pos: SV_POSITION) {
    pos = float4(IN.Position + push_constants.offset, 0.0, 1.0);
    OUT.Color = IN.Color;
    OUT.Texcoord = IN.Texcoord;
}
