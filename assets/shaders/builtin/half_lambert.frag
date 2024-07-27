#version 450

layout(location = 0) out vec4 out_color;
layout(location = 2) out uint top_object_id;

in gs_out_blk {
    vec3 normal;
    vec2 uv;
} gs_out;

in vec3 tangent;

struct lambert_material {
    sampler2D diffuse;
    sampler2D normals;
    vec4 diffuse_color;
};

uniform uint object_id;
uniform mat3 normal_matrix;
uniform lambert_material material;

vec4 get_frag_color() {
    vec3 light_direction = vec3(-1);
    vec3 normal = normalize(normal_matrix * gs_out.normal);
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    normal = TBN * (2. * texture(material.normals, gs_out.uv).rgb - 1.);
    return material.diffuse_color * texture(material.diffuse, gs_out.uv) * (1. + dot(normalize(normal), normalize(-light_direction))) / 2.;
}

void main() {
    out_color = get_frag_color();
    top_object_id = object_id;
}