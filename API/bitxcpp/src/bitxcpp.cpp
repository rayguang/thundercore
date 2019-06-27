/*
	Author: Ray Guang
	Date  : 2019/04/22
	
	C++ library for Bittrex API.
*/

#include "bitxcpp.h"
#include "bitxcpp_logger.h"
#include "bitxcpp_utils.h"


BitxCPP::BitxCPP()
	: curl(NULL)
{
}

BitxCPP::~BitxCPP()
{
	if (NULL != BitxCPP::curl)
		cleanup();
}

//---------------------------------
bool
BitxCPP::init( string &api_key, string &secret_key ) 
{	
	BitxCPP::api_key = api_key;
	BitxCPP::secret_key = secret_key;

	BitxCPP::curl = curl_easy_init();

	return NULL != BitxCPP::curl;
}

void
BitxCPP::cleanup()
{
	curl_easy_cleanup(BitxCPP::curl);	
}


//----------------------
//Used to get the open and available trading markets at Bittrex along with other meta data
//GET /public/getmarkets
//----------------------
void 
BitxCPP::get_markets( Json::Value &json_result ) 
{
	BitxCPP_logger::write_log( "<BitxCPP::get_markets>" ) ;

	string url(BITTREX_HOST);  
	url += "/public/getmarkets";

	string str_result;
	curl_api( url, str_result ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
		} catch ( exception &e ) {
		 	BitxCPP_logger::write_log( "<BitxCPP::get_markets> Error ! %s", e.what() ); 
		}   
		BitxCPP_logger::write_log( "<BitxCPP::get_markets> Done." ) ;
	
	} else {
		BitxCPP_logger::write_log( "<BitxCPP::get_markets> Failed to get anything." ) ;
	}
}



//--------------------
// Used to get all supported currencies at Bittrex along with other meta data.
/*
	GET /public/getcurrencies
*/
void 
BitxCPP::get_currencies( Json::Value &json_result ) 
{	

	BitxCPP_logger::write_log( "<BitxCPP::get_currencies(>" ) ;

	string url(BITTREX_HOST);  
	url += "/public/getcurrencies";

	string str_result;
	curl_api( url, str_result ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
		} catch ( exception &e ) {
		 	BitxCPP_logger::write_log( "<BitxCPP::get_currencies> Error ! %s", e.what() ); 
		}   
		BitxCPP_logger::write_log( "<BitxCPP::get_currencies> Done." ) ;
	
	} else {
		BitxCPP_logger::write_log( "<BitxCPP::get_currencies> Failed to get anything." ) ;
	}
}


//----------
/*
	GET /public/getticker
*/
// Used to get the current tick values for a market.
void
BitxCPP::get_ticker( const char *symbol, Json::Value &json_result )
{
	BitxCPP_logger::write_log( "<BitxCPP::get_price>" ) ;

	double ret = 0.0;
	string str_symbol = string_toupper(symbol);
	string url(BITTREX_HOST);  
	url += "/public/getticker?market="+str_symbol;

	string str_result;
	curl_api( url, str_result );

	if ( str_result.size() > 0 ) {
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
		} catch ( exception &e ) {
		 	BitxCPP_logger::write_log( "<BitxCPP::get_ticker> Error ! %s", e.what() ); 
		}   
		BitxCPP_logger::write_log( "<BitxCPP::get_ticker> Done." ) ;
	
	} else {
		BitxCPP_logger::write_log( "<BitxCPP::get_ticker> Failed to get anything." ) ;
	}
}


/*
	GET /public/getmarketsummaries
*/
// Used to get the last 24 hour summary of all active markets.
void 
BitxCPP::get_marketsummaries( Json::Value &json_result ) 
{	

	BitxCPP_logger::write_log( "<BitxCPP::get_marketsummaries>" ) ;

	string url(BITTREX_HOST);  
	url += "/public/getmarketsummaries";

	string str_result;
	curl_api( url, str_result ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
	    		reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	BitxCPP_logger::write_log( "<BitxCPP::get_marketsummaries> Error ! %s", e.what() ); 
		}   
		BitxCPP_logger::write_log( "<BitxCPP::get_marketsummaries> Done." ) ;
	
	} else {
		BitxCPP_logger::write_log( "<BitxCPP::get_marketsummaries> Failed to get anything." ) ;
	}
}



