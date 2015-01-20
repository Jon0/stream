var Camera = function() {
	this.position = 1;

	this.pMatrix = mat4.create();
	this.vMatrix = mat4.create();
	this.mMatrix = mat4.create();
	mat4.identity(this.pMatrix);
	mat4.identity(this.vMatrix);
	mat4.identity(this.mMatrix);
	mat4.perspective(45, 800 / 600, 10.0, 10000.0, this.pMatrix);
	mat4.lookAt([0.0, 1500.0, 1500.0], [0.0, 0.0, 0.0], [0.0, 1.0, 0.0], this.vMatrix);

	this.set = function(amount) {
		mat4.identity(this.mMatrix);
		mat4.rotate(this.mMatrix, amount, [0.0, 1.0, 0.0], this.mMatrix);
	}

} 
