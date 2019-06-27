#
# See howto.txt
#
CC=g++

DEV_LIB=$(DEV)/lib
DEV_SDK=$(DEV)/sdk

#CPPFLAGS=-Wno-multichar -Wall -lPocoUtil -lPocoNet -lPocoData -lPocoFoundation
CPPFLAGS=-Wno-multichar -Wall -lPocoUtil -lPocoNet -lPocoFoundation -lPocoNetSSL
LDFLAGS=-lm -ldl -lpthread -DNDEBUG

trading: main.cpp
	$(CC) -o trading main.cpp exchangeRequest.cpp $(LDFLAGS) $(CPPFLAGS)

clean:
	rm -f trading
