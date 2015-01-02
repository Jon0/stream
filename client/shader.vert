attribute vec4 a_Position;

uniform mat4 uPMatrix;

void main() {
	gl_Position = uPMatrix * a_Position;
}
