//var Pusher = require('pusher')
var Pusher = require('pusher-js/node')

APP_KEY = '4bc53c8397eb4188b5cb'
APP_CLUSTER = 'us2'

var pusher = new Pusher(APP_KEY, {
	cluster: APP_CLUSTER
});

// Subscribe to the channel
var channel = pusher.subscribe('my-channel');

channel.bind('my-event', function(data) {
  console.log('An event was triggered with message: ' + data.message);
});
