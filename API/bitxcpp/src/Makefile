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

build_dir=../lib/libbitxcpp/lib
objects=$(build_dir)/jsoncpp.o $(build_dir)/bitxcpp_utils.o $(build_dir)/bitxcpp_logger.o $(build_dir)/bitxcpp.o  $(build_dir)/bitxcpp_websocket.o

build_include=../lib/libbitxcpp/include


$(build_dir)/libbitxcpp.so: $(objects)
	g++ -I$(libcurl_include) -I$(jsoncpp_include) -I$(libwebsockets_include)  \
	-L$(libcurl_lib) \
	-L$(libwebsockets_lib) \
	$(objects) \
	-shared \
	-lcurl -lcrypto -lwebsockets -fPIC -o $@

	# Make a new copy of the header too
	cp *.h  $(build_include)

	# Ray changes
	#cp bitxcpp.h /home/ray/trading_execution/lib/libbinacpp/include
	#cp binacpp.cpp /home/ray/trading_execution/src/ray_binacpp_change
	#cp binacpp.h /home/ray/trading_execution/src/ray_binacpp_change 
	#cp /home/ray/trading_execution/API/binacpp/lib/libbinacpp/lib/libbinacpp.so /home/ray/trading_execution/lib/libbinacpp/lib


$(build_dir)/bitxcpp.o: bitxcpp.cpp bitxcpp.h 
	g++ -I$(libcurl_include) -I$(jsoncpp_include) -c bitxcpp.cpp  -fPIC   -o $(build_dir)/bitxcpp.o 


$(build_dir)/bitxcpp_websocket.o: bitxcpp_websocket.cpp bitxcpp_websocket.h 
	g++ -I$(libwebsockets_include) -I$(jsoncpp_include) -c bitxcpp_websocket.cpp  -fPIC   -o $(build_dir)/bitxcpp_websocket.o


$(build_dir)/bitxcpp_logger.o: bitxcpp_logger.cpp bitxcpp_logger.h 
	g++ -c bitxcpp_logger.cpp  -fPIC   -o $(build_dir)/bitxcpp_logger.o 



$(build_dir)/bitxcpp_utils.o: bitxcpp_utils.cpp bitxcpp_utils.h 
	g++ -c bitxcpp_utils.cpp -fPIC   -o $(build_dir)/bitxcpp_utils.o 


$(build_dir)/jsoncpp.o: $(jsoncpp_src)/jsoncpp.cpp
	g++ -I$(jsoncpp_include) -c $(jsoncpp_src)/jsoncpp.cpp -fPIC  -o  $(build_dir)/jsoncpp.o

clean:
	rm $(build_dir)/*.o
	rm $(build_dir)/*.so






