#version 450

layout(location = 0) in vec3 position;

uniform mat4 MVP;

void main() {
    gl_Position = MVP * vec4(mat3(1.015625) * position, 1.);
}