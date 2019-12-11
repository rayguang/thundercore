Racycore Crypto Currency Trading Engine Readme

### Description
This project is a concurrent trading message processing system, which does order execution, order management, account management as well as market info such as orderbook and ticker price.

The code base includes mainly three parts.
1. A C++ implementation of full API of 6 exchanges RESTful and websockets for Binance. These exchanges are Binance, Bitfinex, Bittrex, Bitstamp, Coinbase, Kraken
1. A C++ API wrapper that provides standard API endpoints to interact with any 3rd party code. In our case, a HTML/Javascript UI.
1. A middleware which relays the JSON message from front-end to backend trading engine. Currently this is coded in Node.JS. A GO version is under development.

### Directory Structure 

#### $HOME/API
This is the C++ implementation of exchange API. To compile the libraries, run the following:
```
$HOME/API/src/build.sh

# For example, to compile API for Kraken
thundercore/API/krakcpp/src/build.sh
```
This will build and generate the libraries which you can use for any other projects.

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

