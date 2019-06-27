/*
	Author: Ray Guang
	Date  : 2019/04/24
	
	C++ library for Bitstamp API.
*/

#include "bitscpp.h"
#include "bitscpp_logger.h"
#include "bitscpp_utils.h"
#include "bitscpp_hmac.h"

BitsCPP::BitsCPP()
	: curl(NULL)
{
}

BitsCPP::~BitsCPP()
{
	if (NULL != BitsCPP::curl)
		cleanup();
}

//---------------------------------
bool
BitsCPP::init( string &api_key, string &secret_key ) 
{	
	BitsCPP::api_key = api_key;
	BitsCPP::secret_key = secret_key;

	BitsCPP::curl = curl_easy_init();

	return NULL != BitsCPP::curl;
}

// Signature required: key, nonce, and user_id
// Constructor to include user_id
bool
BitsCPP::init( string &api_key, string &secret_key, string &user_id ) 
{	
	BitsCPP::api_key = api_key;
	BitsCPP::secret_key = secret_key;
	BitsCPP::user_id = user_id;

	BitsCPP::curl = curl_easy_init();

	return NULL != BitsCPP::curl;
}

void
BitsCPP::cleanup()
{
	curl_easy_cleanup(BitsCPP::curl);	
}


//++++++++++++++++++++++++++++++++++++++++++++++++++
// Public API endpoints begins here
//++++++++++++++++++++++++++++++++++++++++++++++++++
// https://www.bitstamp.net/api/v2/ticker/{currency_pair}/
// Supported values for currency_pair: btcusd, btceur, eurusd, 
// xrpusd, xrpeur, xrpbtc, ltcusd, ltceur, ltcbtc, ethusd, etheur, ethbtc, bchusd, bcheur, bchbtc
void 
BitsCPP::get_ticker( const char *symbol, Json::Value &json_result ) 
{
	BitsCPP_logger::write_log( "<BitsCPP::get_ticker>" ) ;

	string url(BITSTAMP_HOST_v2);  
	url += "/ticker/";
	url += string_tolower(symbol);
	
	std::cout << "URL request: " << url << std::endl;

	string str_result;
	curl_api( url, str_result ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
		} catch ( exception &e ) {
		 	BitsCPP_logger::write_log( "<BitsCPP::get_ticker> Error ! %s", e.what() ); 
		}   
		BitsCPP_logger::write_log( "<BitsCPP::get_ticker> Done." ) ;
	
	} else {
		BitsCPP_logger::write_log( "<BitsCPP::get_ticker> Failed to get anything." ) ;
	}
}

// https://www.bitstamp.net/api/v2/order_book/{currency_pair}/API
void
BitsCPP::get_orderbook( const char *symbol, Json::Value &json_result )
{
    BitsCPP_logger::write_log( "<BitsCPP::get_orderbook>" ) ;

    string url(BITSTAMP_HOST_v2);
    url += "/order_book/";
    url += string_tolower(symbol);

    std::cout << "URL request: " << url << std::endl;

    string str_result;
    curl_api( url, str_result ) ;

    if ( str_result.size() > 0 ) {

        try {
            Json::Reader reader;
            json_result.clear();
            reader.parse( str_result , json_result );

        } catch ( exception &e ) {
            BitsCPP_logger::write_log( "<BitsCPP::get_orderbook> Error ! %s", e.what() );
        }
        BitsCPP_logger::write_log( "<BitsCPP::get_orderbook> Done." ) ;

    } else {
        BitsCPP_logger::write_log( "<BitsCPP::get_orderbook> Failed to get anything." ) ;
    }
}

// https://www.bitstamp.net/api/v2/ticker_hour/{currency_pair}/
// Supported values for currency_pair: btcusd, btceur, eurusd, xrpusd, 
// xrpeur, xrpbtc, ltcusd, ltceur, ltcbtc, ethusd, etheur, ethbtc, bchusd, bcheur, bchbtc
void
BitsCPP::get_hourlyticker( const char *symbol, Json::Value &json_result )
{
    BitsCPP_logger::write_log( "<BitsCPP::get_hourlyticker>" ) ;

    string url(BITSTAMP_HOST_v2);
    url += "/ticker_hour/";
    url += string_tolower(symbol);

    std::cout << "URL request: " << url << std::endl;

    string str_result;
    curl_api_loc( url, str_result ) ;
	
	std::cout << "curl api result: " << str_result << std::endl;

    if ( str_result.size() > 0 ) {

        try {
            Json::Reader reader;
            json_result.clear();
            reader.parse( str_result , json_result );

        } catch ( exception &e ) {
            BitsCPP_logger::write_log( "<BitsCPP::get_hourlyticker> Error ! %s", e.what() );
        }
        BitsCPP_logger::write_log( "<BitsCPP::get_hourlyticker> Done." ) ;

    } else {
        BitsCPP_logger::write_log( "<BitsCPP::get_hourlyticker> Failed to get anything." ) ;
    }
}


// https://www.bitstamp.net/api/v2/transactions/{currency_pair}/
// Supported values for currency_pair: btcusd, btceur, eurusd, xrpusd, 
// xrpeur, xrpbtc, ltcusd, ltceur, ltcbtc, ethusd, etheur, ethbtc, bchusd, bcheur, bchbtc
void 
BitsCPP::get_transactions( const char *symbol, Json::Value &json_result ) 
{
	BitsCPP_logger::write_log( "<BitsCPP::get_transactions>" ) ;

	string url(BITSTAMP_HOST_v2);  
	url += "/transactions/";
	url += string_tolower(symbol);
	
	std::cout << "URL request: " << url << std::endl;

	//string querystring;
	//querystring.append( symbol );

	string str_result;
	curl_api_loc( url, str_result ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
		} catch ( exception &e ) {
		 	BitsCPP_logger::write_log( "<BitsCPP::get_transactions> Error ! %s", e.what() ); 
		}   
		BitsCPP_logger::write_log( "<BitsCPP::get_transactions> Done." ) ;
	
	} else {
		BitsCPP_logger::write_log( "<BitsCPP::get_transactions> Failed to get anything." ) ;
	}
}


