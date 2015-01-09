attribute vec4 vert_pos;
attribute vec3 vert_norm;
attribute vec4 vert_color;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

varying vec4 world_pos;
varying vec3 world_normal;
varying vec4 color;

void main() {
	world_pos = model * vert_pos;
	world_normal = (model * vec4(vert_norm, 0.0)).xyz;
	color = vert_color;
	gl_Position = projection * view * model * vert_pos;
}
