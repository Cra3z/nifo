#version 450

layout(location = 0) out vec4 out_color;
layout(location = 2) out uint top_object_id;

struct lambert_material {
    sampler2D diffuse;
    sampler2D normals;
    vec4 diffuse_color;
};

struct parallel_light {
    vec3 direction;
    vec4 color;
    float diffuse;
    float specular;
};

struct point_light {
    vec3 position;
    vec4 color;
    float power;
    float diffuse;
    float specular;
};

struct spot_light {
    vec3 position;
    vec3 direction;
    vec4 color;
    float diffuse;
    float specular;
    float inner_div_outer;
    float outer_cutoff;
};

layout(std430, binding = 0) buffer parallel_light_buffer {
    parallel_light parallel_lights[];
};

layout(std430, binding = 1) buffer point_lights_buffer {
    point_light point_lights[];
};

layout(std430, binding = 2) buffer spot_lights_buffer {
    spot_light spot_lights[];
};

layout(std140, binding = 0) uniform camera_uniforms {
    vec3 eye;
    mat4 view_projection_matrix;
};

in gs_out_blk {
    vec3 position;
    vec3 normal;
    vec2 uv;
} gs_out;

in vec3 tangent;

uniform uint object_id;
uniform mat3 normal_matrix;
uniform lambert_material material;

vec4 gamma_to_linear(vec4 color) {
    return pow(color, vec4(2.2));
}

vec4 tone_mapping(vec4 color) {
    return 1.019 * color / (color + 0.155);
}

float lambert(vec3 normal, vec3 light_diretion) {
    return (dot(normalize(normal), normalize(-light_diretion)) + 1.) / 2.;
}

float get_attenuation(float dist) {
    float linear = 0.;
    float quadratic = 0.;

    if (dist <= 7) {linear = 0.7; quadratic = 1.8;}
    else if (dist <= 13) {linear = 0.35; quadratic = 0.44;}
    else if (dist <= 20) {linear = 0.22; quadratic = 0.20;}
    else if (dist <= 32) {linear = 0.14; quadratic = 0.07;}
    else if (dist <= 50) {linear = 0.09; quadratic = 0.032;}
    else if (dist <= 65) {linear = 0.07; quadratic = 0.017;}
    else if (dist <= 100) {linear = 0.045; quadratic = 0.0075;}
    else if (dist <= 160) {linear = 0.027; quadratic = 0.0028;}
    else if (dist <= 200) {linear = 0.022; quadratic = 0.0019;}
    else if (dist <= 325) {linear = 0.014; quadratic = 0.0007;}
    else if (dist <= 600) {linear = 0.007; quadratic = 0.0002;}
    else if (dist <= 3250) {linear = 0.0014; quadratic = 0.000007;}
    else return 0.;

    return 1. / (1. + linear * dist + quadratic * dist * dist);
}

void main() {
    vec3 normal = normalize(normal_matrix * gs_out.normal);
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    normal = TBN * (2. * texture(material.normals, gs_out.uv).rgb - 1.);

    vec4 sampled_diffuse = gamma_to_linear(texture(material.diffuse, gs_out.uv));

    for (uint i = 0; i < parallel_lights.length(); ++i) {
        vec4 diffuse = material.diffuse_color * sampled_diffuse * lambert(normal, parallel_lights[i].direction) * parallel_lights[i].diffuse * parallel_lights[i].color;

        out_color += diffuse;
    }


    for (uint i = 0; i < point_lights.length(); ++i) {

        const vec3 direction = gs_out.position - point_lights[i].position;
        const float attenuation = point_lights[i].power > 0. ? get_attenuation(length(direction) / point_lights[i].power) : 0.;

        vec4 diffuse = material.diffuse_color * sampled_diffuse * lambert(normal, direction) * point_lights[i].diffuse * point_lights[i].color;

        out_color += attenuation * diffuse;
    }

    for (uint i = 0; i < spot_lights.length(); ++i) {
        const vec3 light_direction = gs_out.position - spot_lights[i].position;

        vec4 diffuse = material.diffuse_color * sampled_diffuse * lambert(normal, light_direction) * spot_lights[i].diffuse * spot_lights[i].color;

        float cos_theta = dot(normalize(light_direction), normalize(spot_lights[i].direction));

        float cos_outer = cos(radians(spot_lights[i].outer_cutoff));

        float cos_inner = cos(radians(spot_lights[i].inner_div_outer * spot_lights[i].outer_cutoff));

        float intensity = clamp((cos_theta - cos_outer) / (cos_inner - cos_outer), 0., 1.);

        out_color += intensity * diffuse;
    }

    out_color = tone_mapping(out_color);

    top_object_id = object_id;
}