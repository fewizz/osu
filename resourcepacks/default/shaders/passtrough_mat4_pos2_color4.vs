#version 130

uniform mat4 u_mat;
in vec2 a_position;
in vec4 a_color;
out vec4 a_color;

void main() {
	uv_color = a_color;
	gl_Position = u_mat * vec4(a_position, 0, 1);
}
