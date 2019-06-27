/*
	Author: Ray Guang
	Date  : 2019/04/24
	
	C++ library for Coinbase Pro API.
*/


#ifndef COINCPP_H
#define COINCPP_H


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



#define COINBASE_HOST					"https://api.pro.coinbase.com"
//#define COINBASE_HOST					"https://api.coinbase.com/v2/"

using namespace std;

class CoinCPP {

	public:	
		CoinCPP();
		virtual ~CoinCPP();

	private:
		string api_key;
		string secret_key;
		string passphrase; 
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
		bool init( string &api_key, string &secret_key, string &passphrase);
		string signature(const string& time_stamp, const string& method, const string& path, const string& body) ;
		const vector<string> build_header();

	private:
		void cleanup();

	public:
		// Public API
		void get_serverTime( Json::Value &json_result ); 
		void get_products( Json::Value &json_result );
		void get_productOrderBook ( const char* symbol, Json::Value &json_result, string level = "1" );
		void get_productTicker( const char* symbol, Json::Value &json_resul );
		void get_productTrades( const char* symbol, Json::Value &json_resul );
		void get_historicRates ( const char* symbol, Json::Value &json_result );
		void get_currencies( Json::Value &json_result );

		// API + Secret keys required
		// Authenticated endpoints: Account
		void get_listAccounts( Json::Value &json_result );
		void get_account ( const char* account_id, Json::Value &json_result );
		void get_accountHistory ( const char* account_id, Json::Value &json_result );
		void get_holds ( const char* account_id, Json::Value &json_result );

		// Authenticated endpoints: Trade
		void send_order_market( const char* symbol, const char* side, double quantity, const char* client_oid, Json::Value &json_result );
		void send_order_limit( const char* symbol, const char* side, double quantity, double price, const char* timeInForce, const char* client_oid, Json::Value &json_result );
		void send_order_stop();
		void cancel_oneOrder( const char* orderId, Json::Value &json_result );
		void cancel_allOrders( const char* symbol, Json::Value &json_result );
		void get_allOrders( const char* symbol, const char* status, Json::Value &json_result ); 
		void get_oneOrder( const char* orderId, Json::Value &json_result );
		void get_fills( const char* symbol, Json::Value &json_result );


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