// https://www.bitstamp.net/api/v2/trading-pairs-info/
void 
BitsCPP::get_tradingpairs( Json::Value &json_result ) 
{
	BitsCPP_logger::write_log( "<BitsCPP::get_tradingpairs>" ) ;

	string url(BITSTAMP_HOST_v2);  
	url += "/trading-pairs-info//";
	
	std::cout << "URL request: " << url << std::endl;

	string str_result;
	curl_api( url, str_result ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
		} catch ( exception &e ) {
		 	BitsCPP_logger::write_log( "<BitsCPP::get_tradingpairs> Error ! %s", e.what() ); 
		}   
		BitsCPP_logger::write_log( "<BitsCPP::get_tradingpairs> Done." ) ;
	
	} else {
		BitsCPP_logger::write_log( "<BitsCPP::get_tradingpairs> Failed to get anything." ) ;
	}
}

// https://www.bitstamp.net/api/v2/trading-pairs-info/
void 
BitsCPP::get_eurusd( Json::Value &json_result ) 
{
	BitsCPP_logger::write_log( "<BitsCPP::get_eurusd>" ) ;

	string url(BITSTAMP_HOST_v1);  
	url += "/eur_usd/";
	
	std::cout << "URL request: " << url << std::endl;


	string str_result;
	curl_api( url, str_result ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
		} catch ( exception &e ) {
		 	BitsCPP_logger::write_log( "<BitsCPP::get_eurusd> Error ! %s", e.what() ); 
		}   
		BitsCPP_logger::write_log( "<BitsCPP::get_eurusd> Done." ) ;
	
	} else {
		BitsCPP_logger::write_log( "<BitsCPP::get_eurusd> Failed to get anything." ) ;
	}
}

//++++++++++++++++++++++++++++++++++++++++++++++++++
// Private API endpoints begins here
//++++++++++++++++++++++++++++++++++++++++++++++++++

//--------------------
// Get current account information. (SIGNED)
/*
POST https://www.bitstamp.net/api/v2/balance/
Returns all the balances.

POST https://www.bitstamp.net/api/v2/balance/{currency_pair}/
Returns the values relevant to the specified currency_pair parameter. 
Supported values for currency_pair: btcusd, btceur, eurusd, xrpusd, xrpeur, 
xrpbtc, ltcusd, ltceur, ltcbtc, ethusd, etheur, ethbtc, bchusd, bcheur, bchbtc
*/
void 
BitsCPP::get_balance( const char *symbol, Json::Value &json_result ) 
{	

	BitsCPP_logger::write_log( "<BitsCPP::get_balances>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		BitsCPP_logger::write_log( "<BitsCPP::get_balances> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string message;
	string nonce;

	string url(BITSTAMP_HOST_v2);
	url += "/balance/";
	string action = "POST";

	// Append currency only if we provides in the request; if no currency is provided in the request, returns history for all currencies
    if ( symbol && symbol[0] )
    {	
        url.append( string_tolower(symbol) );
		url.append("/");
        // std::cout << "appending symbol" << std::endl;
    }

	nonce = to_string( get_current_ms_epoch() );
	string querystring("nonce=");
	querystring.append( nonce );

	// Signature is a HMAC-SHA256 encoded message containing nonce, customer ID (can be found here) and API key. 
	// The HMAC-SHA256 code must be generated using a secret key that was generated with your API key.
	// This code must be converted to it's hexadecimal representation (64 uppercase characters). 
	message = nonce + user_id + api_key;
	string signature =  hmac_sha256( secret_key.c_str() , message.c_str() );
	
	string_toupper( signature );
	querystring.append( "&signature=");
	querystring.append( signature );

	querystring.append("&key=");
    querystring.append( api_key );

	// std::cout << "nonce: "  << nonce << std::endl;
	// std::cout << "user_id: " << user_id << std::endl;
	// std::cout << "key: " << api_key << std::endl;
	// std::cout << "secret: " << secret_key.c_str() << std::endl;
	// std::cout << "message: " << message.c_str() << std::endl;
	// std::cout << "query: " << querystring.c_str() << std::endl;
	// std::cout << "signature: " << signature << std::endl;

	//url.append( querystring );
	vector <string> extra_http_header;
	//string header_chunk("Accept:");
	//header_chunk.append( "application/json" );
	string header_chunk("Content-Type:");
	header_chunk.append( "application/x-www-form-urlencoded" );
	extra_http_header.push_back(header_chunk);

	BitsCPP_logger::write_log( "<BitsCPP::get_balances> url = |%s|" , url.c_str() ) ;
	
	string post_data = querystring;
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	BitsCPP_logger::write_log( "<BitsCPP::get_balances> Error ! %s", e.what() ); 
		}   
		BitsCPP_logger::write_log( "<BitsCPP::get_balances> Done." ) ;
	
	} else {
		BitsCPP_logger::write_log( "<BitsCPP::get_balances> Failed to get anything." ) ;
	}

	BitsCPP_logger::write_log( "<BitsCPP::get_balances> Done.\n" ) ;

}