/*
	GET /public/getmarketsummary
*/
// Used to get the last 24 hour summary of a specific market.
void 
BitxCPP::get_marketsummary( const char *symbol, Json::Value &json_result ) 
{	

	BitxCPP_logger::write_log( "<BitxCPP::get_marketsummary>" ) ;

	string str_symbol = string_toupper(symbol);
	string url(BITTREX_HOST);  
	url += "/public/getmarketsummary?market="+str_symbol;

	string str_result;
	curl_api( url, str_result ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
	    		reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	BitxCPP_logger::write_log( "<BitxCPP::get_marketsummary> Error ! %s", e.what() ); 
		}   
		BitxCPP_logger::write_log( "<BitxCPP::get_marketsummary> Done." ) ;
	
	} else {
		BitxCPP_logger::write_log( "<BitxCPP::get_marketsummary> Failed to get anything." ) ;
	}
}

/*
	GET /public/getorderbook
*/
//Used to get retrieve the orderbook for a given market.

void 
BitxCPP::get_orderBook( 
	const char *symbol, 
	const char *type, 
	Json::Value &json_result ) 
{	

	BitxCPP_logger::write_log( "<BitxCPP::get_orderBook>" ) ;

	string str_symbol = string_toupper(symbol);
	string url(BITTREX_HOST);  
	url += "/public/getorderbook?";

	string querystring("market=");
	querystring.append( str_symbol );
	querystring.append("&type=");
	querystring.append( type );

	url.append( querystring );
	BitxCPP_logger::write_log( "<BitxCPP::get_orderBook> url = |%s|" , url.c_str() ) ;
	
	string str_result;
	curl_api( url, str_result ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
	    		reader.parse( str_result , json_result );
	    		
		} catch ( exception &e ) {
		 	BitxCPP_logger::write_log( "<BitxCPP::get_orderBook> Error ! %s", e.what() ); 
		}   
		BitxCPP_logger::write_log( "<BitxCPP::get_orderBook> Done." ) ;
	
	} else {
		BitxCPP_logger::write_log( "<BitxCPP::get_orderBook> Failed to get anything." ) ;
	}
}

/*
	GET /public/getmarkethistory
*/
// Used to retrieve the latest trades that have occurred for a specific market.
void 
BitxCPP::get_marketHistory( const char *symbol, Json::Value &json_result ) 
{	

	BitxCPP_logger::write_log( "<BitxCPP::get_marketHistory>" ) ;

	string str_symbol = string_toupper(symbol);
	string url(BITTREX_HOST);  
	url += "/public/getmarkethistory?market="+str_symbol;

	string str_result;
	curl_api( url, str_result ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
	    		reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	BitxCPP_logger::write_log( "<BitxCPP::get_marketHistory> Error ! %s", e.what() ); 
		}   
		BitxCPP_logger::write_log( "<BitxCPP::get_marketHistory> Done." ) ;
	
	} else {
		BitxCPP_logger::write_log( "<BitxCPP::get_marketHistory> Failed to get anything." ) ;
	}
}



//++++++++++++++++++++++++++++++++++++++++++++++++++
// Private API endpoints begins here
//++++++++++++++++++++++++++++++++++++++++++++++++++

//--------------------
// Get current account information. (SIGNED)
/*
GET /account/getbalances

Used to retrieve all balances from your account.
https://api.bittrex.com/api/v1.1/account/getbalances?apikey=API_KEY
*/


void 
BitxCPP::get_balances( Json::Value &json_result ) 
{	

	BitxCPP_logger::write_log( "<BitxCPP::get_balances>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		BitxCPP_logger::write_log( "<BitxCPP::get_balances> API Key and Secret Key has not been set." ) ;
		return ;
	}


	string url(BITTREX_HOST);
	url += "/account/getbalances?";
	string action = "GET";
	
	string querystring("apikey=");
	querystring.append( api_key );

	querystring.append("&nonce=");
	querystring.append( to_string( get_current_ms_epoch() ) );

	string signature =  hmac_sha512( secret_key.c_str() , (url+querystring).c_str() );
	//querystring.append( "&signature=");
	//querystring.append( signature );
	//std::cout << "secret: " << secret_key.c_str() << std::endl;
	//std::cout << "query: " << querystring.c_str() << std::endl;
	//std::cout << "signature: " << signature << std::endl;

	url.append( querystring );
	vector <string> extra_http_header;
	//string header_chunk("X-MBX-APIKEY: ");
	//header_chunk.append( api_key );
	string header_chunk("apisign: ");
	header_chunk.append( signature );
	extra_http_header.push_back(header_chunk);

	BitxCPP_logger::write_log( "<BitxCPP::get_balances> url = |%s|" , url.c_str() ) ;
	
	string post_data = "";
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	BitxCPP_logger::write_log( "<BitxCPP::get_balances> Error ! %s", e.what() ); 
		}   
		BitxCPP_logger::write_log( "<BitxCPP::get_balances> Done." ) ;
	
	} else {
		BitxCPP_logger::write_log( "<BitxCPP::get_balances> Failed to get anything." ) ;
	}

	BitxCPP_logger::write_log( "<BitxCPP::get_balances> Done.\n" ) ;

}

