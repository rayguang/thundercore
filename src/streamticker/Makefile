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
CPPFLAGS=-Wno-multichar -Wall -lPocoUtil -lPocoNet -lPocoData -lPocoFoundation -lPocoNetSSL -lPocoData -lPocoDataMySQL -lPocoJSON -lcurl  -lcrypto -lwebsockets -lbinacpp -lcrypto++ 
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


librapidjson_include=../../include/

libcrypto++_lib=/usr/lib/x86_64-linux-gnu/

INCLUDES=-I$(SYSINCLUDE) \
 		 -I$(UTIL) \
		 -I$(libcurl_include) \
		 -I$(jsoncpp_include) \
		 -I$(libwebsockets_include) \
		 -I$(libbinacpp_include) \
		 -I$(librapidjson_include) \
		 -I$(libdbconnector_include) \
		 -I$(libspdlog_include) 

LD_PATH=-L$(libcurl_lib) \
		   -L$(libwebsockets_lib) \
		   -L$(libbinacpp_lib) \
		   -L$(libcrypto++_lib)

orderbook: main.cpp
	$(CC) -g -o orderbook \
	main.cpp \
	$(LD_PATH) $(INCLUDES) $(LDFLAGS) $(CPPFLAGS) 

	cat run.template.sh |  sed s/%executable%/$@/ > $@.run.sh
	chmod 755 $@.run.sh

clean:
	rm -f trading
