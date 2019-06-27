// Ray Guang
// Feb 2019
// This is to build a middle layer NodeJS app
// The middle layer does the following network communication: 
//      1) listen to pusher message broadcast from front-end UI;
//      2) relay the message to C++ trading engine via socket
//      3) listen to the reply sent back from C++ trading engine
//      4) broadcast the message back to front-end via pusher

// Trade engine setup
TRADE_HOST = 'localhost';
TRADE_PORT = 5678;

// PUSHER env setup
require('dotenv').config({path: '../.env'});
APP_ID = `${process.env.APP_ID_TRADING_PRD}`
APP_KEY = `${process.env.APP_KEY_TRADING_PRD}`
APP_SECRET = `${process.env.APP_SECRET_TRADING_PRD}`
APP_CLUSTER = `${process.env.APP_CLUSTER_TRADING_PRD}`

// Channel setup
var request_channel 		= 'my-channel'; 
var reply_channel   		= 'trade-reply';
var trade_request_event		= 'my-event';
var trade_reply_event 		= 'order-reply';

const Pusher 	 = require ('pusher-js/node');
const Pusher_rep = require ('pusher');
const net  		 = require('net');
//var Pusher = require('pusher-js/node');
//var Pusher_rep = require('pusher');
//var net = require('net');

// Listen to pusher message from front-end
var pusher = new Pusher(APP_KEY, {
        cluster: APP_CLUSTER
});

// Broadcast trade reply to channel
var pusher_broadcast = new Pusher_rep({
	appId: APP_ID,
  	key: APP_KEY,
  	secret: APP_SECRET,
  	cluster: APP_CLUSTER,
  	useTLS: true
});

// Setup DB connection
DB_HOST = `${process.env.BACK_DBHOST}`
DB_USER = `${process.env.BACK_DBUSER}`
DB_PASSWORD = `{process.env.BACK_DBPASSWORD}`
DB_DATABASE = `{process.env.BACK_DBNAME}`
/*const mysql = require('mysql');
var con = mysql.createConnection({
	host: DB_HOST,
	user: DB_USER,
	password: DB_PASSWORD,
	database: DB_DATABASE
});


con.connect(function(err){
	if (err) throw err;
	console.log ("Connected to backend DB.");
});

con.end()
*/
// Subscribe to the channel
console.log("****** Starting middleware message relay server ******")

var channel = pusher.subscribe(request_channel);
console.log("Connecting to APP: " + APP_ID);
console.log("Subscribed to PUSHER channel: "+ request_channel);
console.log("Listening trade request event: " + trade_request_event);
console.log("Listening trade reply event: " + trade_reply_event);


// chunk longer message to bypass Pusher limit 
function triggerChunked(pusher, channel, event, data) {
    const chunkSize = 1000;  // artificially small! Set it to more like 9000
    const str = JSON.stringify(data);
    const msgId = JSON.parse(data)["ctRequestId"] + '';
    for (var i = 0; i*chunkSize < str.length; i++) {
      // TODO: use pusher.triggerBatch for better performance
      pusher.trigger(channel, "chunked-" + event, { 
        id: msgId, 
        index: i, 
        chunk: str.substr(i*chunkSize, chunkSize), 
        final: chunkSize*(i+1) >= str.length
      });
    }
  }

// This function create and return a net.Socket object to represent TCP client.
function getConn(connName, pusher_instance){

    var option = {
        host: TRADE_HOST,
        port: TRADE_PORT
    }

    // Create TCP client.
    var client = net.createConnection(option, function () {
        console.log('Connection name : ' + connName);
        console.log('Local address : ' + client.localAddress + ":" + client.localPort);
        console.log('Remote address : ' + client.remoteAddress + ":" + client.remotePort);
    });

    client.setTimeout(5000);
    client.setEncoding('utf8');

    // When receive server send back data.
    client.on('data', function (data) {
        console.log('Server response : ' + data);
    
    // commit out to test chunk
	// pusher_instance.trigger('my-channel', trade_reply_event, {
	// 	"message": data
    // });

    try {
		triggerChunked(pusher_instance, 'my-channel', trade_reply_event, data);
	} catch (err) {
		console.log(err);
	}
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


// Bind to event and process message
channel.bind(trade_request_event, function(data) {
  console.log('Received message: ');
  console.log(data);
  
  // Data must be buffer or string
  var trading_message = JSON.stringify(data);

  var nodeClient = getConn('Node', pusher_broadcast)
  nodeClient.write(trading_message);
});

 
