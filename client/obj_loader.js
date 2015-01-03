// read split a text line and return array of floats
function get_float_values(line) {
	var float_values = [];
	var str_values = line.split(" ");
	for (val = 0; val < str_values.length; ++val) {
		if (str_values[val].length > 0) {
			float_values.push(parseFloat(str_values[val]));
		}
	}
	return float_values;
}

function get_vert_values(line, verts) {
	var vert_values = [];
	var str_values = line.split(" ");
	for (val = 0; val < str_values.length; ++val) {
		if (str_values[val].length > 0) {
			var components = str_values[val].split("/");
			if (components.length == 3) {
				vert_values = vert_values.concat(verts[parseInt(components[0])]);
				vert_values.push(1.0);

				// color
				vert_values.push(1.0);
				vert_values.push(0.0);
				vert_values.push(1.0);
				vert_values.push(1.0);
			}
		}
	}
	return vert_values;
}

function load_obj(filename) {

	// get obj file
	var obj_file;
	jQuery.ajax({
		url: '/'+filename,
		success: function(file) {
			obj_file = file;
		},
		async: false
	});

	var next_ind = 1;
	var indexed_verts = {};
	var obj_result = [];

	// read file lines
	var lines = obj_file.split('\n');
	for (index = 0; index < lines.length; ++index) {
		var line = lines[index];

		// the vertex positions
		if  (line.substr(0, 2) == 'vn') {
			//something = get_float_values(line.substr(2));
		}
		else if (line.substr(0, 1) == 'v') {
			indexed_verts[next_ind++] = get_float_values(line.substr(1));
		}
		else if (line.substr(0, 1) == 'f') {
			var vert = get_vert_values(line.substr(1), indexed_verts);
			obj_result = obj_result.concat(vert);
		}
	}

	return obj_result;

}
