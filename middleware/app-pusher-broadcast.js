var Pusher = require('pusher')
//var Pusher = require('pusher-js/node')

APP_ID = '721540'
APP_KEY = '4bc53c8397eb4188b5cb'
APP_SECRET = 'cb942bd68ad1d17dc72f'
APP_CLUSTER = 'us2'

pusher = new Pusher({
  appId: APP_ID,
  key: APP_KEY,
  secret: APP_SECRET,
  cluster: APP_CLUSTER,
  useTLS: true
});

pusher.trigger('my-channel', 'my-event', {"message": "hello world"});

