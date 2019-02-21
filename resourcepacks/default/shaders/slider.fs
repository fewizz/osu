#version 130

in float tex_x;
uniform vec4 u_color;

void main() {
	//float x = abs(tex_x*2.0-1.0);
	gl_FragColor = vec4(tex_x, tex_x, tex_x, 1.0) * u_color;
}
