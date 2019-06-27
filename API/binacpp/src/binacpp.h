
/*
	Author: tensaix2j
	Date  : 2017/10/15
	
	C++ library for Binance API.
*/


#ifndef BINACPP_H
#define BINACPP_H


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <exception>

#include <curl/curl.h>
#include <json/json.h>



#define BINANCE_HOST "https://api.binance.com"


using namespace std;

class BinaCPP {

	public:	
		BinaCPP();
		virtual ~BinaCPP();

	private:
		string api_key;
		string secret_key;
		CURL* curl;	

	public:
		void curl_api( string &url, string &result_json );
		void curl_api_with_header( string &url, string &result_json , vector <string> &extra_http_header, string &post_data, string &action );

	private:
		static size_t curl_cb( void *content, size_t size, size_t nmemb, string *buffer ) ;
		
	public:
		bool init( string &api_key, string &secret_key);

	private:
		void cleanup();

	public:
		// Public API
		void get_serverTime( Json::Value &json_result); 	
		void get_tradingPairs ( Json::Value &json_result );

		void get_allPrices( Json::Value &json_result );
		void get_price( const char *symbol, Json::Value &json_result );

		void get_allBookTickers( Json::Value &json_result );
		void get_bookTicker( const char *symbol, Json::Value &json_result ) ;

		void get_depth( const char *symbol, int limit, Json::Value &json_result );
		void get_recentTrades( const char *symbol, int limit, Json::Value &json_result );
		void get_aggTrades( const char *symbol, const char *fromId, time_t startTime, time_t endTime, int limit, Json::Value &json_result ); 
		void get_24hr( const char *symbol, Json::Value &json_result ); 
		void get_klines( const char *symbol, const char *interval, int limit, time_t startTime, time_t endTime,  Json::Value &json_result );


		// API + Secret keys required
		void get_account( long recvWindow , Json::Value &json_result );
		
		void get_myTrades( 
			const char *symbol, 
			int limit,
			const char *fromId,
			long recvWindow, 
			Json::Value &json_result 
		);
		
		void get_openOrders(  
			const char *symbol, 
			long recvWindow,   
			Json::Value &json_result 
		) ;
		

		void get_allOrders(   
			const char *symbol, 
			const char *orderId,
			int limit,
			long recvWindow,
			Json::Value &json_result 
		);


		void send_order( 
			const char *symbol, 
			const char *side,
			const char *type,
			const char *timeInForce,
			double quantity,
			double price,
			const char *newClientOrderId,
			double stopPrice,
			double icebergQty,
			long recvWindow,
			Json::Value &json_result ) ;
		
		void send_order_market(
            const char *symbol,
            const char *side,
            const char *type,
            double quantity,
            const char *newClientOrderId,
            double stopPrice,
            double icebergQty,
            long recvWindow,
            Json::Value &json_result ) ;

		void get_order( 
			const char *symbol, 
			const char *orderId,
			const char *origClientOrderId,
			long recvWindow,
			Json::Value &json_result ); 


		void cancel_order( 
			const char *symbol, 
			const char *orderId,
			const char *origClientOrderId,
			const char *newClientOrderId,
			long recvWindow,
			Json::Value &json_result 
		);

		// API key required
		void start_userDataStream( Json::Value &json_result );
		void keep_userDataStream( const char *listenKey  );
		void close_userDataStream( const char *listenKey );


		// WAPI
		void withdraw( 
			const char *asset,
			const char *address,
			const char *addressTag,
			double amount, 
			const char *name,
			long recvWindow,
			Json::Value &json_result );

		void get_depositHistory( 
			const char *asset,
			int  status,
			long startTime,
			long endTime, 
			long recvWindow,
			Json::Value &json_result );

		void get_withdrawHistory( 
			const char *asset,
			int  status,
			long startTime,
			long endTime, 
			long recvWindow,
			Json::Value &json_result ); 

		void get_depositAddress( 
			const char *asset,
			long recvWindow,
			Json::Value &json_result );


};


#endif
