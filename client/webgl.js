// HelloTriangle.js (c) 2012 matsuda

// main rendering function
function main() {
	// Retrieve <canvas> element
	var canvas = document.getElementById('webgl');

	// Get the rendering context for WebGL
	var gl = canvas.getContext("experimental-webgl");
	if (!gl) {
		console.log('Failed to get the rendering context for WebGL');
		return;
	}

	// Initialize shaders
	var vert;
	var frag;
	jQuery.ajaxSetup({async:false});
	jQuery.get('/shader.vert', function(data) {
		vert = data;
	});
	jQuery.get('/shader.frag', function(data) {
		frag = data;
	});

	if (!initShaders(gl, vert, frag)) {
		console.log('Failed to intialize shaders.');
		return;
	}

	// Write the positions of vertices to a vertex shader
	var n = initVertexBuffers(gl);
	if (n < 0) {
		console.log('Failed to set the positions of the vertices');
		return;
	}

	var pMatrix = mat4.create();
	//mat4.identity(pMatrix);
	mat4.perspective(45, 600 / 800, 0.1, 100.0, pMatrix);

	//gl.viewport(0, 0, gl.viewportWidth, gl.viewportHeight);

	// Specify the color for clearing <canvas>
	gl.clearColor(0, 0, 0, 1);

	// Clear <canvas>
	gl.clear(gl.COLOR_BUFFER_BIT);

	// Set uniform values
	gl.uniformMatrix4fv(gl.pMatrixUniform, false, pMatrix);

	// Draw the rectangle
	gl.drawArrays(gl.TRIANGLES, 0, n);
}



function initShaders(gl, vert_source, frag_source) {
	var vertexShader = gl.createShader(gl.VERTEX_SHADER);
	var fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);

	// set source
	setupShader(gl, vertexShader, vert_source);
	setupShader(gl, fragmentShader, frag_source);

	shaderProgram = gl.createProgram();
	gl.attachShader(shaderProgram, vertexShader);
	gl.attachShader(shaderProgram, fragmentShader);
	gl.linkProgram(shaderProgram);

	if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
		console.log("Could not initialise shaders");
	}

	gl.pMatrixUniform = gl.getUniformLocation(shaderProgram, "uPMatrix");

	gl.useProgram(shaderProgram);

	// hacks
	gl.program = shaderProgram;

	return true;
}

function setupShader(gl, shader, source) {
	gl.shaderSource(shader, source);
	gl.compileShader(shader);

	if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
		console.log(gl.getShaderInfoLog(shader));
	}
}

function getShader(gl, id) {
	var shaderScript = document.getElementById(id);
	if (!shaderScript) {
		return null;
	}

	var str = "";
	var k = shaderScript.firstChild;
	while (k) {
		if (k.nodeType == 3)
			str += k.textContent;
		k = k.nextSibling;
	}

	var shader;
	if (shaderScript.type == "x-shader/x-fragment") {
		shader = gl.createShader(gl.FRAGMENT_SHADER);
	} else if (shaderScript.type == "x-shader/x-vertex") {
		shader = gl.createShader(gl.VERTEX_SHADER);
	} else {
		return null;
	}

	return setupShader(gl, shader, str);
}


function initVertexBuffers(gl) {
	var vertices = new Float32Array([
		0, 5.0, -8.0, 1.0,
		-5.0, -5.0, -8.0, 1.0,
		5.0, -5.0, -8.0, 1.0
	]);
	var n = 3; // The number of vertices

	// Create a buffer object
	var vertexBuffer = gl.createBuffer();
	if (!vertexBuffer) {
		console.log('Failed to create the buffer object');
		return -1;
	}

	// Bind the buffer object to target
	gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);
	// Write date into the buffer object
	gl.bufferData(gl.ARRAY_BUFFER, vertices, gl.STATIC_DRAW);

	var a_Position = gl.getAttribLocation(gl.program, 'a_Position');
	if (a_Position < 0) {
		console.log('Failed to get the storage location of a_Position');
		return -1;
	}

	// Assign the buffer object to a_Position variable
	gl.vertexAttribPointer(a_Position, 4, gl.FLOAT, false, 0, 0);

	// Enable the assignment to a_Position variable
	gl.enableVertexAttribArray(a_Position);

	return n;
}

main();