// GET /account/getbalance
void 
BitxCPP::get_balance( const char *symbol, Json::Value &json_result ) 
{	

	BitxCPP_logger::write_log( "<BitxCPP::get_balance>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		BitxCPP_logger::write_log( "<BitxCPP::get_account> API Key and Secret Key has not been set." ) ;
		return ;
	}


	string url(BITTREX_HOST);
	url += "/account/getbalance?";
	string action = "GET";
	
	string querystring("apikey=");
	querystring.append( api_key );

	querystring.append("&currency=");
	querystring.append(  symbol );

	querystring.append("&nonce=");
	querystring.append( to_string( get_current_ms_epoch() ) );

	string signature =  hmac_sha512( secret_key.c_str() , (url+querystring).c_str() );

	url.append( querystring );
	vector <string> extra_http_header;
	string header_chunk("apisign: ");
	header_chunk.append( signature );
	extra_http_header.push_back(header_chunk);

	BitxCPP_logger::write_log( "<BitxCPP::get_balance> url = |%s|" , url.c_str() ) ;
	
	string post_data = "";
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	BitxCPP_logger::write_log( "<BitxCPP::get_balance> Error ! %s", e.what() ); 
		}   
		BitxCPP_logger::write_log( "<BitxCPP::get_balance> Done." ) ;
	
	} else {
		BitxCPP_logger::write_log( "<BitxCPP::get_balance> Failed to get anything." ) ;
	}

	BitxCPP_logger::write_log( "<BitxCPP::get_balance> Done.\n" ) ;

}

// GET /account/getdepositaddress
void 
BitxCPP::get_depositAddress( const char *symbol, Json::Value &json_result ) 
{	

	BitxCPP_logger::write_log( "<BitxCPP::get_depositAddress>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		BitxCPP_logger::write_log( "<BitxCPP::get_depositAddress> API Key and Secret Key has not been set." ) ;
		return ;
	}


	string url(BITTREX_HOST);
	url += "/account/getdepositaddress?";
	string action = "GET";
	
	string querystring("apikey=");
	querystring.append( api_key );

	querystring.append("&currency=");
	querystring.append(  symbol );

	querystring.append("&nonce=");
	querystring.append( to_string( get_current_ms_epoch() ) );

	string signature =  hmac_sha512( secret_key.c_str() , (url+querystring).c_str() );

	url.append( querystring );
	vector <string> extra_http_header;
	string header_chunk("apisign: ");
	header_chunk.append( signature );
	extra_http_header.push_back(header_chunk);

	BitxCPP_logger::write_log( "<BitxCPP::get_depositAddress> url = |%s|" , url.c_str() ) ;
	
	string post_data = "";
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	BitxCPP_logger::write_log( "<BitxCPP::get_depositAddress> Error ! %s", e.what() ); 
		}   
		BitxCPP_logger::write_log( "<BitxCPP::get_depositAddress> Done." ) ;
	
	} else {
		BitxCPP_logger::write_log( "<BitxCPP::get_depositAddress> Failed to get anything." ) ;
	}

	BitxCPP_logger::write_log( "<BitxCPP::get_depositAddress> Done.\n" ) ;

}

