#version 150

layout (std140)
uniform
view {
    mat4 camera, projection;
};

in vec3 frag_position;
in vec4 vertex_color;

out vec4 fragColor;

const vec3 light_direction = vec3(0.408248, -0.816497, -0.408248);

void main() {
//    vec3 normal = normalize(frag_normal);

//    float shade = 0.5 * (-dot(normal, normalize(light_direction)) + 1.0);
    fragColor = vertex_color; // vec4(color.rgb * shade, color.a);
}