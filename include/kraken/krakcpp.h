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

		// Auenticated endpoints: trades
		void get_openOrders( Json::Value &json_result );
		void get_closedOrders( Json::Value &json_result );
		void query_orders( const char* orderId,  Json::Value &json_result );
		void get_tradeHistory( Json::Value &json_result );
		void query_trades( Json::Value &json_result );
		void get_openPositions( Json::Value &json_result );
		void get_ledgerInfo( Json::Value &json_result );
		void query_ledgers( Json::Value &json_result );
		void get_tradeVolume( Json::Value &json_result );
		
		void send_order_market( const char* symbol, const char* side, double quantity, const char* userref, Json::Value &json_result, const char* leverage );
		void send_order_limit ( const char* symbol, const char* side, double quantity, double price, const char* userref, Json::Value &json_result , const char* starttm, const char* expiretm, const char* leverage );
		void cancel_order( const char* orderId, Json::Value &json_result );

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