// GET /account/withdraw
void 
BitxCPP::withdraw( const char *symbol, double quantity, const char *address, Json::Value &json_result ) 
{	

	BitxCPP_logger::write_log( "<BitxCPP::withdraw>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		BitxCPP_logger::write_log( "<BitxCPP::withdraw> API Key and Secret Key has not been set." ) ;
		return ;
	}


	string url(BITTREX_HOST);
	url += "/account/withdraw?";
	string action = "GET";
	
	string querystring("apikey=");
	querystring.append( api_key );

	querystring.append("&currency=");
	querystring.append(  symbol );

	querystring.append("&quantity=");
	querystring.append( to_string (quantity) );

	querystring.append("&address");
	querystring.append( address );

	querystring.append("&nonce=");
	querystring.append( to_string( get_current_ms_epoch() ) );

	string signature =  hmac_sha512( secret_key.c_str() , (url+querystring).c_str() );

	url.append( querystring );
	vector <string> extra_http_header;
	string header_chunk("apisign: ");
	header_chunk.append( signature );
	extra_http_header.push_back(header_chunk);

	BitxCPP_logger::write_log( "<BitxCPP::withdraw> url = |%s|" , url.c_str() ) ;
	
	string post_data = "";
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	BitxCPP_logger::write_log( "<BitxCPP::withdraw> Error ! %s", e.what() ); 
		}   
		BitxCPP_logger::write_log( "<BitxCPP::withdraw> Done." ) ;
	
	} else {
		BitxCPP_logger::write_log( "<BitxCPP::withdraw> Failed to get anything." ) ;
	}

	BitxCPP_logger::write_log( "<BitxCPP::withdraw> Done.\n" ) ;

}

// GET /account/getorder
void 
BitxCPP::get_order( const char *orderId, Json::Value &json_result ) 
{	

	BitxCPP_logger::write_log( "<BitxCPP::get_order>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		BitxCPP_logger::write_log( "<BitxCPP::get_order> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string url(BITTREX_HOST);
	url += "/account/getorder?";
	string action = "GET";
	
	string querystring("apikey=");
	querystring.append( api_key );

	querystring.append("&uuid=");
	querystring.append( orderId );

	querystring.append("&nonce=");
	querystring.append( to_string( get_current_ms_epoch() ) );

	string signature =  hmac_sha512( secret_key.c_str() , (url+querystring).c_str() );

	url.append( querystring );
	vector <string> extra_http_header;
	string header_chunk("apisign: ");
	header_chunk.append( signature );
	extra_http_header.push_back(header_chunk);

	BitxCPP_logger::write_log( "<BitxCPP::get_order> url = |%s|" , url.c_str() ) ;
	
	string post_data = "";
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	BitxCPP_logger::write_log( "<BitxCPP::get_order> Error ! %s", e.what() ); 
		}   
		BitxCPP_logger::write_log( "<BitxCPP::get_order> Done." ) ;
	
	} else {
		BitxCPP_logger::write_log( "<BitxCPP::get_order> Failed to get anything." ) ;
	}

	BitxCPP_logger::write_log( "<BitxCPP::get_order> Done.\n" ) ;

}


// GET /account/getorderhistory
void 
BitxCPP::get_orderHistory( Json::Value &json_result ) 
{	

	BitxCPP_logger::write_log( "<BitxCPP::get_orderHistory>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		BitxCPP_logger::write_log( "<BitxCPP::get_orderHistory> API Key and Secret Key has not been set." ) ;
		return ;
	}


	string url(BITTREX_HOST);
	url += "/account/getorderhistory?";
	string action = "GET";
	
	string querystring("apikey=");
	querystring.append( api_key );
	

	querystring.append("&nonce=");
	querystring.append( to_string( get_current_ms_epoch() ) );

	string signature =  hmac_sha512( secret_key.c_str() , (url+querystring).c_str() );

	url.append( querystring );
	vector <string> extra_http_header;
	string header_chunk("apisign: ");
	header_chunk.append( signature );
	extra_http_header.push_back(header_chunk);

	BitxCPP_logger::write_log( "<BitxCPP::get_orderHistory> url = |%s|" , url.c_str() ) ;
	
	string post_data = "";
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	BitxCPP_logger::write_log( "<BitxCPP::get_orderHistory> Error ! %s", e.what() ); 
		}   
		BitxCPP_logger::write_log( "<BitxCPP::get_orderHistory> Done." ) ;
	
	} else {
		BitxCPP_logger::write_log( "<BitxCPP::get_orderHistory> Failed to get anything." ) ;
	}

	BitxCPP_logger::write_log( "<BitxCPP::get_orderHistory> Done.\n" ) ;

}

