#version 130

uniform sampler2D u_tex;
in vec2 uv_vs;

void main() {
	gl_FragColor = texture(u_tex, uv_vs);
}