// https://www.bitstamp.net/api/v2/user_transactions/{currency_pair}/
// Supported values for currency_pair: btcusd, btceur, eurusd, xrpusd, 
// xrpeur, xrpbtc, ltcusd, ltceur, ltcbtc, ethusd, etheur, ethbtc, bchusd, bcheur, bchbtc
void 
BitsCPP::get_userTransactions( const char *offset, int limit, const char * sort, const char *symbol, Json::Value &json_result ) 
{	

	BitsCPP_logger::write_log( "<BitsCPP::get_userTransactions>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		BitsCPP_logger::write_log( "<BitsCPP::get_userTransactions> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string message;
	string nonce;

	string url(BITSTAMP_HOST_v2);
	url += "/user_transactions/";
	string action = "POST";

	// Append currency only if we provides in the request; if no currency is provided in the request, returns history for all currencies
    if ( symbol && symbol[0] )
    {	
        url.append( string_tolower(symbol) );
		url.append("/");
        // std::cout << "appending symbol" << std::endl;
    }

	nonce = to_string( get_current_ms_epoch() );
	string querystring("nonce=");
	querystring.append( nonce );

	// Signature is a HMAC-SHA256 encoded message containing nonce, customer ID (can be found here) and API key. 
	// The HMAC-SHA256 code must be generated using a secret key that was generated with your API key.
	// This code must be converted to it's hexadecimal representation (64 uppercase characters). 
	message = nonce + user_id + api_key;
	string signature =  hmac_sha256( secret_key.c_str() , message.c_str() );
	
	string_toupper( signature );
	querystring.append( "&signature=");
	querystring.append( signature );

	querystring.append("&key=");
    querystring.append( api_key );

	querystring.append("&offset=");
	querystring.append( offset );

	querystring.append("&limit=");
	querystring.append( to_string(limit) );

	querystring.append("&sort=");
	querystring.append( sort );

	//url.append( querystring );
	vector <string> extra_http_header;
	//string header_chunk("Accept:");
	//header_chunk.append( "application/json" );
	string header_chunk("Content-Type:");
	header_chunk.append( "application/x-www-form-urlencoded" );
	extra_http_header.push_back(header_chunk);

	BitsCPP_logger::write_log( "<BitsCPP::get_userTransactions> url = |%s|" , url.c_str() ) ;
	
	string post_data = querystring;
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	BitsCPP_logger::write_log( "<BitsCPP::get_userTransactions> Error ! %s", e.what() ); 
		}   
		BitsCPP_logger::write_log( "<BitsCPP::get_userTransactions> Done." ) ;
	
	} else {
		BitsCPP_logger::write_log( "<BitsCPP::get_userTransactions> Failed to get anything." ) ;
	}

	BitsCPP_logger::write_log( "<BitsCPP::get_userTransactions> Done.\n" ) ;

}


// Authenticated endpoints: Orders
// Get open orders
// https://www.bitstamp.net/api/v2/open_orders/all/
// https://www.bitstamp.net/api/v2/open_orders/{currency_pair}
// Supported values for currency_pair: btcusd, btceur, eurusd, 
// xrpusd, xrpeur, xrpbtc, ltcusd, ltceur, ltcbtc, ethusd, etheur, ethbtc, bchusd, bcheur, bchbtc
void 
BitsCPP::get_openOrders( const char *symbol, Json::Value &json_result ) 
{	

	BitsCPP_logger::write_log( "<BitsCPP::get_openOrders>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		BitsCPP_logger::write_log( "<BitsCPP::get_openOrders> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string message;
	string nonce;

	string url(BITSTAMP_HOST_v2);
	url += "/open_orders/";
	string action = "POST";

	// Append currency only if we provides in the request; if no currency is provided in the request, returns history for all currencies
    if ( symbol && symbol[0] )
    {	
        url.append( string_tolower(symbol) );
		url.append("/");
        // std::cout << "appending symbol" << std::endl;
    }

	nonce = to_string( get_current_ms_epoch() );
	string querystring("nonce=");
	querystring.append( nonce );

	// Signature is a HMAC-SHA256 encoded message containing nonce, customer ID (can be found here) and API key. 
	// The HMAC-SHA256 code must be generated using a secret key that was generated with your API key.
	// This code must be converted to it's hexadecimal representation (64 uppercase characters). 
	message = nonce + user_id + api_key;
	string signature =  hmac_sha256( secret_key.c_str() , message.c_str() );
	
	string_toupper( signature );
	querystring.append( "&signature=");
	querystring.append( signature );

	querystring.append("&key=");
    querystring.append( api_key );

	vector <string> extra_http_header;
	string header_chunk("Content-Type:");
	header_chunk.append( "application/x-www-form-urlencoded" );
	extra_http_header.push_back(header_chunk);

	BitsCPP_logger::write_log( "<BitsCPP::get_openOrders> url = |%s|" , url.c_str() ) ;
	
	string post_data = querystring;
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	BitsCPP_logger::write_log( "<BitsCPP::get_openOrders> Error ! %s", e.what() ); 
		}   
		BitsCPP_logger::write_log( "<BitsCPP::get_openOrders> Done." ) ;
	
	} else {
		BitsCPP_logger::write_log( "<BitsCPP::get_openOrders> Failed to get anything." ) ;
	}

	BitsCPP_logger::write_log( "<BitsCPP::get_openOrders> Done.\n" ) ;

}