// GET /account/getwithdrawalhistory
void 
BitxCPP::get_withdrawalHistory( const char *symbol, Json::Value &json_result ) 
{	

	BitxCPP_logger::write_log( "<BitxCPP::get_withdrawalHistory>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		BitxCPP_logger::write_log( "<BitxCPP::get_withdrawalHistory> API Key and Secret Key has not been set." ) ;
		return ;
	}


	string url(BITTREX_HOST);
	url += "/account/getwithdrawalhistory?";
	string action = "GET";
	
	string querystring("apikey=");
	querystring.append( api_key );
	
	// Append currency only if we provides in the request; if no currency is provided in the request, returns history for all currencies
    if ( symbol && symbol[0] )
    {
        querystring.append("&currency=");
        querystring.append( symbol );
        // std::cout << "appending symbol" << std::endl;
    }

	querystring.append("&nonce=");
	querystring.append( to_string( get_current_ms_epoch() ) );

	string signature =  hmac_sha512( secret_key.c_str() , (url+querystring).c_str() );

	url.append( querystring );
	vector <string> extra_http_header;
	string header_chunk("apisign: ");
	header_chunk.append( signature );
	extra_http_header.push_back(header_chunk);

	BitxCPP_logger::write_log( "<BitxCPP::get_withdrawalHistory> url = |%s|" , url.c_str() ) ;
	
	string post_data = "";
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	BitxCPP_logger::write_log( "<BitxCPP::get_withdrawalHistory> Error ! %s", e.what() ); 
		}   
		BitxCPP_logger::write_log( "<BitxCPP::get_withdrawalHistory> Done." ) ;
	
	} else {
		BitxCPP_logger::write_log( "<BitxCPP::get_withdrawalHistory> Failed to get anything." ) ;
	}

	BitxCPP_logger::write_log( "<BitxCPP::get_withdrawalHistory> Done.\n" ) ;

}


// GET /account/getdeposithistory
void 
BitxCPP::get_depositHistory( const char *symbol, Json::Value &json_result ) 
{	

	BitxCPP_logger::write_log( "<BitxCPP::get_depositHistory>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		BitxCPP_logger::write_log( "<BitxCPP::get_depositHistory> API Key and Secret Key has not been set." ) ;
		return ;
	}


	string url(BITTREX_HOST);
	url += "/account/getdeposithistory?";
	string action = "GET";
	
	string querystring("apikey=");
	querystring.append( api_key );
	
	// Append currency only if we provides in the request; if no currency is provided in the request, returns history for all currencies
	if ( symbol && symbol[0] )
	{ 
		querystring.append("&currency=");
		querystring.append( symbol );
		// std::cout << "appending symbol" << std::endl;
	} 

	querystring.append("&nonce=");
	querystring.append( to_string( get_current_ms_epoch() ) );

	string signature =  hmac_sha512( secret_key.c_str() , (url+querystring).c_str() );

	url.append( querystring );
	vector <string> extra_http_header;
	string header_chunk("apisign: ");
	header_chunk.append( signature );
	extra_http_header.push_back(header_chunk);

	BitxCPP_logger::write_log( "<BitxCPP::get_depositHistory> url = |%s|" , url.c_str() ) ;
	
	string post_data = "";
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	BitxCPP_logger::write_log( "<BitxCPP::get_depositHistory> Error ! %s", e.what() ); 
		}   
		BitxCPP_logger::write_log( "<BitxCPP::get_depositHistory> Done." ) ;
	
	} else {
		BitxCPP_logger::write_log( "<BitxCPP::get_depositHistory> Failed to get anything." ) ;
	}

	BitxCPP_logger::write_log( "<BitxCPP::get_depositHistory> Done.\n" ) ;

}


