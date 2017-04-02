#version 450

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 itexcoord;
layout(location = 0) out vec4 otexcoord;

void main() {
    gl_Position = position * vec4(1.0, -1.0, 1.0, 1.0);
    otexcoord = itexcoord;
}
