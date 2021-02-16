#version 150

uniform mat4 transform = mat4(1.0);

layout (std140)
uniform
view {
    mat4 camera, projection;
};

in vec3 position;
in vec4 color;

out vec3 frag_position;
out vec4 vertex_color;

void main() {
    mat4 modelview = camera * transform;
    vec4 eye_position = modelview * vec4(position, 1.0);
    gl_Position = projection * eye_position;
    frag_position = eye_position.xyz;
    vertex_color = color;
}