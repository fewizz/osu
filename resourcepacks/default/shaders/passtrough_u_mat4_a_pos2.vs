#version 130

uniform mat4 u_mat;
in vec2 a_pos;

void main() {
	gl_Position = u_mat * vec4(a_pos, 0, 1);
}
