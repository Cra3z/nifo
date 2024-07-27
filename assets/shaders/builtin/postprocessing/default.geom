#version 450

layout(triangles) in;
layout(triangle_strip, max_vertices = 6) out;

uniform vec4 rect; 

out gs_out_blk {
    vec2 uv;
} gs_out;

void main() {
    gs_out.uv = vec2(rect[0], rect[1] + rect[3]);
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    gs_out.uv = vec2(rect[0], rect[1]);
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    gs_out.uv = vec2(rect[0] + rect[2], rect[1]);
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    EndPrimitive();

    gs_out.uv = vec2(rect[0] + rect[2], rect[1]);
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    gs_out.uv = vec2(rect[0] + rect[2], rect[1] + rect[3]);
    gl_Position = gl_in[2].gl_Position + (gl_in[0].gl_Position - gl_in[1].gl_Position);
    EmitVertex();
    gs_out.uv = vec2(rect[0], rect[1] + rect[3]);
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    EndPrimitive();
}