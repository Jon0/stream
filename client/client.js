// http://binaryjs.com/

var client = BinaryClient('ws://localhost:9000');
client.on('open', function(stream){
  var stream = client.createStream({file: 'hello.txt'});
  stream.write('Hello');
  stream.write('World!');
  stream.end();
});
