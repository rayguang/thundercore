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

build_dir=../lib/libkrakcpp/lib
objects=$(build_dir)/jsoncpp.o $(build_dir)/krakcpp_utils.o $(build_dir)/krakcpp_logger.o $(build_dir)/krakcpp.o  $(build_dir)/krakcpp_websocket.o

build_include=../lib/libkrakcpp/include


$(build_dir)/libkrakcpp.so: $(objects)
	g++ -I$(libcurl_include) -I$(jsoncpp_include) -I$(libwebsockets_include)  \
	-L$(libcurl_lib) \
	-L$(libwebsockets_lib) \
	$(objects) \
	-shared \
	-lcurl -lcrypto -lwebsockets -fPIC -o $@

	# Make a new copy of the header too
	cp *.h  $(build_include)


$(build_dir)/krakcpp.o: krakcpp.cpp krakcpp.h 
	g++ -I$(libcurl_include) -I$(jsoncpp_include) -c krakcpp.cpp  -fPIC   -o $(build_dir)/krakcpp.o 


$(build_dir)/krakcpp_websocket.o: krakcpp_websocket.cpp krakcpp_websocket.h 
	g++ -I$(libwebsockets_include) -I$(jsoncpp_include) -c krakcpp_websocket.cpp  -fPIC   -o $(build_dir)/krakcpp_websocket.o


$(build_dir)/krakcpp_logger.o: krakcpp_logger.cpp krakcpp_logger.h 
	g++ -c krakcpp_logger.cpp  -fPIC   -o $(build_dir)/krakcpp_logger.o 


$(build_dir)/krakcpp_utils.o: krakcpp_utils.cpp krakcpp_utils.h 
	g++ -c krakcpp_utils.cpp -fPIC   -o $(build_dir)/krakcpp_utils.o 


$(build_dir)/jsoncpp.o: $(jsoncpp_src)/jsoncpp.cpp
	g++ -I$(jsoncpp_include) -c $(jsoncpp_src)/jsoncpp.cpp -fPIC  -o  $(build_dir)/jsoncpp.o

clean:
	rm $(build_dir)/*.o
	rm $(build_dir)/*.so






