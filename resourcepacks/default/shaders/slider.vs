#version 130

uniform mat4 u_mat;
in vec2 a_pos;
out float tex_x;

void main() {
	tex_x = gl_VertexID & 0x1;
	gl_Position = u_mat * vec4(a_pos, 0, 1);
}
