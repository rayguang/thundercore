
/*
	Author: Ray Guang
	Date  : 2019/04/22
	
	C++ library for Bittrex API.
*/


#ifndef BITXCPP_H
#define BITXCPP_H


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



#define BITTREX_HOST "https://api.bittrex.com/api/v1.1"


using namespace std;

class BitxCPP {

	public:	
		BitxCPP();
		virtual ~BitxCPP();

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
		void get_markets( Json::Value &json_result); 	
		void get_currencies( Json::Value &json_result );
		void get_ticker( const char *symbol, Json::Value &json_result );

		void get_marketsummaries( Json::Value &json_result );
		void get_marketsummary( const char *symbol, Json::Value &json_result ) ;

		void get_orderBook( const char *symbol, const char *type, Json::Value &json_result );
		void get_marketHistory( const char *symbol, Json::Value &json_result  ); 
		

		// API + Secret keys required
		// Authenticated endpoints: Account
		void get_balances( 
			Json::Value &json_result
		   	);

		void get_balance(
			const char *symbol,
			Json::Value &json_result
			);
		
		void get_depositAddress( 
			const char *symbol, 
			Json::Value &json_result 
		);
		
		void withdraw(  
			const char *symbol, 
			double quantity,
			const char *address,   
			Json::Value &json_result 
		) ;
		
		void get_order(   
			const char *orderId,
			Json::Value &json_result 
		);

		void get_orderHistory(
			Json::Value &json_result
		);

		void get_withdrawalHistory(
			const char *symbol,
			Json::Value &json_result
		);

		void get_depositHistory(
			const char *symbol,
			Json::Value &json_result
		);

		// Authenticated endpoints: Market
		void buy_limit(
			const char *symbol,
			double quantity,
			double rate,
			Json::Value &json_result
		);
		
		void sell_limit(
			const char *symbol,
			double quantity,
			double rate,
			Json::Value &json_result
		);

		void cancel_oneOrder(
			const char *orderId,
			Json::Value &json_result
		);

		void get_openOrders(
			const char *market,
			Json::Value &json_result
		);

		// API key required
		void start_userDataStream( Json::Value &json_result );
		void keep_userDataStream( const char *listenKey  );
		void close_userDataStream( const char *listenKey );


		// WAPI
		// void withdraw( 
		// 	const char *asset,
		// 	const char *address,
		// 	const char *addressTag,
		// 	double amount, 
		// 	const char *name,
		// 	long recvWindow,
		// 	Json::Value &json_result );

		// void get_depositHistory( 
		// 	const char *asset,
		// 	int  status,
		// 	long startTime,
		// 	long endTime, 
		// 	long recvWindow,
		// 	Json::Value &json_result );

		// void get_withdrawHistory( 
		// 	const char *asset,
		// 	int  status,
		// 	long startTime,
		// 	long endTime, 
		// 	long recvWindow,
		// 	Json::Value &json_result ); 

		// void get_depositAddress( 
		// 	const char *asset,
		// 	long recvWindow,
		// 	Json::Value &json_result );


};


#endif
