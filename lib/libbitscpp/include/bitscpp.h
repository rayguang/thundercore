/*
	Author: Ray Guang
	Date  : 2019/04/24
	
	C++ library for Bitstamp API.
*/


#ifndef BITSCPP_H
#define BITSCPP_H


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



#define BITSTAMP_HOST_v2 "https://www.bitstamp.net/api/v2"
#define BITSTAMP_HOST_v1 "https://www.bitstamp.net/api"

using namespace std;

class BitsCPP {

	public:	
		BitsCPP();
		virtual ~BitsCPP();

	private:
		string api_key;
		string secret_key;
		string user_id;
		CURL* curl;	

	public:
		void curl_api( string &url, string &result_json );
		void curl_api_loc( string &url, string &result_json );
		void curl_api_with_header( string &url, string &result_json , vector <string> &extra_http_header, string &post_data, string &action );
		void curl_api_with_header_loc( string &url, string &result_json , vector <string> &extra_http_header, string &post_data, string &action );

	private:
		static size_t curl_cb( void *content, size_t size, size_t nmemb, string *buffer ) ;
		
	public:
		bool init( string &api_key, string &secret_key);
		bool init( string &api_key, string &secret_key, string &user_id);

	private:
		void cleanup();

	public:
		// Public API
		void get_ticker( const char *symbol, Json::Value &json_result ); 	
		void get_orderbook ( const char * symbol, Json::Value &json_result );
		void get_hourlyticker( const char *symbol, Json::Value &json_result );
		void get_transactions( const char *symbol, Json::Value &json_result ); 
		void get_tradingpairs( Json::Value &json_result ); 
		void get_eurusd( Json::Value &json_result ); 

		// API + Secret keys required
		// Authenticated endpoints: Account
		void get_balance( const char *symbol, Json::Value &json_result );
		void get_userTransactions( const char *offset, int limit, const char *sort, const char *symbol, Json::Value &json_result );

		// Authenticated endpoints: Orders
		void get_openOrders(const char *symbol, Json::Value &json_result);
		void get_orderStatus(const char *orderId, Json::Value &json_result);
		void cancel_order(const char *orderId, Json::Value &json_result);
		void cancel_allOrders( Json::Value &json_result );
		void buy_limitOrder(const char *symbol, double amount, double price, double limit_price, const char *timeInForce, Json::Value &json_result);
		void buy_marketOrder(const char *symbol, double amount, Json::Value &json_result);
		void buy_instantOrder(const char *symbol, double amount, Json::Value &json_result); // a limit order with limit set to market price
		void sell_limitOrder(const char *symbol, double amount, double price, double limit_price, const char *timeInForce, Json::Value &json_result);
		void sell_marketOrder(const char *symbol, double amount, Json::Value &json_result);
		void sell_instantOrder(const char *symbol, double amount, Json::Value &json_result);

		// Authenticated endpoints: Deposit and Withdrawl
		void get_ethDepositAddress();
		void get_btcDepositAddress();

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