// https://www.bitstamp.net/api/order_status/
// Get order status
void 
BitsCPP::get_orderStatus( const char *orderId, Json::Value &json_result ) 
{	

	BitsCPP_logger::write_log( "<BitsCPP::get_orderStatus>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		BitsCPP_logger::write_log( "<BitsCPP::get_orderStatus> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string message;
	string nonce;

	string url(BITSTAMP_HOST_v1);
	url += "/order_status/";
	string action = "POST";

	nonce = to_string( get_current_ms_epoch() );
	string querystring("nonce=");
	querystring.append( nonce );

	// Signature is a HMAC-SHA256 encoded message containing nonce, customer ID (can be found here) and API key. 
	// The HMAC-SHA256 code must be generated using a secret key that was generated with your API key.
	// This code must be converted to it's hexadecimal representation (64 uppercase characters). 
	message = nonce + user_id + api_key;
	string signature =  hmac_sha256( secret_key.c_str() , message.c_str() );
	
	string_toupper( signature );
	querystring.append( "&signature=");
	querystring.append( signature );

	querystring.append("&key=");
    querystring.append( api_key );

	querystring.append("&id=");
	querystring.append( orderId );

	vector <string> extra_http_header;
	string header_chunk("Content-Type:");
	header_chunk.append( "application/x-www-form-urlencoded" );
	extra_http_header.push_back(header_chunk);

	BitsCPP_logger::write_log( "<BitsCPP::get_orderStatus> url = |%s|" , url.c_str() ) ;
	
	string post_data = querystring;
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	BitsCPP_logger::write_log( "<BitsCPP::get_orderStatus> Error ! %s", e.what() ); 
		}   
		BitsCPP_logger::write_log( "<BitsCPP::get_orderStatus> Done." ) ;
	
	} else {
		BitsCPP_logger::write_log( "<BitsCPP::get_orderStatus> Failed to get anything." ) ;
	}

	BitsCPP_logger::write_log( "<BitsCPP::get_orderStatus> Done.\n" ) ;

}

// https://www.bitstamp.net/api/v2/cancel_order/
// Cancel ONE open order
void
BitsCPP::cancel_order( const char *orderId, Json::Value &json_result ) 
{	

	BitsCPP_logger::write_log( "<BitsCPP::cancel_order>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		BitsCPP_logger::write_log( "<BitsCPP::cancel_order> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string message;
	string nonce;

	string url(BITSTAMP_HOST_v2);
	url += "/cancel_order/";
	string action = "POST";

	nonce = to_string( get_current_ms_epoch() );
	string querystring("nonce=");
	querystring.append( nonce );

	// Signature is a HMAC-SHA256 encoded message containing nonce, customer ID (can be found here) and API key. 
	// The HMAC-SHA256 code must be generated using a secret key that was generated with your API key.
	// This code must be converted to it's hexadecimal representation (64 uppercase characters). 
	message = nonce + user_id + api_key;
	string signature =  hmac_sha256( secret_key.c_str() , message.c_str() );
	
	string_toupper( signature );
	querystring.append( "&signature=");
	querystring.append( signature );

	querystring.append("&key=");
    querystring.append( api_key );

	querystring.append("&id=");
	querystring.append( orderId );

	vector <string> extra_http_header;
	string header_chunk("Content-Type:");
	header_chunk.append( "application/x-www-form-urlencoded" );
	extra_http_header.push_back(header_chunk);

	BitsCPP_logger::write_log( "<BitsCPP::cancel_order> url = |%s|" , url.c_str() ) ;
	
	string post_data = querystring;
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	BitsCPP_logger::write_log( "<BitsCPP::cancel_order> Error ! %s", e.what() ); 
		}   
		BitsCPP_logger::write_log( "<BitsCPP::cancel_order> Done." ) ;
	
	} else {
		BitsCPP_logger::write_log( "<BitsCPP::cancel_order> Failed to get anything." ) ;
	}

	BitsCPP_logger::write_log( "<BitsCPP::cancel_order> Done.\n" ) ;

}

// Cancel all orders
// https://www.bitstamp.net/api/cancel_all_orders/
void 
BitsCPP::cancel_allOrders( Json::Value &json_result ) 
{	

	BitsCPP_logger::write_log( "<BitsCPP::cancel_allOrders>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		BitsCPP_logger::write_log( "<BitsCPP::cancel_allOrders> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string message;
	string nonce;

	string url(BITSTAMP_HOST_v1);
	url += "/cancel_all_orders/";
	string action = "POST";

	nonce = to_string( get_current_ms_epoch() );
	string querystring("nonce=");
	querystring.append( nonce );

	// Signature is a HMAC-SHA256 encoded message containing nonce, customer ID (can be found here) and API key. 
	// The HMAC-SHA256 code must be generated using a secret key that was generated with your API key.
	// This code must be converted to it's hexadecimal representation (64 uppercase characters). 
	message = nonce + user_id + api_key;
	string signature =  hmac_sha256( secret_key.c_str() , message.c_str() );
	
	string_toupper( signature );
	querystring.append( "&signature=");
	querystring.append( signature );

	querystring.append("&key=");
    querystring.append( api_key );

	vector <string> extra_http_header;
	string header_chunk("Content-Type:");
	header_chunk.append( "application/x-www-form-urlencoded" );
	extra_http_header.push_back(header_chunk);

	BitsCPP_logger::write_log( "<BitsCPP::cancel_allOrders> url = |%s|" , url.c_str() ) ;
	
	string post_data = querystring;
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	BitsCPP_logger::write_log( "<BitsCPP::cancel_allOrders> Error ! %s", e.what() ); 
		}   
		BitsCPP_logger::write_log( "<BitsCPP::cancel_allOrders> Done." ) ;
	
	} else {
		BitsCPP_logger::write_log( "<BitsCPP::cancel_allOrders> Failed to get anything." ) ;
	}

	BitsCPP_logger::write_log( "<BitsCPP::cancel_allOrders> Done.\n" ) ;

}

