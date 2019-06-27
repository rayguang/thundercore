// Import net module.
var net = require('net');

// This function create and return a net.Socket object to represent TCP client.
function getConn(connName){

    var option = {
        host:'localhost',
        port: 1234
    }

    // Create TCP client.
    var client = net.createConnection(option, function () {
        console.log('Connection name : ' + connName);
        console.log('Local address : ' + client.localAddress + ":" + client.localPort);
        console.log('Remote address : ' + client.remoteAddress + ":" + client.remotePort);
    });

    client.setTimeout(1000);
    client.setEncoding('utf8');

    // When receive server send back data.
    client.on('data', function (data) {
        console.log('Server response : ' + data);
    });

    // When connection disconnected.
    client.on('end',function () {
        console.log('NodeJS client socket disconnect. ');
    });

    client.on('timeout', function () {
        console.log('NodeJS client connection timeout. ');
    });

    client.on('error', function (err) {
        console.error(JSON.stringify(err));
    });

    return client;
}

// Create a java client socket.

// Create node client socket.
var nodeClient = getConn('Node');


nodeClient.write('Trading Request : BUY MARKET 100 ETHBTC ');
