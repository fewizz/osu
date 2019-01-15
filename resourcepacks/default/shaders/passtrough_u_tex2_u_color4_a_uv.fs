#version 130

uniform sampler2D u_tex;
in vec2 uv_vs;
uniform vec4 u_color;

void main() {
	gl_FragColor = texture(u_tex, uv_vs) * u_color;
}
