#version 450

layout(location = 0) out vec4 out_color;
layout(location = 2) out uint top_object_id;

struct phong_material {
    sampler2D diffuse;
    sampler2D normals;
    sampler2D specular;
    vec4 ambient_color;
    vec4 diffuse_color;
    vec4 specular_color;
    float specular_intensity;
    float shininess;
};

struct parallel_light {
    vec3 direction;
    vec4 ambient_color;
    vec4 diffuse_color;
    vec4 specular_color;
};

in gs_out_blk {
    vec3 position;
    vec3 normal;
    vec2 uv;
} gs_out;

in vec3 tangent;

uniform uint object_id;
uniform mat3 normal_matrix;
uniform vec3 eye;
uniform phong_material material;

float lambert(vec3 normal, vec3 light_diretion) {
    return max(dot(normalize(normal), normalize(-light_diretion)), 0);
}

float blinn_spec(vec3 normal, vec3 light_direction, vec3 eye, vec3 position, float shininess) {
    vec3 halfway = normalize(normalize(-light_direction) + normalize(eye - position));
    return pow(max(dot(normalize(normal), halfway), 0), shininess);
}

vec4 get_frag_color() {
    parallel_light light = parallel_light(
        vec3(-1, -1, -1),
        vec4(0.2),
        vec4(0.7),
        vec4(0.3)
    );
    vec3 normal = normalize(normal_matrix * gs_out.normal);
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    normal = TBN * (2. * texture(material.normals, gs_out.uv).rgb - 1.);
    vec4 ambient = material.ambient_color * texture(material.diffuse, gs_out.uv) * light.ambient_color;
    vec4 diffuse = material.diffuse_color * texture(material.diffuse, gs_out.uv) * lambert(normal, light.direction) * light.diffuse_color;
    vec4 specular = material.specular_intensity * material.specular_color * texture(material.specular, gs_out.uv) * blinn_spec(normal, light.direction, eye, gs_out.position, material.shininess) * light.specular_color;
    return ambient + diffuse + specular;
}

void main() {
    out_color = get_frag_color();
    top_object_id = object_id;
}