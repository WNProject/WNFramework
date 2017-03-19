struct input {
    float4 Position: Position;
};

struct output {
    float4 Position: SV_Position;
};

void main(in input IN, out output OUT) {
    OUT.Position = IN.Position;
}