struct input {
    float4 Position: SV_Position;
};

struct output {
    float4 Color: SV_Target;
};

void main(in input IN, out output OUT) {
    OUT.Color = float4(0.0, 0.0, 0.0, 1.0);
}