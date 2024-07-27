#version 450

layout(location = 0) out vec3 frag_color;

in vs_out_blk {
    vec3 color;
} vs_out;

void main() {
    frag_color = vs_out.color;
}