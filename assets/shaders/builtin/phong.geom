#version 450

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vs_out_blk {
    vec3 position;
    vec3 normal;
    vec2 uv;
} vs_out[];

out gs_out_blk {
    vec3 position;
    vec3 normal;
    vec2 uv;
} gs_out;

out vec3 tangent;

void main() {
    vec3 a = vs_out[0].position - vs_out[1].position;
    vec3 b = vs_out[2].position - vs_out[1].position;
    tangent = (vs_out[2].uv.y - vs_out[1].uv.y) * a - (vs_out[0].uv.y - vs_out[1].uv.y) * b;
    tangent /= 
        (vs_out[0].uv.x - vs_out[1].uv.x) * (vs_out[2].uv.y - vs_out[1].uv.y) - 
        (vs_out[2].uv.x - vs_out[1].uv.x) * (vs_out[0].uv.y - vs_out[1].uv.y);
    tangent = normalize(tangent);
    for (int i = 0; i < 3; ++i) {
        gs_out.position = vs_out[i].position;
        gs_out.normal = vs_out[i].normal;
        gs_out.uv = vs_out[i].uv;
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}
