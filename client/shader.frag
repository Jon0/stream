precision highp float;
varying vec4 world_pos;
varying vec3 world_normal;
varying vec4 color;

void main() {

	// lighting
	vec3 light_pos = vec3(500.0, 1500.0, 700.0);
	vec3 L = normalize(light_pos - world_pos.xyz);

	// todo specular
	//vec3 E = normalize(-world_pos.xyz);
	//vec3 R = normalize(-reflect(L, world_normal));

	// ambient component
	vec3 ambient = color.xyz * vec3(0.2, 0.2, 0.2);
	ambient = clamp(ambient, 0.0, 1.0);

	// diffuse component
	vec3 diffuse = color.xyz * vec3(1.0, 1.0, 1.0) * max(dot(world_normal, L), 0.0);
	diffuse = clamp(diffuse, 0.0, 1.0);

	vec3 final_color = ambient + diffuse;
	gl_FragColor = vec4(final_color, 1.0);
}
