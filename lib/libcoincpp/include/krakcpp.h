/*
	Author: Ray Guang
	Date  : 2019/04/24
	
	C++ library for Kraken API.
*/


#ifndef KRAKCPP_H
#define KRAKCPP_H


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



#define KRAKEN_HOST_PUBLIC		"https://api.kraken.com/0/public"
#define KRAKEN_HOST_PRIVATE			"https://api.kraken.com/0/private"
#define KRAKEN_HOST					"https://api.kraken.com"

using namespace std;

class KrakCPP {

	public:	
		KrakCPP();
		virtual ~KrakCPP();

	private:
		string api_key;
		string secret_key;
		string two_factor; 
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
		bool init( string &api_key, string &secret_key, string &two_factor);
		string signature(const string& path, const string& nonce, const string& postdata) ;

	private:
		void cleanup();

	public:
		// Public API
		void get_serverTime( Json::Value &json_result ); 
		void get_assetInfo ( const char* symbol, Json::Value &json_result );
		void get_assetPairs ( const char* symbol, Json::Value &json_result );
		void get_ticker ( const char* symbol, Json::Value &json_result );
		void get_OHLC();
		void get_orderBook ( const char* symbol, Json::Value &json_result, int count = 0 );
		void get_recentTrades( const char* symbol, Json::Value &json_result, const char* id );
		void get_spread(const char* symbol, Json::Value &json_result, const char* id);

		// API + Secret keys required
		// Authenticated endpoints: Account
		void get_balance( Json::Value &json_result );
		void get_tradeBalance(const char* symbol, Json::Value &json_result);
		void get_openOrders();
		void get_closedOrders();
		void query_orders();
		void get_tradeHistory();
		void query_trades();
		void get_ledgerInfo();
		void query_ledgers();
		void get_tradeVolume();
		void send_order();
		void cancel_order();

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
