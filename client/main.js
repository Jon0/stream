// http://localhost:8080/

var source = new EventSource("http://www.w3schools.com/html/demo_sse.php"); 
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

