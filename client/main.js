// appends recieved text

var source = new EventSource("/stream"); 
source.onopen = function() {
	console.log("connection opened");
}

source.onerror = function() {
	console.log("connection error");
}

source.onmessage = function(event) {
	console.log("recieved");
	console.log(event.data);
    document.getElementById("result").innerHTML += event.data + "<br>";
};

