#version 450

layout(location = 0) out vec4 frag_color;

in vs_out_blk {
    vec3 uvw;
} vs_out;

uniform samplerCube skyboxmap;

void main() {
    frag_color = texture(skyboxmap, vs_out.uvw);
}