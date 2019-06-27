#
# See howto.txt
#
CC=g++

DEV=/usr/include/
DEV_LIB=$(DEV)/lib
DEV_SDK=$(DEV)/sdk

CPPFLAGS=-Wno-multichar -Wall -lPocoUtil -lPocoNet -lPocoFoundation -lPocoNetSSL -lPocoJSON -luuid
LDFLAGS=-lm -ldl -lpthread -DNDEBUG

askTrading: main.cpp
	$(CC) -o askTrading main.cpp $(LDFLAGS) $(CPPFLAGS)

clean:
	rm -f askTrading