// ****************************************
// Authenticated API: market
// ****************************************
// GET /market/buylimit
// Used to place a buy order in a specific market. Use buylimit to place limit orders. 
// Make sure you have the proper permissions set on your API keys for this call to work.
void 
BitxCPP::buy_limit( const char *symbol, double quantity, double rate, Json::Value &json_result ) 
{	

	BitxCPP_logger::write_log( "<BitxCPP::buy_limit>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		BitxCPP_logger::write_log( "<BitxCPP::buy_limit> API Key and Secret Key has not been set." ) ;
		return ;
	}


	string url(BITTREX_HOST);
	url += "/market/buylimit?";
	string action = "GET";
	
	string querystring("apikey=");
	querystring.append( api_key );
	
	querystring.append("&market=");
	querystring.append( symbol );

	querystring.append("&quantity=");
	querystring.append( to_string(quantity) );

	querystring.append("&rate=");
	querystring.append( to_string(rate) );

	querystring.append("&nonce=");
	querystring.append( to_string( get_current_ms_epoch() ) );

	string signature =  hmac_sha512( secret_key.c_str() , (url+querystring).c_str() );

	url.append( querystring );
	vector <string> extra_http_header;
	string header_chunk("apisign: ");
	header_chunk.append( signature );
	extra_http_header.push_back(header_chunk);

	BitxCPP_logger::write_log( "<BitxCPP::buy_limit> url = |%s|" , url.c_str() ) ;
	
	string post_data = "";
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	BitxCPP_logger::write_log( "<BitxCPP::buy_limit> Error ! %s", e.what() ); 
		}   
		BitxCPP_logger::write_log( "<BitxCPP::buy_limit> Done." ) ;
	
	} else {
		BitxCPP_logger::write_log( "<BitxCPP::buy_limit> Failed to get anything." ) ;
	}

	BitxCPP_logger::write_log( "<BitxCPP::buy_limit> Done.\n" ) ;

}


// GET /market/selllimit
// Used to place an sell order in a specific market. Use selllimit to place limit orders. 
// Make sure you have the proper permissions set on your API keys for this call to work.
void 
BitxCPP::sell_limit( const char *symbol, double quantity, double rate, Json::Value &json_result ) 
{	

	BitxCPP_logger::write_log( "<BitxCPP::sell_limit>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		BitxCPP_logger::write_log( "<BitxCPP::sell_limit> API Key and Secret Key has not been set." ) ;
		return ;
	}


	string url(BITTREX_HOST);
	url += "/market/selllimit?";
	string action = "GET";
	
	string querystring("apikey=");
	querystring.append( api_key );
	
	querystring.append("&market=");
	querystring.append( symbol );

	querystring.append("&quantity=");
	querystring.append( to_string(quantity) );

	querystring.append("&rate=");
	querystring.append( to_string(rate) );

	querystring.append("&nonce=");
	querystring.append( to_string( get_current_ms_epoch() ) );

	string signature =  hmac_sha512( secret_key.c_str() , (url+querystring).c_str() );

	url.append( querystring );
	vector <string> extra_http_header;
	string header_chunk("apisign: ");
	header_chunk.append( signature );
	extra_http_header.push_back(header_chunk);

	BitxCPP_logger::write_log( "<BitxCPP::sell_limit> url = |%s|" , url.c_str() ) ;
	
	string post_data = "";
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	BitxCPP_logger::write_log( "<BitxCPP::sell_limit> Error ! %s", e.what() ); 
		}   
		BitxCPP_logger::write_log( "<BitxCPP::sell_limit> Done." ) ;
	
	} else {
		BitxCPP_logger::write_log( "<BitxCPP::sell_limit> Failed to get anything." ) ;
	}

	BitxCPP_logger::write_log( "<BitxCPP::sell_limit> Done.\n" ) ;

}

// GET /market/cancel
// Used to cancel a buy or sell order.
void 
BitxCPP::cancel_oneOrder( const char *orderId, Json::Value &json_result ) 
{	

	BitxCPP_logger::write_log( "<BitxCPP::cancel_oneOrder>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		BitxCPP_logger::write_log( "<BitxCPP::cancel_oneOrder> API Key and Secret Key has not been set." ) ;
		return ;
	}


	string url(BITTREX_HOST);
	url += "/market/cancel?";
	string action = "GET";
	
	string querystring("apikey=");
	querystring.append( api_key );
	
	querystring.append("&uuid=");
	querystring.append( orderId );

	querystring.append("&nonce=");
	querystring.append( to_string( get_current_ms_epoch() ) );

	string signature =  hmac_sha512( secret_key.c_str() , (url+querystring).c_str() );

	url.append( querystring );
	vector <string> extra_http_header;
	string header_chunk("apisign: ");
	header_chunk.append( signature );
	extra_http_header.push_back(header_chunk);

	BitxCPP_logger::write_log( "<BitxCPP::cancel_oneOrder> url = |%s|" , url.c_str() ) ;
	
	string post_data = "";
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	BitxCPP_logger::write_log( "<BitxCPP::cancel_oneOrder> Error ! %s", e.what() ); 
		}   
		BitxCPP_logger::write_log( "<BitxCPP::cancel_oneOrder> Done." ) ;
	
	} else {
		BitxCPP_logger::write_log( "<BitxCPP::cancel_oneOrder> Failed to get anything." ) ;
	}

	BitxCPP_logger::write_log( "<BitxCPP::cancel_oneOrder> Done.\n" ) ;

}

