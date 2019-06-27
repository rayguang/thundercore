# Makefile for trade execution application
PROG = trading
CC = g++

libcurl_dir=../../lib/libcurl-7.56.0
libcurl_include=${libcurl_dir}/include
libcurl_lib=${libcurl_dir}/lib

jsoncpp_dir=../../lib/jsoncpp-1.8.3
jsoncpp_include=${jsoncpp_dir}/include
jsoncpp_src=${jsoncpp_dir}/src


libwebsockets_dir=../../lib/libwebsockets-2.4.0
libwebsockets_include=${libwebsockets_dir}/include
libwebsockets_lib=${libwebsockets_dir}/lib

libbinacpp_dir=../../lib/libbinacpp
libbinacpp_include=${libbinacpp_dir}/include
libbinacpp_lib=${libbinacpp_dir}/lib

libopenssl_dir=/usr/local/Cellar/openssl/1.0.2q
libopenssl_include=/usr/local/Cellar/openssl/1.0.2q/include
libopenssl_lib=/usr/local/Cellar/openssl/1.0.2q/lib


%: %.cpp
	@echo -n "\nMaking $@\n\n\n"
	$(CC) -I$(libcurl_include) -I$(jsoncpp_include) -I$(libwebsockets_include) -I$(libbinacpp_include) -I$(libopenssl_dir) \
	$@.cpp \
	-L$(libcurl_lib) \
	-L$(libwebsockets_lib) \
	-L$(libbinacpp_lib) \
	-L$(libopenssl_lib) \
	-lcurl -lcrypto -lwebsockets -lbinacpp  -o $@

	cat run.template.sh  | sed s/%executable%/$@/ > $@.run.sh
	chmod 755 $@.run.sh



clean:
	rm $(build_dir)/*.o
	rm $(build_dir)/*.so
