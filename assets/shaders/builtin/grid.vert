#version 450

layout(location = 0) in vec3 position;

uniform mat4 view_projection_matrix;

out vs_out_blk {
    vec3 color;
} vs_out;

void main() {
    if (position.y == 1.) {
        vs_out.color = 0.32 * vec3(1.);
    }
    else if (position.y == 2.) {
        vs_out.color = 0.40 * vec3(1.);
    }
    else if (position.y == 4.) {
        vs_out.color = vec3(0.72, 0.21, 0.2);
    }
    else if (position.y == 8.) {
        vs_out.color = vec3(0.27, 0.45, 0.7);
    }
    gl_Position = view_projection_matrix * vec4(position.x, 0., position.z, 1.);
}