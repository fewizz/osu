#version 130

uniform float u_r;
uniform vec4 u_color;
uniform vec2 u_normal;
in vec2 vs_pos;

void main() {
	if(dot(u_normal, vs_pos) < 0) {
		gl_FragColor = vec4(0);
		return;
	}
	float x = length(vs_pos) / u_r;
	float a = 1;
	if(x > 1){
		x = 0;
		a = 0;
	}
	gl_FragColor = vec4(x, x, x, a) * u_color;
}