// Authenticated endpoints: send orders
// https://www.bitstamp.net/api/v2/buy/{currency_pair}/
// Supported values for currency_pair: btcusd, btceur, eurusd, 
// xrpusd, xrpeur, xrpbtc, ltcusd, ltceur, ltcbtc, ethusd, etheur, ethbtc, bchusd, bcheur, bchbtc
void 
BitsCPP::buy_limitOrder( const char *symbol, double amount, double price, double limit_price, const char *timeInForce, Json::Value &json_result ) 
{	

	BitsCPP_logger::write_log( "<BitsCPP::buy_limitOrder>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		BitsCPP_logger::write_log( "<BitsCPP::buy_limitOrder> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string message;
	string nonce;

	string url(BITSTAMP_HOST_v2);
	url += "/buy/";
	string action = "POST";

	if ( symbol && symbol[0] )
    {	
        url.append( string_tolower(symbol) );
		url.append("/");
        // std::cout << "appending symbol" << std::endl;
    }

	nonce = to_string( get_current_ms_epoch() );
	string querystring("nonce=");
	querystring.append( nonce );

	// Signature is a HMAC-SHA256 encoded message containing nonce, customer ID (can be found here) and API key. 
	// The HMAC-SHA256 code must be generated using a secret key that was generated with your API key.
	// This code must be converted to it's hexadecimal representation (64 uppercase characters). 
	message = nonce + user_id + api_key;
	string signature =  hmac_sha256( secret_key.c_str() , message.c_str() );
	
	string_toupper( signature );
	querystring.append( "&signature=");
	querystring.append( signature );

	querystring.append("&key=");
    querystring.append( api_key );

	querystring.append("&amount=");
	querystring.append( to_string(amount) );

	querystring.append("&price=");
	querystring.append( to_2decimal(price) );

	querystring.append("&limit_price=");
	querystring.append( to_2decimal(limit_price) );

	if ( timeInForce == "DAY" ) 
	{
		querystring.append("&daily_order");
		querystring.append( "True" );
	} else if ( timeInForce == "IOC" )
	{
		querystring.append("&loc_order=");
		querystring.append( "True" );
	} 

	vector <string> extra_http_header;
	string header_chunk("Content-Type:");
	header_chunk.append( "application/x-www-form-urlencoded" );
	extra_http_header.push_back(header_chunk);

	BitsCPP_logger::write_log( "<BitsCPP::buy_limitOrder> url = |%s|" , url.c_str() ) ;
	
	string post_data = querystring;
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	BitsCPP_logger::write_log( "<BitsCPP::buy_limitOrder> Error ! %s", e.what() ); 
		}   
		BitsCPP_logger::write_log( "<BitsCPP::buy_limitOrder> Done." ) ;
	
	} else {
		BitsCPP_logger::write_log( "<BitsCPP::buy_limitOrder> Failed to get anything." ) ;
	}

	BitsCPP_logger::write_log( "<BitsCPP::buy_limitOrder> Done.\n" ) ;

}

//  Sell limit order
// https://www.bitstamp.net/api/v2/sell/{currency_pair}/
// Supported values for currency_pair: btcusd, btceur, eurusd, xrpusd,
// xrpeur, xrpbtc, ltcusd, ltceur, ltcbtc, ethusd, etheur, ethbtc, bchusd, bcheur, bchbtc
void 
BitsCPP::sell_limitOrder( const char *symbol, double amount, double price, double limit_price, const char *timeInForce, Json::Value &json_result ) 
{	

	BitsCPP_logger::write_log( "<BitsCPP::sell_limitOrder>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		BitsCPP_logger::write_log( "<BitsCPP::sell_limitOrder> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string message;
	string nonce;

	string url(BITSTAMP_HOST_v2);
	url += "/sell/";
	string action = "POST";

	if ( symbol && symbol[0] )
    {	
        url.append( string_tolower(symbol) );
		url.append("/");
        // std::cout << "appending symbol" << std::endl;
    }

	nonce = to_string( get_current_ms_epoch() );
	string querystring("nonce=");
	querystring.append( nonce );

	message = nonce + user_id + api_key;
	string signature =  hmac_sha256( secret_key.c_str() , message.c_str() );
	
	string_toupper( signature );
	querystring.append( "&signature=");
	querystring.append( signature );

	querystring.append("&key=");
    querystring.append( api_key );

	querystring.append("&amount=");
	querystring.append( to_string(amount) );

	querystring.append("&price=");
	querystring.append( to_2decimal(price) );

	querystring.append("&limit_price=");
	querystring.append( to_2decimal(limit_price) );

	if ( timeInForce == "DAY" ) 
	{
		querystring.append("&daily_order");
		querystring.append( "True" );
	} else if ( timeInForce == "IOC" )
	{
		querystring.append("&loc_order=");
		querystring.append( "True" );
	} 

	vector <string> extra_http_header;
	string header_chunk("Content-Type:");
	header_chunk.append( "application/x-www-form-urlencoded" );
	extra_http_header.push_back(header_chunk);

	BitsCPP_logger::write_log( "<BitsCPP::sell_limitOrder> url = |%s|" , url.c_str() ) ;
	
	string post_data = querystring;
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	BitsCPP_logger::write_log( "<BitsCPP::sell_limitOrder> Error ! %s", e.what() ); 
		}   
		BitsCPP_logger::write_log( "<BitsCPP::sell_limitOrder> Done." ) ;
	
	} else {
		BitsCPP_logger::write_log( "<BitsCPP::sell_limitOrder> Failed to get anything." ) ;
	}

	BitsCPP_logger::write_log( "<BitsCPP::sell_limitOrder> Done.\n" ) ;

}


