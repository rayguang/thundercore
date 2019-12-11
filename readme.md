Racycore Crypto Currency Trading Engine Readme

### Description
This project is a concurrent trading message processing system, which does order execution, order management, account management as well as market info such as orderbook and ticker price.

The code base includes mainly three parts.
1. A C++ implementation of full API of 6 exchanges RESTful and websockets for Binance. These exchanges are Binance, Bitfinex, Bittrex, Bitstamp, Coinbase, Kraken
1. A C++ API wrapper that provides standard API endpoints to interact with any 3rd party code. In our case, a HTML/Javascript UI.
1. A middleware which relays the JSON message from front-end to backend trading engine. Currently this is coded in Node.JS. A GO version is under development.

### Todo
1. Review firewall rules on the trading host 35.199.2.70. For example only white list the pusher server and exchange api servers, the rest traffic in/to any other IPs should be blocked.
1. Implement past trades for the exchange, which shows the trades filled of the whole exchange. This is can be done either via a public REST request or websocket.
1. Extend websocket project. I have build websocket for Binance orderbook (C++) and coinbase pro/kraken in python for websocket ticker as demo code. Either of them can be extended to other exchanges with full websocket for orderbook, ticker price and real time filled trades.
1. Activate all the API functions for front-end. The current UI only implements a small portion of the APIs that provided by the backend. It can be easily extended with more sophiscated features, such as aggregated stats and analytics as well as advanced trading algorithm. 

### Directory Structure 
Home Directory:
/home/ray/dev/trading_execution

#### $HOME/API
This is the C++ implementation of exchange API. To compile the libraries, run

$HOME/API/src/build.sh

#### $HOME/include
This is where the header files are stored.

#### $HOME/lib
This is where the compiled libraries are stored.

#### $HOME/middleware
This is where the middleware program are stored.

#### $HOME/src/
This is where the trading engine API wrapper and utilities code stored.

$HOME/src/trading: all the trading engine API wrapper stored

#### $HOME/test
This is where the test code are stored.

#### $HOME/trade_signals
This is where the trade signals are stored.

#### Process to add new exchange
1. build new exchange API $HOME/API
1. add compiled lib to $HOME/lib
1. add headers to $HOME/include
1. add new interface in main.cpp
1. add new exchange path in run.template.sh
1. add exchange API wrapper in src/trading/modules/exchange_name

