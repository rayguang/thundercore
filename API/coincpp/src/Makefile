# Ray trading API make file updated for CT trading enging

root_dir=../../..
root_include=/include

libcurl_dir=../lib/libcurl-7.56.0
libcurl_include=${libcurl_dir}/include
libcurl_lib=${libcurl_dir}/lib

jsoncpp_dir=../lib/jsoncpp-1.8.3
jsoncpp_include=${jsoncpp_dir}/include
jsoncpp_src=${jsoncpp_dir}/src

libwebsockets_dir=../lib/libwebsockets-2.4.0
libwebsockets_include=${libwebsockets_dir}/include
libwebsockets_lib=${libwebsockets_dir}/lib

build_dir=../lib/libcoincpp/lib
objects=$(build_dir)/jsoncpp.o $(build_dir)/coincpp_utils.o $(build_dir)/coincpp_logger.o $(build_dir)/coincpp.o  $(build_dir)/coincpp_websocket.o

build_include=../lib/libcoincpp/include


$(build_dir)/libcoincpp.so: $(objects)
	g++ -I$(libcurl_include) -I$(jsoncpp_include) -I$(libwebsockets_include)  \
	-L$(libcurl_lib) \
	-L$(libwebsockets_lib) \
	$(objects) \
	-shared \
	-lcurl -lcrypto -lwebsockets -fPIC -o $@

	# Make a new copy of the header too
	cp *.h  $(build_include)


$(build_dir)/coincpp.o: coincpp.cpp coincpp.h 
	g++ -I$(libcurl_include) -I$(jsoncpp_include) -c coincpp.cpp  -fPIC   -o $(build_dir)/coincpp.o 


$(build_dir)/coincpp_websocket.o: coincpp_websocket.cpp coincpp_websocket.h 
	g++ -I$(libwebsockets_include) -I$(jsoncpp_include) -c coincpp_websocket.cpp  -fPIC   -o $(build_dir)/coincpp_websocket.o


$(build_dir)/coincpp_logger.o: coincpp_logger.cpp coincpp_logger.h 
	g++ -c coincpp_logger.cpp  -fPIC   -o $(build_dir)/coincpp_logger.o 


$(build_dir)/coincpp_utils.o: coincpp_utils.cpp coincpp_utils.h 
	g++ -c coincpp_utils.cpp -fPIC   -o $(build_dir)/coincpp_utils.o 


$(build_dir)/jsoncpp.o: $(jsoncpp_src)/jsoncpp.cpp
	g++ -I$(jsoncpp_include) -c $(jsoncpp_src)/jsoncpp.cpp -fPIC  -o  $(build_dir)/jsoncpp.o

clean:
	rm $(build_dir)/*.o
	rm $(build_dir)/*.so






