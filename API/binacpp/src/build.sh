#!/bin/bash
EXCHANGE=binance
API_DIR=libbinacpp

clear

make clean

echo ""
echo "******** Compiling exchange API ${EXCHANGE} ********"
make

echo "Exchange API lib/include: ${API_DIR}"
echo "Copying headers to /include"
cp *.h ../../../include/${EXCHANGE}/

echo "Copying libs to /lib"
cp ../lib/${API_DIR}/lib/* ../../../lib/${API_DIR}/lib/

echo "Copying headers to /lib"
cp *.h ../../../lib/${API_DIR}/include/
