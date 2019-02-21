#version 130

uniform mat4 u_mat;
uniform vec2 u_pos;
uniform float u_r;

out vec2 vs_pos;

void main() {
	vec2 poss[4] = vec2[](
		vec2(-u_r, -u_r),
		vec2(u_r, -u_r),
		vec2(-u_r, u_r),
		vec2(u_r, u_r)
	);
	vs_pos = poss[gl_VertexID];
	gl_Position = u_mat * vec4(poss[gl_VertexID] + u_pos, 0, 1);
}