//--------------------
//Start user data stream (API-KEY)

void 
BitxCPP::start_userDataStream( Json::Value &json_result ) 
{	
	BitxCPP_logger::write_log( "<BitxCPP::start_userDataStream>" ) ;

	if ( api_key.size() == 0 ) {
		BitxCPP_logger::write_log( "<BitxCPP::start_userDataStream> API Key has not been set." ) ;
		return ;
	}


	string url(BITTREX_HOST);
	url += "/api/v1/userDataStream";

	vector <string> extra_http_header;
	string header_chunk("X-MBX-APIKEY: ");
	

	header_chunk.append( api_key );
	extra_http_header.push_back(header_chunk);

	BitxCPP_logger::write_log( "<BitxCPP::start_userDataStream> url = |%s|" , url.c_str() ) ;
	
	string action = "POST";
	string post_data = "";

	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	BitxCPP_logger::write_log( "<BitxCPP::start_userDataStream> Error ! %s", e.what() ); 
		}   
		BitxCPP_logger::write_log( "<BitxCPP::start_userDataStream> Done." ) ;
	
	} else {
		BitxCPP_logger::write_log( "<BitxCPP::start_userDataStream> Failed to get anything." ) ;
	}

	BitxCPP_logger::write_log( "<BitxCPP::start_userDataStream> Done.\n" ) ;

}

// GET /market/getopenordersvoid
// Get all orders that you currently have opened. A specific market can be requested.
void
BitxCPP::get_openOrders( const char *market, Json::Value &json_result ) 
{	

	BitxCPP_logger::write_log( "<BitxCPP::get_openOrders>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		BitxCPP_logger::write_log( "<BitxCPP::get_openOrders> API Key and Secret Key has not been set." ) ;
		return ;
	}


	string url(BITTREX_HOST);
	url += "/market/getopenorders?";
	string action = "GET";
	
	string querystring("apikey=");
	querystring.append( api_key );
	
	// Append market only if we provides in the request; if no market is provided in the request, 
	// returns history for all currencies
	if ( market && market[0] )
	{ 
		querystring.append("&market=");
		querystring.append( market );
	} 

	querystring.append("&nonce=");
	querystring.append( to_string( get_current_ms_epoch() ) );

	string signature =  hmac_sha512( secret_key.c_str() , (url+querystring).c_str() );

	url.append( querystring );
	vector <string> extra_http_header;
	string header_chunk("apisign: ");
	header_chunk.append( signature );
	extra_http_header.push_back(header_chunk);

	BitxCPP_logger::write_log( "<BitxCPP::get_openOrders> url = |%s|" , url.c_str() ) ;
	
	string post_data = "";
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	BitxCPP_logger::write_log( "<BitxCPP::get_openOrders> Error ! %s", e.what() ); 
		}   
		BitxCPP_logger::write_log( "<BitxCPP::get_openOrders> Done." ) ;
	
	} else {
		BitxCPP_logger::write_log( "<BitxCPP::get_openOrders> Failed to get anything." ) ;
	}

	BitxCPP_logger::write_log( "<BitxCPP::get_openOrders> Done.\n" ) ;

}








//--------------------
//Keepalive user data stream (API-KEY)
void 
BitxCPP::keep_userDataStream( const char *listenKey ) 
{	
	BitxCPP_logger::write_log( "<BitxCPP::keep_userDataStream>" ) ;

	if ( api_key.size() == 0 ) {
		BitxCPP_logger::write_log( "<BitxCPP::keep_userDataStream> API Key has not been set." ) ;
		return ;
	}


	string url(BITTREX_HOST);
	url += "/api/v1/userDataStream";

	vector <string> extra_http_header;
	string header_chunk("X-MBX-APIKEY: ");
	

	header_chunk.append( api_key );
	extra_http_header.push_back(header_chunk);

	string action = "PUT";
	string post_data("listenKey=");
	post_data.append( listenKey );

	BitxCPP_logger::write_log( "<BitxCPP::keep_userDataStream> url = |%s|, post_data = |%s|" , url.c_str() , post_data.c_str() ) ;
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;

	if ( str_result.size() > 0 ) {
		
		BitxCPP_logger::write_log( "<BitxCPP::keep_userDataStream> Done." ) ;
	
	} else {
		BitxCPP_logger::write_log( "<BitxCPP::keep_userDataStream> Failed to get anything." ) ;
	}

	BitxCPP_logger::write_log( "<BitxCPP::keep_userDataStream> Done.\n" ) ;

}





