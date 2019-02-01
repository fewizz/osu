#version 130

in float tex_x;

void main() {
	float x = abs(tex_x*2.0-1.0);
	gl_FragColor = vec4(x, x, x, 1.0);
}
