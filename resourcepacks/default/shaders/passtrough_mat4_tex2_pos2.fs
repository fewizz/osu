#version 130

uniform sampler2D u_atlas;
in vec2 uv_vs;

void main() {
	gl_FragColor = texture(u_atlas, uv_vs);
}
