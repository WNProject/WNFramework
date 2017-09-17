#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 itexcoord;

layout(location = 0) out vec2 otexcoord;


layout (set = 0, binding = 0) uniform inputs {
    vec2 offset;
};

void main() {
    gl_Position = vec4(position + offset, 0.0f, 1.0f) * vec4(1.0f, -1.0f, 1.0f, 1.0f);
    otexcoord = itexcoord;
}
