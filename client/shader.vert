attribute vec4 vert_pos;
attribute vec4 vert_color;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

varying vec4 color;

void main() {
	color = vert_color;
	gl_Position = projection * view * model * vert_pos;
}
