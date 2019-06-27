#
# See howto.txt
#
CC=g++

TRADE_DIR=../
SRC=./modules
UTIL=$(SRC)/../../util
ROOTINCLUDE=../../include
SYSINCLUDE=/usr/include

# Load libxxx.so, e.g., libbinacpp.so
CPPFLAGS=-Wno-multichar -Wall -lPocoUtil -lPocoNet -lPocoData -lPocoFoundation -lPocoNetSSL -lPocoData -lPocoDataMySQL -lPocoJSON -lcurl  -lcrypto -lwebsockets -lbinacpp -lcrypto++ -lbitxcpp -lbitscpp -lkrakcpp -lcoincpp
#LDFLAGS=-lm -ldl -lpthread -DNDEBUG
LDFLAGS=-lm -ldl -lpthread -DDEBUG

# logger
libspdlog_include=$(ROOTINCLUDE)/spdlog

# lib root directory trade_execution/lib
libcurl_dir=../../lib/libcurl-7.56.0
libcurl_include=${libcurl_dir}/include
libcurl_lib=${libcurl_dir}/lib

jsoncpp_dir=../../lib/jsoncpp-1.8.3
jsoncpp_include=${jsoncpp_dir}/include
jsoncpp_src=${jsoncpp_dir}/src


libwebsockets_dir=../../lib/libwebsockets-2.4.0
libwebsockets_include=${libwebsockets_dir}/include
libwebsockets_lib=${libwebsockets_dir}/lib

# Binance env
libbinacpp_dir=../../lib/libbinacpp
libbinacpp_include=${libbinacpp_dir}/include
libbinacpp_lib=${libbinacpp_dir}/lib

# Bittrex env
libbitxcpp_dir=../../lib/libbitxcpp
libbitxcpp_include=${libbitxcpp_dir}/include
libbitxcpp_lib=${libbitxcpp_dir}/lib

# Bitstamp env
libbitscpp_dir=../../lib/libbitscpp
libbitscpp_include=${libbitscpp_dir}/include
libbitscpp_lib=${libbitscpp_dir}/lib

# Kraken env
libkrakcpp_dir=../../lib/libkrakcpp
libkrakcpp_include=${libkrakcpp_dir}/include
libkrakcpp_lib=${libkrakcpp_dir}/lib

# Coinbase env
libcoincpp_dir=../../lib/libcoincpp
libcoincpp_include=${libcoincpp_dir}/include
libcoincpp_lib=${libcoincpp_dir}/lib

librapidjson_include=../../include/

libcrypto++_lib=/usr/lib/x86_64-linux-gnu/

INCLUDES=-I$(SYSINCLUDE) \
		 -I$(SRC) \
 		 -I$(UTIL) \
		 -I$(SRC)/server \
		 -I$(SRC)/raycore \
		 -I$(SRC)/binance \
		 -I$(SRC)/bitfinex \
		 -I$(SRC)/bittrex \
		 -I$(SRC)/bitstamp \
		 -I$(SRC)/kraken \
		 -I$(SRC)/coinbase \
		 -I$(libcurl_include) \
		 -I$(jsoncpp_include) \
		 -I$(libwebsockets_include) \
		 -I$(libbinacpp_include) \
		 -I$(libbitxcpp_include) \
		 -I$(libbitscpp_include) \
		 -I$(libkrakcpp_include) \
		 -I$(libcoincpp_include) \
		 -I$(librapidjson_include) \
		 -I$(libdbconnector_include) \
		 -I$(libspdlog_include) 

LD_PATH=-L$(libcurl_lib) \
		   -L$(libwebsockets_lib) \
		   -L$(libbinacpp_lib) \
		   -L$(libbitxcpp_lib) \
		   -L$(libbitscpp_lib) \
		   -L$(libkrakcpp_lib) \
		   -L$(libcoincpp_lib) \
		   -L$(libcrypto++_lib)

trading: main.cpp
	$(CC) -g -o trading \
	main.cpp \
	$(UTIL)/signalHandler.cpp \
	$(UTIL)/mySqlStore.cpp \
	$(SRC)/server/exchangeRequest.cpp \
	$(SRC)/server/tradingFactory.cpp \
	$(SRC)/server/exchange.cpp \
	$(SRC)/raycore/raycore.cpp \
	$(SRC)/binance/binance.cpp \
	$(SRC)/bittrex/bittrex.cpp \
	$(SRC)/bitstamp/bitstamp.cpp \
	$(SRC)/kraken/kraken.cpp \
	$(SRC)/coinbase/coinbase.cpp \
	$(SRC)/bitfinex/bitfinex.cpp \
	$(LD_PATH) $(INCLUDES) $(LDFLAGS) $(CPPFLAGS) 

	cat run.template.sh |  sed s/%executable%/$@/ > $@.run.sh
	chmod 755 $@.run.sh

clean:
	rm -f trading
