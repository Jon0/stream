var Renderable = function(id) {

	this.id = id;
	this.u = 0.0;
	this.v = 0.0;
	this.w = 0.0;

	this.render = function(gl) {
		gl.camera.set(Math.random() * i);
		gl.uniformMatrix4fv(gl.mMatrixUniform, false, gl.camera.mMatrix);

		// render object
		gl.drawArrays(gl.TRIANGLES, 0, gl.triangle_count);
	}

} 