// Buy market order
// https://www.bitstamp.net/api/v2/buy/market/{currency_pair}/
// Supported values for currency_pair: btcusd, btceur, eurusd, xrpusd, 
// xrpeur, xrpbtc, ltcusd, ltceur, ltcbtc, ethusd, etheur, ethbtc, bchusd, bcheur, bchbtc
void 
BitsCPP::buy_marketOrder( const char *symbol, double amount, Json::Value &json_result ) 
{	

	BitsCPP_logger::write_log( "<BitsCPP::buy_marketOrder>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		BitsCPP_logger::write_log( "<BitsCPP::buy_marketOrder> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string message;
	string nonce;

	string url(BITSTAMP_HOST_v2);
	url += "/buy/market/";
	string action = "POST";

	if ( symbol && symbol[0] )
    {	
        url.append( string_tolower(symbol) );
		url.append("/");
        // std::cout << "appending symbol" << std::endl;
    }

	nonce = to_string( get_current_ms_epoch() );
	string querystring("nonce=");
	querystring.append( nonce );

	message = nonce + user_id + api_key;
	string signature =  hmac_sha256( secret_key.c_str() , message.c_str() );
	
	string_toupper( signature );
	querystring.append( "&signature=");
	querystring.append( signature );

	querystring.append("&key=");
    querystring.append( api_key );

	querystring.append("&amount=");
	querystring.append( to_string(amount) );

	vector <string> extra_http_header;
	string header_chunk("Content-Type:");
	header_chunk.append( "application/x-www-form-urlencoded" );
	extra_http_header.push_back(header_chunk);

	BitsCPP_logger::write_log( "<BitsCPP::buy_marketOrder> url = |%s|" , url.c_str() ) ;
	
	string post_data = querystring;
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	BitsCPP_logger::write_log( "<BitsCPP::buy_marketOrder> Error ! %s", e.what() ); 
		}   
		BitsCPP_logger::write_log( "<BitsCPP::buy_marketOrder> Done." ) ;
	
	} else {
		BitsCPP_logger::write_log( "<BitsCPP::buy_marketOrder> Failed to get anything." ) ;
	}

	BitsCPP_logger::write_log( "<BitsCPP::buy_marketOrder> Done.\n" ) ;

}

// Sell market order
// https://www.bitstamp.net/api/v2/sell/market/{currency_pair}/
// Supported values for currency_pair: btcusd, btceur, eurusd, xrpusd,
// xrpeur, xrpbtc, ltcusd, ltceur, ltcbtc, ethusd, etheur, ethbtc, bchusd, bcheur, bchbtc
void 
BitsCPP::sell_marketOrder( const char *symbol, double amount, Json::Value &json_result ) 
{	

	BitsCPP_logger::write_log( "<BitsCPP::sell_marketOrder>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		BitsCPP_logger::write_log( "<BitsCPP::sell_marketOrder> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string message;
	string nonce;

	string url(BITSTAMP_HOST_v2);
	url += "/sell/market/";
	string action = "POST";

	if ( symbol && symbol[0] )
    {	
        url.append( string_tolower(symbol) );
		url.append("/");
        // std::cout << "appending symbol" << std::endl;
    }

	nonce = to_string( get_current_ms_epoch() );
	string querystring("nonce=");
	querystring.append( nonce );

	message = nonce + user_id + api_key;
	string signature =  hmac_sha256( secret_key.c_str() , message.c_str() );
	
	string_toupper( signature );
	querystring.append( "&signature=");
	querystring.append( signature );

	querystring.append("&key=");
    querystring.append( api_key );

	querystring.append("&amount=");
	querystring.append( to_string(amount) );

	vector <string> extra_http_header;
	string header_chunk("Content-Type:");
	header_chunk.append( "application/x-www-form-urlencoded" );
	extra_http_header.push_back(header_chunk);

	BitsCPP_logger::write_log( "<BitsCPP::sell_marketOrder> url = |%s|" , url.c_str() ) ;
	
	string post_data = querystring;
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	BitsCPP_logger::write_log( "<BitsCPP::sell_marketOrder> Error ! %s", e.what() ); 
		}   
		BitsCPP_logger::write_log( "<BitsCPP::sell_marketOrder> Done." ) ;
	
	} else {
		BitsCPP_logger::write_log( "<BitsCPP::sell_marketOrder> Failed to get anything." ) ;
	}

	BitsCPP_logger::write_log( "<BitsCPP::sell_marketOrder> Done.\n" ) ;

}

