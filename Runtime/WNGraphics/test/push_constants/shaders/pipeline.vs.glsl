#version 450

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 itexcoord;
layout(location = 0) out vec4 otexcoord;

layout(push_constant) uniform push_constants {
    vec2 offset;
};

void main() {
    gl_Position = position;
    gl_Position.xy += offset;
    gl_Position.y *= -1;
    otexcoord = itexcoord;
}