//--------------------
//Keepalive user data stream (API-KEY)
void 
BitxCPP::close_userDataStream( const char *listenKey ) 
{	
	BitxCPP_logger::write_log( "<BitxCPP::close_userDataStream>" ) ;

	if ( api_key.size() == 0 ) {
		BitxCPP_logger::write_log( "<BitxCPP::close_userDataStream> API Key has not been set." ) ;
		return ;
	}


	string url(BITTREX_HOST);
	url += "/api/v1/userDataStream";

	vector <string> extra_http_header;
	string header_chunk("X-MBX-APIKEY: ");
	

	header_chunk.append( api_key );
	extra_http_header.push_back(header_chunk);

	string action = "DELETE";
	string post_data("listenKey=");
	post_data.append( listenKey );

	BitxCPP_logger::write_log( "<BitxCPP::close_userDataStream> url = |%s|, post_data = |%s|" , url.c_str() , post_data.c_str() ) ;
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;

	if ( str_result.size() > 0 ) {
		
		BitxCPP_logger::write_log( "<BitxCPP::close_userDataStream> Done." ) ;
	
	} else {
		BitxCPP_logger::write_log( "<BitxCPP::close_userDataStream> Failed to get anything." ) ;
	}

	BitxCPP_logger::write_log( "<BitxCPP::close_userDataStream> Done.\n" ) ;

}






//-----------------
// Curl's callback
size_t 
BitxCPP::curl_cb( void *content, size_t size, size_t nmemb, std::string *buffer ) 
{	
	BitxCPP_logger::write_log( "<BitxCPP::curl_cb> " ) ;

	buffer->append((char*)content, size*nmemb);

	BitxCPP_logger::write_log( "<BitxCPP::curl_cb> done" ) ;
	return size*nmemb;
}






//--------------------------------------------------
void 
BitxCPP::curl_api( string &url, string &result_json ) {
	vector <string> v;
	string action = "GET";
	string post_data = "";
	curl_api_with_header( url , result_json , v, post_data , action );	
} 



//--------------------
// Do the curl
void 
BitxCPP::curl_api_with_header( string &url, string &str_result, vector <string> &extra_http_header , string &post_data , string &action ) 
{
	BitxCPP_logger::write_log( "<BitxCPP::curl_api>" ) ;

	CURLcode res;

	if( BitxCPP::curl ) {

		curl_easy_setopt(BitxCPP::curl, CURLOPT_URL, url.c_str() );
		curl_easy_setopt(BitxCPP::curl, CURLOPT_WRITEFUNCTION, BitxCPP::curl_cb);
		curl_easy_setopt(BitxCPP::curl, CURLOPT_WRITEDATA, &str_result );
		curl_easy_setopt(BitxCPP::curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(BitxCPP::curl, CURLOPT_ENCODING, "gzip");

		if ( extra_http_header.size() > 0 ) {
			
			struct curl_slist *chunk = NULL;
			for ( int i = 0 ; i < extra_http_header.size() ;i++ ) {
				chunk = curl_slist_append(chunk, extra_http_header[i].c_str() );
			}
			curl_easy_setopt(BitxCPP::curl, CURLOPT_HTTPHEADER, chunk);
		}

		if ( post_data.size() > 0 || action == "POST" || action == "PUT" || action == "DELETE" ) {

			if ( action == "PUT" || action == "DELETE" ) {
				curl_easy_setopt(BitxCPP::curl, CURLOPT_CUSTOMREQUEST, action.c_str() );
			}
			curl_easy_setopt(BitxCPP::curl, CURLOPT_POSTFIELDS, post_data.c_str() );
 		}

		res = curl_easy_perform(BitxCPP::curl);

		/* Check for errors */ 
		if ( res != CURLE_OK ) {
			BitxCPP_logger::write_log( "<BitxCPP::curl_api> curl_easy_perform() failed: %s" , curl_easy_strerror(res) ) ;
		} 	

	}

	BitxCPP_logger::write_log( "<BitxCPP::curl_api> done" ) ;

}



