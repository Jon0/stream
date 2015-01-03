// HelloTriangle.js (c) 2012 matsuda

// main setup function
function setup_webgl() {
	// Retrieve <canvas> element
	var canvas = document.getElementById('webgl');

	// Get the rendering context for WebGL
	var gl = canvas.getContext("experimental-webgl");
	if (!gl) {
		console.log('Failed to get the rendering context for WebGL');
		return null;
	}

	// Initialize shaders
	var vert;
	var frag;

	// todo: init shaders async, or use ajax
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
	gl.triangle_count = initVertexBuffers(gl);
	if (gl.triangle_count < 0) {
		console.log('Failed to set the positions of the vertices');
		return;
	}

	gl.pMatrix = mat4.create();
	gl.vMatrix = mat4.create();
	gl.mMatrix = mat4.create();
	mat4.identity(gl.pMatrix);
	mat4.identity(gl.vMatrix);
	mat4.identity(gl.mMatrix);
	mat4.perspective(45, 600 / 800, 0.1, 100.0, gl.pMatrix);
	mat4.lookAt([0.0, 5.0, 5.0], [0.0, -1.0, 0.0], [0.0, 1.0, 0.0], gl.vMatrix);

	return gl;
}

// global setup
var gl = setup_webgl();

// main rendering function
function render() {
	if (!gl) {
		return;
	}

	// update render state
	//gl.mMatrix.rotate(1.0, 0.0, 1.0, 0.0);
	mat4.rotate(gl.mMatrix, 0.1, [0.0, 1.0, 0.0], gl.mMatrix);


	gl.viewport(0, 0, 600, 400);

	// Specify the color for clearing <canvas>
	gl.clearColor(0, 0, 0, 1);

	// Clear <canvas>
	gl.clear(gl.COLOR_BUFFER_BIT);

	// Set uniform values
	gl.uniformMatrix4fv(gl.pMatrixUniform, false, gl.pMatrix);
	gl.uniformMatrix4fv(gl.vMatrixUniform, false, gl.vMatrix);
	gl.uniformMatrix4fv(gl.mMatrixUniform, false, gl.mMatrix);

	// Draw the rectangle
	gl.drawArrays(gl.TRIANGLES, 0, gl.triangle_count);
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

	gl.pMatrixUniform = gl.getUniformLocation(shaderProgram, "projection");
	gl.vMatrixUniform = gl.getUniformLocation(shaderProgram, "view");
	gl.mMatrixUniform = gl.getUniformLocation(shaderProgram, "model");

	gl.useProgram(shaderProgram);

	// hacks
	gl.program = shaderProgram;

	return true;
}

function setupShader(gl, shader, source) {
	gl.shaderSource(shader, source);
	gl.compileShader(shader);

	if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
		//console.log(gl.getShaderInfoLog(shader));
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
	var obj = load_obj("cube.obj");
	console.log(obj);


	var vertices_loaded = [
		0.0, 0.5, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0,
		-0.5, -0.5, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0,
		0.5, -0.5, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0
	];


	var vertices = new Float32Array(obj);
	var n = obj.length / 8; // The number of vertices

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

	var vert_pos = gl.getAttribLocation(gl.program, 'vert_pos');
	var vert_color = gl.getAttribLocation(gl.program, 'vert_color');
	if (vert_pos < 0) {
		console.log('Failed to get the storage location of vert_pos');
		return -1;
	}

	// Assign the buffer object to a_Position variable
	gl.vertexAttribPointer(vert_pos, 4, gl.FLOAT, false, 8*4, 0);
	gl.vertexAttribPointer(vert_color, 4, gl.FLOAT, false, 8*4, 4*4);

	// Enable the assignment to a_Position variable
	gl.enableVertexAttribArray(vert_pos);
	gl.enableVertexAttribArray(vert_color);

	return n;
}

var source = new EventSource("/stream"); 
source.onopen = function() {
	console.log("connection opened");
}

source.onerror = function() {
	console.log("connection error");
}

source.onmessage = function(event) {
	console.log("recieved: "+event.data);
	render();
    document.getElementById("result").innerHTML += event.data + "<br>";
};
