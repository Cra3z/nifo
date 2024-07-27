#version 450

out vec4 out_color;

uniform sampler2D src1;
uniform sampler2D src2;

in gs_out_blk {
    vec2 uv;
} gs_out;

void main() {
    vec4 color1 = texture(src1, gs_out.uv);
    vec4 color2 = texture(src2, gs_out.uv);
    out_color = mix(color1, color2, color2.a);
}

