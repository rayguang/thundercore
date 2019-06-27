#
# See howto.txt
#
CC=g++

DEV_LIB=$(DEV)/lib
DEV_SDK=$(DEV)/sdk

CPPFLAGS=-Wno-multichar -Wall -lPocoUtil -lPocoFoundation -lPocoNetSSL -lPocoData -lPocoDataMySQL -lPocoDataSQLite
LDFLAGS=-lm -ldl -lpthread -DNDEBUG

dbconnector: dbconnector.cpp
	$(CC) -g -o dbconnector dbconnector.cpp $(LDFLAGS) $(CPPFLAGS)

clean:
	rm -f dbconnector
