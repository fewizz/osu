#version 130

uniform mat4 u_mat;
uniform vec2 u_dim;
in vec2 a_uv;
out vec2 uv_vs;

void main() {
	vec2 positions[4] = vec2[](
		vec2(0,0),
		vec2(u_dim[0],0),
		vec2(u_dim[0],u_dim[1]),
		vec2(0,u_dim[1])
	);

	gl_Position = u_mat * vec4(positions[gl_VertexID], 0, 1);
	uv_vs = a_uv;
}
