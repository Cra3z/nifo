#version 450

layout(location = 0) in vec3 position;

uniform mat4 MV;
uniform mat4 P;

out vs_out_blk {
    vec3 uvw;
} vs_out;

void main() {
    vs_out.uvw = vec3(1.f, 1.f, -1.f) * normalize(position); // right-handed system to left-handed system
    gl_Position = (P * vec4(inverse(transpose(mat3(MV))) * position, 1.f)).xyww;
}