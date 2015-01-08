attribute vec4 vert_pos;
attribute vec3 vert_norm;
attribute vec4 vert_color;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

varying vec3 normal;
varying vec4 color;

void main() {
	normal = vert_norm;

	// lighting
	vec3 light_pos = vec3(3.0, 7.0, 1.0);
	vec3 L = normalize(light_pos - vert_pos.xyz);
	vec3 E = normalize(-vert_pos.xyz);
	vec3 R = normalize(-reflect(L, vert_norm));


	// diffuse component
	vec4 diffuse = vert_color * max(dot(vert_norm, L), 0.0);
	diffuse = clamp(diffuse, 0.0, 1.0);

	color = diffuse;
	gl_Position = projection * view * model * vert_pos;
}
