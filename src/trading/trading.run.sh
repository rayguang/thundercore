#!/bin/bash

libcurl_dir=../../lib/libcurl-7.56.0
libcurl_lib=${libcurl_dir}/lib

libwebsockets_dir=../../lib/libwebsockets-2.4.0
libwebsockets_lib=${libwebsockets_dir}/lib

libbinacpp_dir=../../lib/libbinacpp
libbinacpp_lib=${libbinacpp_dir}/lib

libbitxcpp_dir=../../lib/libbitxcpp
libbitxcpp_lib=${libbitxcpp_dir}/lib

libbitscpp_dir=../../lib/libbitscpp
libbitscpp_lib=${libbitscpp_dir}/lib

libkrakcpp_dir=../../lib/libkrakcpp
libkrakcpp_lib=${libkrakcpp_dir}/lib

libcoincpp_dir=../../lib/libcoincpp
libcoincpp_lib=${libcoincpp_dir}/lib

export SSL_CERT_FILE=`pwd`/cacert.pem
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$libcurl_lib:$libwebsockets_lib:$libbinacpp_lib:$libbitxcpp_lib:$libbitscpp_lib:$libkrakcpp_lib:$libcoincpp_lib


terminal_trading="trading"

clear

echo "Stopping all running process if any ..."
killall trading

echo "Checking duplicated running process ..."
ps -ef | grep $terminal_trading | grep -v grep | awk '{print $1 "\t" $2 "\t" $NF}'

echo ""
./trading 1234
