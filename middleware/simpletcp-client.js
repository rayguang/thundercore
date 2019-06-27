var net = require ('net');

PORT = 1234

var client = new net.Socket();
client.connect(PORT,'127.0.0.1', function(){
    console.log('NodeJS connected');
    client.write('Write NodeJS test message.');
});

client.on('data',function(data){
    console.log('Received: ' + data);
});

client.on('close', function(){
    console.log('Connection closed.')
});

