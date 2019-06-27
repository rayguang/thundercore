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
	
	pusher_instance.trigger(reply_channel, trade_reply_event, {
		"message": data
	});
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

// var channel_name			= 'private-trading-production-request.1'
//var request_channel 		= channel_name;;
//var reply_channel   		= channel_name;
// var trade_request_event		= 'client-request-event';
// var trade_reply_event 		= 'client-reply-event';
var uid                     = process.argv[2];
var channel_name			= 'my-channel';
var request_channel         = channel_name;
var reply_channel           = channel_name;
var trade_request_event     = 'my-event';
var trade_reply_event       = 'my-event';


const Pusher 	 = require ('pusher-js/node');
const Pusher_rep = require ('pusher');
const net  		 = require('net');
const pusher_auth = require('pusher-client');

// Listen to pusher message from front-end
//var pusher = new Pusher(APP_KEY, {
//        cluster: APP_CLUSTER
//});
//var pusher = new pusher_auth.forURL("https://4bc53c8397eb4188b5cb:cb942bd68ad1d17dc72f@api.pusherapp.com/apps/721540");
var pusher = new pusher_auth(APP_KEY,{
	secret: APP_SECRET,
	useTLS: true,
	cluster: 'us2',
	encrypted: true
}
	
);

// Broadcast trade reply to channel
var pusher_broadcast = new Pusher_rep({
	appId: APP_ID,
  	key: APP_KEY,
  	secret: APP_SECRET,
  	cluster: APP_CLUSTER,
  	useTLS: true
	//encryptionMasterKey: 'abcdefghijklmnopqrstuvwxyzabcdef'
});

// Subscribe to the channel
console.log("****** Starting middleware message relay server ******")

var privateChannel = pusher.subscribe(request_channel);
console.log("Connecting to APP: " + APP_ID);
console.log("Subscribed to PUSHER channel: "+ request_channel);
console.log("Listening trade request event: " + trade_request_event);
console.log("Listening trade reply event: " + trade_reply_event);


// Bind to event and process message
privateChannel.bind(trade_request_event, function(data, metadata) {
  console.log('Received message: ');
  console.log(
	"Received:", data
  );
  
  // Add uid to data
  data["uid"] = Number(uid);

 console.log("Modified:", data)
 

  // Data must be buffer or string
  var trading_message = JSON.stringify(data);

  var nodeClient = getConn('Node', pusher_broadcast)
  nodeClient.write(trading_message);
});

 