// Buy instant order
// https://www.bitstamp.net/api/v2/buy/instant/{currency_pair}/
// Supported values for currency_pair: btcusd, btceur, eurusd, xrpusd,
// xrpeur, xrpbtc, ltcusd, ltceur, ltcbtc, ethusd, etheur, ethbtc, bchusd, bcheur, bchbtc
void 
BitsCPP::buy_instantOrder( const char *symbol, double amount, Json::Value &json_result ) 
{	

	BitsCPP_logger::write_log( "<BitsCPP::buy_marketOrder>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		BitsCPP_logger::write_log( "<BitsCPP::buy_marketOrder> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string message;
	string nonce;

	string url(BITSTAMP_HOST_v2);
	url += "/buy/instant/";
	string action = "POST";

	if ( symbol && symbol[0] )
    {	
        url.append( string_tolower(symbol) );
		url.append("/");
        // std::cout << "appending symbol" << std::endl;
    }

	nonce = to_string( get_current_ms_epoch() );
	string querystring("nonce=");
	querystring.append( nonce );

	message = nonce + user_id + api_key;
	string signature =  hmac_sha256( secret_key.c_str() , message.c_str() );
	
	string_toupper( signature );
	querystring.append( "&signature=");
	querystring.append( signature );

	querystring.append("&key=");
    querystring.append( api_key );

	querystring.append("&amount=");
	querystring.append( to_string(amount) );

	vector <string> extra_http_header;
	string header_chunk("Content-Type:");
	header_chunk.append( "application/x-www-form-urlencoded" );
	extra_http_header.push_back(header_chunk);

	BitsCPP_logger::write_log( "<BitsCPP::buy_marketOrder> url = |%s|" , url.c_str() ) ;
	
	string post_data = querystring;
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	BitsCPP_logger::write_log( "<BitsCPP::buy_marketOrder> Error ! %s", e.what() ); 
		}   
		BitsCPP_logger::write_log( "<BitsCPP::buy_marketOrder> Done." ) ;
	
	} else {
		BitsCPP_logger::write_log( "<BitsCPP::buy_marketOrder> Failed to get anything." ) ;
	}

	BitsCPP_logger::write_log( "<BitsCPP::buy_marketOrder> Done.\n" ) ;

}


// Sell instant order
// https://www.bitstamp.net/api/v2/sell/{currency_pair}/
// Supported values for currency_pair: btcusd, btceur, eurusd, xrpusd,
// xrpeur, xrpbtc, ltcusd, ltceur, ltcbtc, ethusd, etheur, ethbtc, bchusd, bcheur, bchbtc
void 
BitsCPP::sell_instantOrder( const char *symbol, double amount, Json::Value &json_result ) 
{	

	BitsCPP_logger::write_log( "<BitsCPP::sell_instantOrder>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		BitsCPP_logger::write_log( "<BitsCPP::sell_instantOrder> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string message;
	string nonce;

	string url(BITSTAMP_HOST_v2);
	url += "/sell/instant/";
	string action = "POST";

	if ( symbol && symbol[0] )
    {	
        url.append( string_tolower(symbol) );
		url.append("/");
        // std::cout << "appending symbol" << std::endl;
    }

	nonce = to_string( get_current_ms_epoch() );
	string querystring("nonce=");
	querystring.append( nonce );

	message = nonce + user_id + api_key;
	string signature =  hmac_sha256( secret_key.c_str() , message.c_str() );
	
	string_toupper( signature );
	querystring.append( "&signature=");
	querystring.append( signature );

	querystring.append("&key=");
    querystring.append( api_key );

	querystring.append("&amount=");
	querystring.append( to_string(amount) );

	vector <string> extra_http_header;
	string header_chunk("Content-Type:");
	header_chunk.append( "application/x-www-form-urlencoded" );
	extra_http_header.push_back(header_chunk);

	BitsCPP_logger::write_log( "<BitsCPP::sell_instantOrder> url = |%s|" , url.c_str() ) ;
	
	string post_data = querystring;
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	BitsCPP_logger::write_log( "<BitsCPP::sell_instantOrder> Error ! %s", e.what() ); 
		}   
		BitsCPP_logger::write_log( "<BitsCPP::sell_instantOrder> Done." ) ;
	
	} else {
		BitsCPP_logger::write_log( "<BitsCPP::sell_instantOrder> Failed to get anything." ) ;
	}

	BitsCPP_logger::write_log( "<BitsCPP::sell_instantOrder> Done.\n" ) ;

}

//--------------------
//Keepalive user data stream (API-KEY)
void 
BitsCPP::keep_userDataStream( const char *listenKey ) 
{	
	BitsCPP_logger::write_log( "<BitsCPP::keep_userDataStream>" ) ;

	if ( api_key.size() == 0 ) {
		BitsCPP_logger::write_log( "<BitsCPP::keep_userDataStream> API Key has not been set." ) ;
		return ;
	}


	string url(BITSTAMP_HOST_v2);
	url += "/api/v1/userDataStream";

	vector <string> extra_http_header;
	string header_chunk("X-MBX-APIKEY: ");
	

	header_chunk.append( api_key );
	extra_http_header.push_back(header_chunk);

	string action = "PUT";
	string post_data("listenKey=");
	post_data.append( listenKey );

	BitsCPP_logger::write_log( "<BitsCPP::keep_userDataStream> url = |%s|, post_data = |%s|" , url.c_str() , post_data.c_str() ) ;
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;

	if ( str_result.size() > 0 ) {
		
		BitsCPP_logger::write_log( "<BitsCPP::keep_userDataStream> Done." ) ;
	
	} else {
		BitsCPP_logger::write_log( "<BitsCPP::keep_userDataStream> Failed to get anything." ) ;
	}

	BitsCPP_logger::write_log( "<BitsCPP::keep_userDataStream> Done.\n" ) ;

}





//--------------------
//Keepalive user data stream (API-KEY)
void 
BitsCPP::close_userDataStream( const char *listenKey ) 
{	
	BitsCPP_logger::write_log( "<BitsCPP::close_userDataStream>" ) ;

	if ( api_key.size() == 0 ) {
		BitsCPP_logger::write_log( "<BitsCPP::close_userDataStream> API Key has not been set." ) ;
		return ;
	}


	string url(BITSTAMP_HOST_v2);
	url += "/api/v1/userDataStream";

	vector <string> extra_http_header;
	string header_chunk("X-MBX-APIKEY: ");
	

	header_chunk.append( api_key );
	extra_http_header.push_back(header_chunk);

	string action = "DELETE";
	string post_data("listenKey=");
	post_data.append( listenKey );

	BitsCPP_logger::write_log( "<BitsCPP::close_userDataStream> url = |%s|, post_data = |%s|" , url.c_str() , post_data.c_str() ) ;
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;

	if ( str_result.size() > 0 ) {
		
		BitsCPP_logger::write_log( "<BitsCPP::close_userDataStream> Done." ) ;
	
	} else {
		BitsCPP_logger::write_log( "<BitsCPP::close_userDataStream> Failed to get anything." ) ;
	}

	BitsCPP_logger::write_log( "<BitsCPP::close_userDataStream> Done.\n" ) ;

}






