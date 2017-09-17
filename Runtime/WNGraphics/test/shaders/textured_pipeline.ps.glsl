#version 450

layout(location = 0) in vec2 texcoord;
layout(location = 0) out vec4 color;

layout(set = 0, binding = 1) uniform sampler default_sampler;
layout(set = 0, binding = 2) uniform texture2D default_texture;

void main() {
    color = texture(sampler2D(default_texture, default_sampler), texcoord);
    color.w = 1.0f;
}
