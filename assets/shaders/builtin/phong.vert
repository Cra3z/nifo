#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

uniform mat4 model_matrix;

layout(std140, binding = 0) uniform camera_uniforms {
    vec3 eye;
    mat4 view_projection_matrix;
};
 
out vs_out_blk {
    vec3 position;
    vec3 normal;
    vec2 uv;
} vs_out;

void main() {
    mat4 MVP = view_projection_matrix * model_matrix;
    vs_out.position = vec3(model_matrix * vec4(position, 1.));
    vs_out.normal = normal;
    vs_out.uv = uv;
    gl_Position = MVP * vec4(position, 1.f);
}