//-----------------
// Curl's callback
size_t 
BitsCPP::curl_cb( void *content, size_t size, size_t nmemb, std::string *buffer ) 
{	
	BitsCPP_logger::write_log( "<BitsCPP::curl_cb> " ) ;

	buffer->append((char*)content, size*nmemb);

	BitsCPP_logger::write_log( "<BitsCPP::curl_cb> done" ) ;
	return size*nmemb;
}






//--------------------------------------------------
void 
BitsCPP::curl_api( string &url, string &result_json ) {
	vector <string> v;
	string action = "GET";
	string post_data = "";
	curl_api_with_header( url , result_json , v, post_data , action );	
} 

// Ray mods to include followlocation option
void
BitsCPP::curl_api_loc( string &url, string &result_json ) {
    vector <string> v;
    string action = "GET";
    string post_data = "";
    curl_api_with_header_loc( url , result_json , v, post_data , action );
}

//--------------------
// Do the curl
void 
BitsCPP::curl_api_with_header( string &url, string &str_result, vector <string> &extra_http_header , string &post_data , string &action ) 
{
	BitsCPP_logger::write_log( "<BitsCPP::curl_api>" ) ;

	CURLcode res;

	if( BitsCPP::curl ) {

		curl_easy_setopt(BitsCPP::curl, CURLOPT_URL, url.c_str() );
		curl_easy_setopt(BitsCPP::curl, CURLOPT_WRITEFUNCTION, BitsCPP::curl_cb);
		curl_easy_setopt(BitsCPP::curl, CURLOPT_WRITEDATA, &str_result );
		curl_easy_setopt(BitsCPP::curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(BitsCPP::curl, CURLOPT_ENCODING, "gzip");

		if ( extra_http_header.size() > 0 ) {
			
			struct curl_slist *chunk = NULL;
			for ( int i = 0 ; i < extra_http_header.size() ;i++ ) {
				chunk = curl_slist_append(chunk, extra_http_header[i].c_str() );
			}
			curl_easy_setopt(BitsCPP::curl, CURLOPT_HTTPHEADER, chunk);
		}

		if ( post_data.size() > 0 || action == "POST" || action == "PUT" || action == "DELETE" ) {

			if ( action == "PUT" || action == "DELETE" ) {
				curl_easy_setopt(BitsCPP::curl, CURLOPT_CUSTOMREQUEST, action.c_str() );
			}
			std::cout << "POST request: " << post_data.c_str() << std::endl;
			curl_easy_setopt(BitsCPP::curl, CURLOPT_POSTFIELDS, post_data.c_str() );
 		}

		res = curl_easy_perform(BitsCPP::curl);

		/* Check for errors */ 
		if ( res != CURLE_OK ) {
			BitsCPP_logger::write_log( "<BitsCPP::curl_api> curl_easy_perform() failed: %s" , curl_easy_strerror(res) ) ;
		} 	

	}

	BitsCPP_logger::write_log( "<BitsCPP::curl_api> done" ) ;

}

// Ray changes to include follow location
// // Do the curl
void
BitsCPP::curl_api_with_header_loc( string &url, string &str_result, vector <string> &extra_http_header , string &post_data , string &action )
{
    BitsCPP_logger::write_log( "<BitsCPP::curl_api>" ) ;

    CURLcode res;

    if( BitsCPP::curl ) {

        curl_easy_setopt(BitsCPP::curl, CURLOPT_URL, url.c_str() );
        curl_easy_setopt(BitsCPP::curl, CURLOPT_WRITEFUNCTION, BitsCPP::curl_cb);
        curl_easy_setopt(BitsCPP::curl, CURLOPT_WRITEDATA, &str_result );
        curl_easy_setopt(BitsCPP::curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(BitsCPP::curl, CURLOPT_ENCODING, "gzip");
		curl_easy_setopt(BitsCPP::curl, CURLOPT_FOLLOWLOCATION, 1L);

        if ( extra_http_header.size() > 0 ) {

            struct curl_slist *chunk = NULL;
            for ( int i = 0 ; i < extra_http_header.size() ;i++ ) {
                chunk = curl_slist_append(chunk, extra_http_header[i].c_str() );
            }
            curl_easy_setopt(BitsCPP::curl, CURLOPT_HTTPHEADER, chunk);
        }

        if ( post_data.size() > 0 || action == "POST" || action == "PUT" || action == "DELETE" ) {

            if ( action == "PUT" || action == "DELETE" ) {
                curl_easy_setopt(BitsCPP::curl, CURLOPT_CUSTOMREQUEST, action.c_str() );
            }
            curl_easy_setopt(BitsCPP::curl, CURLOPT_POSTFIELDS, post_data.c_str() );
        }

        res = curl_easy_perform(BitsCPP::curl);

        /* Check for errors */
        if ( res != CURLE_OK ) {
            BitsCPP_logger::write_log( "<BitsCPP::curl_api> curl_easy_perform() failed: %s" , curl_easy_strerror(res) ) ;
        }

    }

    BitsCPP_logger::write_log( "<BitsCPP::curl_api> done" ) ;

}

