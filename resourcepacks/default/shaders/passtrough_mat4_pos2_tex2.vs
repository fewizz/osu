#version 130

uniform mat4 u_mat;
in vec2 a_pos;
in vec2 a_uv;
out vec2 uv_vs;

void main() {
	uv_vs = a_uv;
	gl_Position = u_mat * vec4(a_pos, 0, 1);
}
