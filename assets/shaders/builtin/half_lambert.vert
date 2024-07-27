#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

uniform mat4 MVP;

out vs_out_blk {
    vec3 position;
    vec3 normal;
    vec2 uv;
} vs_out;

void main() {
    vs_out.position = position;
    vs_out.normal = normal;
    vs_out.uv = uv;
    gl_Position = MVP * vec4(position, 1.f);
}