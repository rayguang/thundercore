/*
	Author: Ray Guang
	Date  : 2019/04/24
	
	C++ library for Kraken API.
*/

#include "krakcpp.h"
#include "krakcpp_logger.h"
#include "krakcpp_utils.h"
#include "krakcpp_hmac.h"


KrakCPP::KrakCPP()
	: curl(NULL)
{
}

KrakCPP::~KrakCPP()
{
	if (NULL != KrakCPP::curl)
		cleanup();
}

//---------------------------------
bool
KrakCPP::init( string &api_key, string &secret_key ) 
{	
	KrakCPP::api_key = api_key;
	KrakCPP::secret_key = secret_key;

	KrakCPP::curl = curl_easy_init();

	return NULL != KrakCPP::curl;
}

// Signature required: key, nonce, and user_id
// Constructor to include user_id
bool
KrakCPP::init( string &api_key, string &secret_key, string &two_factor ) 
{	
	KrakCPP::api_key = api_key;
	KrakCPP::secret_key = secret_key;
	KrakCPP::two_factor = two_factor;

	KrakCPP::curl = curl_easy_init();

	return NULL != KrakCPP::curl;
}

void
KrakCPP::cleanup()
{
	curl_easy_cleanup(KrakCPP::curl);	
}

std::string
KrakCPP::signature(const string& path, const string& nonce, const string& postdata)
{
    std::cout << "path:\t" << path << std::endl;
    std::cout << "nonce:\t" << nonce << std::endl;
    std::cout << "postdata:\t" << postdata << std::endl;

    std::vector<unsigned char> data(path.begin(), path.end());
    std::vector<unsigned char> nonce_postdata = sha256_krak(nonce + postdata);

    data.insert(data.end(), nonce_postdata.begin(), nonce_postdata.end());
    return b64_encode( hmac_sha512_krak(data, b64_decode(secret_key)) );
}

//++++++++++++++++++++++++++++++++++++++++++++++++++
// Public API endpoints begins here
//++++++++++++++++++++++++++++++++++++++++++++++++++
// Result: Server's time
// Note: This is to aid in approximating the skew time between the server and client.
void 
KrakCPP::get_serverTime( Json::Value &json_result ) 
{
	KrakCPP_logger::write_log( "<KrakCPP::get_serverTime>" ) ;

	string url(KRAKEN_HOST_PUBLIC);  
	url += "/Time";
	
	std::cout << "URL request: " << url << std::endl;

	string str_result;
	curl_api( url, str_result ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
		} catch ( exception &e ) {
		 	KrakCPP_logger::write_log( "<KrakCPP::get_serverTime> Error ! %s", e.what() ); 
		}   
		KrakCPP_logger::write_log( "<KrakCPP::get_serverTime> Done." ) ;
	
	} else {
		KrakCPP_logger::write_log( "<KrakCPP::get_serverTime> Failed to get anything." ) ;
	}
}

// Get asset info
// URL: https://api.kraken.com/0/public/Assets
// https://api.kraken.com/0/public/Assets?asset=ADA
void 
KrakCPP::get_assetInfo( const char* symbol, Json::Value &json_result ) 
{
	KrakCPP_logger::write_log( "<KrakCPP::get_assetInfo>" ) ;

	string url(KRAKEN_HOST_PUBLIC);  
	url += "/Assets";

 	if ( symbol && symbol[0] )
    {
		url.append("?asset=");
        url.append( symbol );
        // std::cout << "appending symbol" << std::endl;
    }
	
	std::cout << "URL request: " << url << std::endl;

	string str_result;
	curl_api( url, str_result ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
		} catch ( exception &e ) {
		 	KrakCPP_logger::write_log( "<KrakCPP::get_assetInfo> Error ! %s", e.what() ); 
		}   
		KrakCPP_logger::write_log( "<KrakCPP::get_assetInfo> Done." ) ;
	
	} else {
		KrakCPP_logger::write_log( "<KrakCPP::get_assetInfo> Failed to get anything." ) ;
	}
}

// Get tradable asset pairs
// URL: https://api.kraken.com/0/public/AssetPairs
// https://api.kraken.com/0/public/AssetPairs?pair=adacad
void 
KrakCPP::get_assetPairs( const char* symbol, Json::Value &json_result ) 
{
	KrakCPP_logger::write_log( "<KrakCPP::get_assetPairs>" ) ;

	string url(KRAKEN_HOST_PUBLIC);  
	url += "/AssetPairs";

 	if ( symbol && symbol[0] )
    {
		url.append("?pair=");
        url.append( symbol );
        // std::cout << "appending symbol" << std::endl;
    }
	
	std::cout << "URL request: " << url << std::endl;

	string str_result;
	curl_api( url, str_result ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
		} catch ( exception &e ) {
		 	KrakCPP_logger::write_log( "<KrakCPP::get_assetPairs> Error ! %s", e.what() ); 
		}   
		KrakCPP_logger::write_log( "<KrakCPP::get_assetPairs> Done." ) ;
	
	} else {
		KrakCPP_logger::write_log( "<KrakCPP::get_assetPairs> Failed to get anything." ) ;
	}
}

// Get ticker information
// URL: https://api.kraken.com/0/public/Ticker
// https://api.kraken.com/0/public/Ticker?pair=ADACAD
void 
KrakCPP::get_ticker( const char* symbol, Json::Value &json_result ) 
{
	KrakCPP_logger::write_log( "<KrakCPP::get_ticker>" ) ;

	string url(KRAKEN_HOST_PUBLIC);  
	url += "/Ticker";

	string querystring("?pair=");
	querystring.append( symbol );
	url.append( querystring );
		
	std::cout << "URL request: " << url << std::endl;

	string str_result;
	curl_api( url, str_result ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
		} catch ( exception &e ) {
		 	KrakCPP_logger::write_log( "<KrakCPP::get_ticker> Error ! %s", e.what() ); 
		}   
		KrakCPP_logger::write_log( "<KrakCPP::get_ticker> Done." ) ;
	
	} else {
		KrakCPP_logger::write_log( "<KrakCPP::get_ticker> Failed to get anything." ) ;
	}
}

// Get order book
// URL: https://api.kraken.com/0/public/Depth
// https://api.kraken.com/0/public/Depth?pair=adacad
// https://api.kraken.com/0/public/Depth?pair=adacad&count=2 (count optional)
void 
KrakCPP::get_orderBook( const char* symbol, Json::Value &json_result, int count ) 
{
	KrakCPP_logger::write_log( "<KrakCPP::get_orderBook>" ) ;

	string url(KRAKEN_HOST_PUBLIC);  
	url += "/Depth";

	string querystring("?pair=");
	querystring.append( symbol );
	url.append( querystring );

	if ( count > 0 ){
		url.append("&count=");
		url.append(to_string(count));
	} else {
		url.append ("&count=10");
	}

		
	std::cout << "URL request: " << url << std::endl;

	string str_result;
	curl_api( url, str_result ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
		} catch ( exception &e ) {
		 	KrakCPP_logger::write_log( "<KrakCPP::get_orderBook> Error ! %s", e.what() ); 
		}   
		KrakCPP_logger::write_log( "<KrakCPP::get_orderBook> Done." ) ;
	
	} else {
		KrakCPP_logger::write_log( "<KrakCPP::get_orderBook> Failed to get anything." ) ;
	}
}

// Get recent trades
// URL: https://api.kraken.com/0/public/Trades
// pair = asset pair to get trade data for
// since = return trade data since given id (optional.  exclusive)
// https://api.kraken.com/0/public/Trades?pair=ADACAD&since=1557191323956868434
void 
KrakCPP::get_recentTrades( const char* symbol, Json::Value &json_result, const char* id ) 
{
	KrakCPP_logger::write_log( "<KrakCPP::get_recentTrades>" ) ;

	string url(KRAKEN_HOST_PUBLIC);  
	url += "/Trades";

	string querystring("?pair=");
	querystring.append( symbol );
	url.append( querystring );

	// get trades only from id xxxxxx
	if ( id && id[0] )
    {
		url.append("&since=");
        url.append( id );
    }
		
	std::cout << "URL request: " << url << std::endl;

	string str_result;
	curl_api( url, str_result ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
		} catch ( exception &e ) {
		 	KrakCPP_logger::write_log( "<KrakCPP::get_recentTrades> Error ! %s", e.what() ); 
		}   
		KrakCPP_logger::write_log( "<KrakCPP::get_recentTrades> Done." ) ;
	
	} else {
		KrakCPP_logger::write_log( "<KrakCPP::get_recentTrades> Failed to get anything." ) ;
	}
}

// Get recent spread data
// URL: https://api.kraken.com/0/public/Spread
// pair = asset pair to get spread data for
// since = return spread data since given id (optional.  inclusive)
// https://api.kraken.com/0/public/Spread?pair=ADACAD&since=1557191323956868434
void 
KrakCPP::get_spread( const char* symbol, Json::Value &json_result, const char* id ) 
{
	KrakCPP_logger::write_log( "<KrakCPP::get_spread>" ) ;

	string url(KRAKEN_HOST_PUBLIC);  
	url += "/Spread";

	string querystring("?pair=");
	querystring.append( symbol );
	url.append( querystring );

	// get trades only from id xxxxxx
	if ( id && id[0] )
    {
		url.append("&since=");
        url.append( id );
    }
		
	std::cout << "URL request: " << url << std::endl;

	string str_result;
	curl_api( url, str_result ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
		} catch ( exception &e ) {
		 	KrakCPP_logger::write_log( "<KrakCPP::get_spread> Error ! %s", e.what() ); 
		}   
		KrakCPP_logger::write_log( "<KrakCPP::get_spread> Done." ) ;
	
	} else {
		KrakCPP_logger::write_log( "<KrakCPP::get_spread> Failed to get anything." ) ;
	}
}

//++++++++++++++++++++++++++++++++++++++++++++++++++
// Private API endpoints begins here
//++++++++++++++++++++++++++++++++++++++++++++++++++
// Get current account information. (SIGNED)
// URL: https://api.kraken.com/0/private/Balance
// Result: array of asset names and balance amount
void 
KrakCPP::get_balance( Json::Value &json_result ) 
{	

	KrakCPP_logger::write_log( "<KrakCPP::get_balances>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		KrakCPP_logger::write_log( "<KrakCPP::get_balance> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string signature_header;
	string nonce;
	string post_data;

	string url(KRAKEN_HOST), path;
	path = "/0/private/Balance";
	url += path;
	string action = "POST";

	nonce = to_string( get_current_ms_epoch() );
	
	post_data = "nonce=" + nonce;

	if ( !two_factor.empty() )
		post_data = post_data + "&" + "otp=" + two_factor;

	// HEADER:
	// API-Key = API key
	// API-Sign = Message signature using HMAC-SHA512 of (URI path + SHA256(nonce + POST data)) and base64 decoded secret API key
	// POST:
	// nonce = always increasing unsigned 64 bit integer
	// otp = two-factor password (if two-factor enabled, otherwise not required)
	signature_header = signature(path, nonce, post_data);

	// std::cout << "path: " << path << std::endl;
	// std::cout << "url: " << url << std::endl;
	// std::cout << "key: " << api_key << std::endl;
	// std::cout << "secret: " << secret_key.c_str() << std::endl;
	// std::cout << "post data: " << post_data << std::endl;
	// std::cout << "signature_header: " << signature_header << std::endl;

	vector <string> extra_http_header;
	string header_key("API-Key:"), header_sign("API-Sign:");
	header_key.append( api_key );
	header_sign.append( signature_header ); 
	extra_http_header.push_back(header_key);
	extra_http_header.push_back(header_sign);
	
	// for ( auto & it : extra_http_header )
	// 	std::cout << it << "\n";

	KrakCPP_logger::write_log( "<KrakCPP::get_balances> url = |%s|" , url.c_str() ) ;
	
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	KrakCPP_logger::write_log( "<KrakCPP::get_balance> Error ! %s", e.what() ); 
		}   
		KrakCPP_logger::write_log( "<KrakCPP::get_balance> Done." ) ;
	
	} else {
		KrakCPP_logger::write_log( "<KrakCPP::get_balance> Failed to get anything." ) ;
	}

	KrakCPP_logger::write_log( "<KrakCPP::get_balance> Done.\n" ) ;

}

// Get trade balance
// URL: https://api.kraken.com/0/private/TradeBalance
// aclass = asset class (optional): currency (default)
// asset = base asset used to determine balance (default = ZUSD)
void 
KrakCPP::get_tradeBalance( const char* symbol, Json::Value &json_result ) 
{	

	KrakCPP_logger::write_log( "<KrakCPP::get_tradeBalance>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		KrakCPP_logger::write_log( "<KrakCPP::get_tradeBalance> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string signature_header;
	string nonce;
	string post_data;

	string url(KRAKEN_HOST), path;
	path = "/0/private/TradeBalance";
	url += path;
	string action = "POST";

	if ( symbol && symbol[0] )
    {
		url.append("?asset=");
        url.append( symbol );
    }

	nonce = to_string( get_current_ms_epoch() );
	post_data = "nonce=" + nonce;

	if ( !two_factor.empty() )
		post_data = post_data + "&" + "otp=" + two_factor;

	// HEADER:
	// API-Key = API key
	// API-Sign = Message signature using HMAC-SHA512 of (URI path + SHA256(nonce + POST data)) and base64 decoded secret API key
	// POST:
	// nonce = always increasing unsigned 64 bit integer
	// otp = two-factor password (if two-factor enabled, otherwise not required)
	signature_header = signature(path, nonce, post_data);

	// std::cout << "path: " << path << std::endl;
	// std::cout << "url: " << url << std::endl;
	// std::cout << "key: " << api_key << std::endl;
	// std::cout << "secret: " << secret_key.c_str() << std::endl;
	// std::cout << "post data: " << post_data << std::endl;
	// std::cout << "signature_header: " << signature_header << std::endl;

	vector <string> extra_http_header;
	string header_key("API-Key:"), header_sign("API-Sign:");
	header_key.append( api_key );
	header_sign.append( signature_header ); 
	extra_http_header.push_back(header_key);
	extra_http_header.push_back(header_sign);
	
	KrakCPP_logger::write_log( "<KrakCPP::get_tradeBalance> url = |%s|" , url.c_str() ) ;
	
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	KrakCPP_logger::write_log( "<KrakCPP::get_tradeBalance> Error ! %s", e.what() ); 
		}   
		KrakCPP_logger::write_log( "<KrakCPP::get_tradeBalance> Done." ) ;
	
	} else {
		KrakCPP_logger::write_log( "<KrakCPP::get_tradeBalance> Failed to get anything." ) ;
	}

	KrakCPP_logger::write_log( "<KrakCPP::get_tradeBalance> Done.\n" ) ;

}


// Get open orders
// URL: https://api.kraken.com/0/private/OpenOrders
void 
KrakCPP::get_openOrders( Json::Value &json_result ) 
{	

	KrakCPP_logger::write_log( "<KrakCPP::get_openOrders>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		KrakCPP_logger::write_log( "<KrakCPP::get_openOrders> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string signature_header;
	string nonce;
	string post_data;

	string url(KRAKEN_HOST), path;
	path = "/0/private/OpenOrders";
	url += path;
	string action = "POST";

	nonce = to_string( get_current_ms_epoch() );
	
	post_data = "nonce=" + nonce;

	if ( !two_factor.empty() )
		post_data = post_data + "&" + "otp=" + two_factor;

	signature_header = signature(path, nonce, post_data);

	vector <string> extra_http_header;
	string header_key("API-Key:"), header_sign("API-Sign:");
	header_key.append( api_key );
	header_sign.append( signature_header ); 
	extra_http_header.push_back(header_key);
	extra_http_header.push_back(header_sign);
	
	// for ( auto & it : extra_http_header )
	// 	std::cout << it << "\n";

	KrakCPP_logger::write_log( "<KrakCPP::get_openOrders> url = |%s|" , url.c_str() ) ;
	
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	KrakCPP_logger::write_log( "<KrakCPP::get_openOrders> Error ! %s", e.what() ); 
		}   
		KrakCPP_logger::write_log( "<KrakCPP::get_openOrders> Done." ) ;
	
	} else {
		KrakCPP_logger::write_log( "<KrakCPP::get_openOrders> Failed to get anything." ) ;
	}

	KrakCPP_logger::write_log( "<KrakCPP::get_openOrders> Done.\n" ) ;
}


// Get closed orders
// URL: https://api.kraken.com/0/private/ClosedOrders
void 
KrakCPP::get_closedOrders( Json::Value &json_result ) 
{	

	KrakCPP_logger::write_log( "<KrakCPP::get_closedOrders>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		KrakCPP_logger::write_log( "<KrakCPP::get_closedOrders> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string signature_header;
	string nonce;
	string post_data;

	string url(KRAKEN_HOST), path;
	path = "/0/private/ClosedOrders";
	url += path;
	string action = "POST";

	nonce = to_string( get_current_ms_epoch() );
	
	post_data = "nonce=" + nonce;

	if ( !two_factor.empty() )
		post_data = post_data + "&" + "otp=" + two_factor;

	signature_header = signature(path, nonce, post_data);

	vector <string> extra_http_header;
	string header_key("API-Key:"), header_sign("API-Sign:");
	header_key.append( api_key );
	header_sign.append( signature_header ); 
	extra_http_header.push_back(header_key);
	extra_http_header.push_back(header_sign);
	
	// for ( auto & it : extra_http_header )
	// 	std::cout << it << "\n";

	KrakCPP_logger::write_log( "<KrakCPP::get_closedOrders> url = |%s|" , url.c_str() ) ;
	
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	KrakCPP_logger::write_log( "<KrakCPP::get_closedOrders> Error ! %s", e.what() ); 
		}   
		KrakCPP_logger::write_log( "<KrakCPP::get_closedOrders> Done." ) ;
	
	} else {
		KrakCPP_logger::write_log( "<KrakCPP::get_closedOrders> Failed to get anything." ) ;
	}

	KrakCPP_logger::write_log( "<KrakCPP::get_closedOrders> Done.\n" ) ;
}

// Query orders info
// URL: https://api.kraken.com/0/private/QueryOrders
void 
KrakCPP::query_orders( const char* orderId, Json::Value &json_result ) 
{	

	KrakCPP_logger::write_log( "<KrakCPP::query_orders>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		KrakCPP_logger::write_log( "<KrakCPP::query_orders> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string signature_header;
	string nonce;
	string post_data;

	string url(KRAKEN_HOST), path;
	path = "/0/private/QueryOrders";
	url += path;
	string action = "POST";

	nonce = to_string( get_current_ms_epoch() );
	
	post_data = "nonce=" + nonce;

	#pragma message "txid, userref, ctOrderId/orderId protocol review"
	if ( orderId && orderId[0] )
	{
		post_data.append("&txid=");
		post_data.append( orderId );
	}

	if ( !two_factor.empty() )
		post_data = post_data + "&" + "otp=" + two_factor;

	signature_header = signature(path, nonce, post_data);

	vector <string> extra_http_header;
	string header_key("API-Key:"), header_sign("API-Sign:");
	header_key.append( api_key );
	header_sign.append( signature_header ); 
	extra_http_header.push_back(header_key);
	extra_http_header.push_back(header_sign);
	
	// for ( auto & it : extra_http_header )
	// 	std::cout << it << "\n";

	KrakCPP_logger::write_log( "<KrakCPP::query_orders> url = |%s|" , url.c_str() ) ;
	
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	KrakCPP_logger::write_log( "<KrakCPP::query_orders> Error ! %s", e.what() ); 
		}   
		KrakCPP_logger::write_log( "<KrakCPP::query_orders> Done." ) ;
	
	} else {
		KrakCPP_logger::write_log( "<KrakCPP::query_orders> Failed to get anything." ) ;
	}

	KrakCPP_logger::write_log( "<KrakCPP::query_orders> Done.\n" ) ;
}

// Get trades history
// URL: https://api.kraken.com/0/private/TradesHistory
void 
KrakCPP::get_tradeHistory( Json::Value &json_result ) 
{	

	KrakCPP_logger::write_log( "<KrakCPP::get_tradeHistory>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		KrakCPP_logger::write_log( "<KrakCPP::get_tradeHistory> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string signature_header;
	string nonce;
	string post_data;

	string url(KRAKEN_HOST), path;
	path = "/0/private/TradesHistory";
	url += path;
	string action = "POST";

	nonce = to_string( get_current_ms_epoch() );
	
	post_data = "nonce=" + nonce;

	if ( !two_factor.empty() )
		post_data = post_data + "&" + "otp=" + two_factor;

	signature_header = signature(path, nonce, post_data);

	vector <string> extra_http_header;
	string header_key("API-Key:"), header_sign("API-Sign:");
	header_key.append( api_key );
	header_sign.append( signature_header ); 
	extra_http_header.push_back(header_key);
	extra_http_header.push_back(header_sign);
	
	// for ( auto & it : extra_http_header )
	// 	std::cout << it << "\n";

	KrakCPP_logger::write_log( "<KrakCPP::get_tradeHistory> url = |%s|" , url.c_str() ) ;
	
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	KrakCPP_logger::write_log( "<KrakCPP::get_tradeHistory> Error ! %s", e.what() ); 
		}   
		KrakCPP_logger::write_log( "<KrakCPP::get_tradeHistory> Done." ) ;
	
	} else {
		KrakCPP_logger::write_log( "<KrakCPP::get_tradeHistory> Failed to get anything." ) ;
	}

	KrakCPP_logger::write_log( "<KrakCPP::get_tradeHistory> Done.\n" ) ;
}

// Query trades info
// URL: https://api.kraken.com/0/private/QueryTrades
void 
KrakCPP::query_trades( Json::Value &json_result ) 
{	

	KrakCPP_logger::write_log( "<KrakCPP::query_trades>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		KrakCPP_logger::write_log( "<KrakCPP::query_trades> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string signature_header;
	string nonce;
	string post_data;

	string url(KRAKEN_HOST), path;
	path = "/0/private/QueryTrades";
	url += path;
	string action = "POST";

	nonce = to_string( get_current_ms_epoch() );
	
	post_data = "nonce=" + nonce;

	if ( !two_factor.empty() )
		post_data = post_data + "&" + "otp=" + two_factor;

	signature_header = signature(path, nonce, post_data);

	vector <string> extra_http_header;
	string header_key("API-Key:"), header_sign("API-Sign:");
	header_key.append( api_key );
	header_sign.append( signature_header ); 
	extra_http_header.push_back(header_key);
	extra_http_header.push_back(header_sign);
	
	// for ( auto & it : extra_http_header )
	// 	std::cout << it << "\n";

	KrakCPP_logger::write_log( "<KrakCPP::query_trades> url = |%s|" , url.c_str() ) ;
	
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	KrakCPP_logger::write_log( "<KrakCPP::query_trades> Error ! %s", e.what() ); 
		}   
		KrakCPP_logger::write_log( "<KrakCPP::query_trades> Done." ) ;
	
	} else {
		KrakCPP_logger::write_log( "<KrakCPP::query_trades> Failed to get anything." ) ;
	}

	KrakCPP_logger::write_log( "<KrakCPP::query_trades> Done.\n" ) ;
}

// Get open positions
// URL: https://api.kraken.com/0/private/OpenPositions
void 
KrakCPP::get_openPositions( Json::Value &json_result ) 
{	

	KrakCPP_logger::write_log( "<KrakCPP::get_openPositions>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		KrakCPP_logger::write_log( "<KrakCPP::get_openPositions> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string signature_header;
	string nonce;
	string post_data;

	string url(KRAKEN_HOST), path;
	path = "/0/private/OpenPositions";
	url += path;
	string action = "POST";

	nonce = to_string( get_current_ms_epoch() );
	
	post_data = "nonce=" + nonce;

	if ( !two_factor.empty() )
		post_data = post_data + "&" + "otp=" + two_factor;

	signature_header = signature(path, nonce, post_data);

	vector <string> extra_http_header;
	string header_key("API-Key:"), header_sign("API-Sign:");
	header_key.append( api_key );
	header_sign.append( signature_header ); 
	extra_http_header.push_back(header_key);
	extra_http_header.push_back(header_sign);
	
	// for ( auto & it : extra_http_header )
	// 	std::cout << it << "\n";

	KrakCPP_logger::write_log( "<KrakCPP::get_openPositions> url = |%s|" , url.c_str() ) ;
	
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	KrakCPP_logger::write_log( "<KrakCPP::get_openPositions> Error ! %s", e.what() ); 
		}   
		KrakCPP_logger::write_log( "<KrakCPP::get_openPositions> Done." ) ;
	
	} else {
		KrakCPP_logger::write_log( "<KrakCPP::get_openPositions> Failed to get anything." ) ;
	}

	KrakCPP_logger::write_log( "<KrakCPP::get_openPositions> Done.\n" ) ;
}


// Get ledgers info
// URL: https://api.kraken.com/0/private/Ledgers
void 
KrakCPP::get_ledgerInfo( Json::Value &json_result ) 
{	

	KrakCPP_logger::write_log( "<KrakCPP::get_ledgerInfo>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		KrakCPP_logger::write_log( "<KrakCPP::get_ledgerInfo> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string signature_header;
	string nonce;
	string post_data;

	string url(KRAKEN_HOST), path;
	path = "/0/private/Ledgers";
	url += path;
	string action = "POST";

	nonce = to_string( get_current_ms_epoch() );
	
	post_data = "nonce=" + nonce;

	if ( !two_factor.empty() )
		post_data = post_data + "&" + "otp=" + two_factor;

	signature_header = signature(path, nonce, post_data);

	vector <string> extra_http_header;
	string header_key("API-Key:"), header_sign("API-Sign:");
	header_key.append( api_key );
	header_sign.append( signature_header ); 
	extra_http_header.push_back(header_key);
	extra_http_header.push_back(header_sign);
	
	// for ( auto & it : extra_http_header )
	// 	std::cout << it << "\n";

	KrakCPP_logger::write_log( "<KrakCPP::get_ledgerInfo> url = |%s|" , url.c_str() ) ;
	
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	KrakCPP_logger::write_log( "<KrakCPP::get_ledgerInfo> Error ! %s", e.what() ); 
		}   
		KrakCPP_logger::write_log( "<KrakCPP::get_ledgerInfo> Done." ) ;
	
	} else {
		KrakCPP_logger::write_log( "<KrakCPP::get_ledgerInfo> Failed to get anything." ) ;
	}

	KrakCPP_logger::write_log( "<KrakCPP::get_ledgerInfo> Done.\n" ) ;
}

// Query ledgers
// URL: https://api.kraken.com/0/private/QueryLedgers
void 
KrakCPP::query_ledgers( Json::Value &json_result ) 
{	

	KrakCPP_logger::write_log( "<KrakCPP::query_ledgers>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		KrakCPP_logger::write_log( "<KrakCPP::query_ledgers> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string signature_header;
	string nonce;
	string post_data;

	string url(KRAKEN_HOST), path;
	path = "/0/private/QueryLedgers";
	url += path;
	string action = "POST";

	nonce = to_string( get_current_ms_epoch() );
	
	post_data = "nonce=" + nonce;

	if ( !two_factor.empty() )
		post_data = post_data + "&" + "otp=" + two_factor;

	signature_header = signature(path, nonce, post_data);

	vector <string> extra_http_header;
	string header_key("API-Key:"), header_sign("API-Sign:");
	header_key.append( api_key );
	header_sign.append( signature_header ); 
	extra_http_header.push_back(header_key);
	extra_http_header.push_back(header_sign);
	
	// for ( auto & it : extra_http_header )
	// 	std::cout << it << "\n";

	KrakCPP_logger::write_log( "<KrakCPP::query_ledgers> url = |%s|" , url.c_str() ) ;
	
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	KrakCPP_logger::write_log( "<KrakCPP::query_ledgers> Error ! %s", e.what() ); 
		}   
		KrakCPP_logger::write_log( "<KrakCPP::query_ledgers> Done." ) ;
	
	} else {
		KrakCPP_logger::write_log( "<KrakCPP::query_ledgers> Failed to get anything." ) ;
	}

	KrakCPP_logger::write_log( "<KrakCPP::query_ledgers> Done.\n" ) ;
}

// Get trade volume
// URL: https://api.kraken.com/0/private/TradeVolume
void 
KrakCPP::get_tradeVolume( Json::Value &json_result ) 
{	

	KrakCPP_logger::write_log( "<KrakCPP::get_tradeVolume>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		KrakCPP_logger::write_log( "<KrakCPP::get_tradeVolume> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string signature_header;
	string nonce;
	string post_data;

	string url(KRAKEN_HOST), path;
	path = "/0/private/TradeVolume";
	url += path;
	string action = "POST";

	nonce = to_string( get_current_ms_epoch() );
	
	post_data = "nonce=" + nonce;

	if ( !two_factor.empty() )
		post_data = post_data + "&" + "otp=" + two_factor;

	signature_header = signature(path, nonce, post_data);

	vector <string> extra_http_header;
	string header_key("API-Key:"), header_sign("API-Sign:");
	header_key.append( api_key );
	header_sign.append( signature_header ); 
	extra_http_header.push_back(header_key);
	extra_http_header.push_back(header_sign);
	
	// for ( auto & it : extra_http_header )
	// 	std::cout << it << "\n";

	KrakCPP_logger::write_log( "<KrakCPP::get_tradeVolume> url = |%s|" , url.c_str() ) ;
	
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	KrakCPP_logger::write_log( "<KrakCPP::get_tradeVolume> Error ! %s", e.what() ); 
		}   
		KrakCPP_logger::write_log( "<KrakCPP::get_tradeVolume> Done." ) ;
	
	} else {
		KrakCPP_logger::write_log( "<KrakCPP::get_tradeVolume> Failed to get anything." ) ;
	}

	KrakCPP_logger::write_log( "<KrakCPP::get_tradeVolume> Done.\n" ) ;
}


// Private authticated endpoints: place orders
// Place a market order
void 
KrakCPP::send_order_market(const char* symbol, const char* side, double quantity, const char* userref, Json::Value &json_result, const char* leverage ) 
{	
	KrakCPP_logger::write_log( "<KrakCPP::send_order_market>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		KrakCPP_logger::write_log( "<KrakCPP::send_order_market> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string signature_header;
	string nonce;
	string post_data;

	string url(KRAKEN_HOST), path;
	path = "/0/private/AddOrder";
	url += path;
	string action = "POST";

	nonce = to_string( get_current_ms_epoch() );
	
	post_data = "nonce=" + nonce;

	post_data.append("&pair=");
	post_data.append( symbol );

	post_data.append("&type=");
	post_data.append( side );

	post_data.append("&ordertype=market");

	post_data.append("&volume=");
	post_data.append( to_string(quantity) );

	if ( leverage && leverage[0] )
	{
		post_data.append("&leverage=");
		post_data.append( leverage );
	}

	if ( !two_factor.empty() )
		post_data = post_data + "&" + "otp=" + two_factor;

	signature_header = signature(path, nonce, post_data);

	vector <string> extra_http_header;
	string header_key("API-Key:"), header_sign("API-Sign:");
	header_key.append( api_key );
	header_sign.append( signature_header ); 
	extra_http_header.push_back(header_key);
	extra_http_header.push_back(header_sign);
	
	// for ( auto & it : extra_http_header )
	// 	std::cout << it << "\n";

	KrakCPP_logger::write_log( "<KrakCPP::send_order_market> url = |%s|" , url.c_str() ) ;
	
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	KrakCPP_logger::write_log( "<KrakCPP::send_order_market> Error ! %s", e.what() ); 
		}   
		KrakCPP_logger::write_log( "<KrakCPP::send_order_market> Done." ) ;
	
	} else {
		KrakCPP_logger::write_log( "<KrakCPP::send_order_market> Failed to get anything." ) ;
	}

	KrakCPP_logger::write_log( "<KrakCPP::send_order_market> Done.\n" ) ;
}

// Place a limit order
void 
KrakCPP::send_order_limit( const char* symbol, const char* side, double quantity, double price, const char* userref,Json::Value &json_result , const char* starttm, const char*  expiretm, const char* leverage ) 
{	

	KrakCPP_logger::write_log( "<KrakCPP::send_order_limit>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		KrakCPP_logger::write_log( "<KrakCPP::send_order_limit> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string signature_header;
	string nonce;
	string post_data;

	string url(KRAKEN_HOST), path;
	path = "/0/private/AddOrder";
	url += path;
	string action = "POST";

	nonce = to_string( get_current_ms_epoch() );
	
	post_data = "nonce=" + nonce;

	post_data.append("&pair=");
	post_data.append( symbol );

	post_data.append("&type=");
	post_data.append( side );

	post_data.append("&ordertype=limit");

	post_data.append("&price=");
	post_data.append( to_string(price) );

	post_data.append("&volume=");
	post_data.append( to_string(quantity) );

	if ( starttm && starttm[0] )
	{
		post_data.append("&starttm=");
		post_data.append( starttm );
	}

	if ( expiretm && expiretm[0] )
	{
		post_data.append("&expiretm=");
		post_data.append( expiretm );
	}

	if ( leverage && leverage[0] )
	{
		post_data.append("&leverage=");
		post_data.append( leverage );
	}

	if ( !two_factor.empty() )
		post_data = post_data + "&" + "otp=" + two_factor;

	signature_header = signature(path, nonce, post_data);

	vector <string> extra_http_header;
	string header_key("API-Key:"), header_sign("API-Sign:");
	header_key.append( api_key );
	header_sign.append( signature_header ); 
	extra_http_header.push_back(header_key);
	extra_http_header.push_back(header_sign);
	
	// for ( auto & it : extra_http_header )
	// 	std::cout << it << "\n";

	KrakCPP_logger::write_log( "<KrakCPP::send_order_limit> url = |%s|" , url.c_str() ) ;
	
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	KrakCPP_logger::write_log( "<KrakCPP::send_order_limit> Error ! %s", e.what() ); 
		}   
		KrakCPP_logger::write_log( "<KrakCPP::send_order_limit> Done." ) ;
	
	} else {
		KrakCPP_logger::write_log( "<KrakCPP::send_order_limit> Failed to get anything." ) ;
	}

	KrakCPP_logger::write_log( "<KrakCPP::send_order_limit> Done.\n" ) ;
}

// Cancel an order
void 
KrakCPP::cancel_order( const char* orderId, Json::Value &json_result ) 
{	

	KrakCPP_logger::write_log( "<KrakCPP::cancel_order>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		KrakCPP_logger::write_log( "<KrakCPP::cancel_order> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string signature_header;
	string nonce;
	string post_data;

	string url(KRAKEN_HOST), path;
	path = "/0/private/CancelOrder";
	url += path;
	string action = "POST";

	nonce = to_string( get_current_ms_epoch() );
	
	post_data = "nonce=" + nonce;

	if ( !two_factor.empty() )
		post_data = post_data + "&" + "otp=" + two_factor;

	post_data.append("&txid=");
	post_data.append( orderId );

	signature_header = signature(path, nonce, post_data);

	vector <string> extra_http_header;
	string header_key("API-Key:"), header_sign("API-Sign:");
	header_key.append( api_key );
	header_sign.append( signature_header ); 
	extra_http_header.push_back(header_key);
	extra_http_header.push_back(header_sign);
	
	// for ( auto & it : extra_http_header )
	// 	std::cout << it << "\n";

	KrakCPP_logger::write_log( "<KrakCPP::cancel_order> url = |%s|" , url.c_str() ) ;
	
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	KrakCPP_logger::write_log( "<KrakCPP::cancel_order> Error ! %s", e.what() ); 
		}   
		KrakCPP_logger::write_log( "<KrakCPP::cancel_order> Done." ) ;
	
	} else {
		KrakCPP_logger::write_log( "<KrakCPP::cancel_order> Failed to get anything." ) ;
	}

	KrakCPP_logger::write_log( "<KrakCPP::cancel_order> Done.\n" ) ;
}


//--------------------
//Keepalive user data stream (API-KEY)
void 
KrakCPP::keep_userDataStream( const char *listenKey ) 
{	
	KrakCPP_logger::write_log( "<KrakCPP::keep_userDataStream>" ) ;

	if ( api_key.size() == 0 ) {
		KrakCPP_logger::write_log( "<KrakCPP::keep_userDataStream> API Key has not been set." ) ;
		return ;
	}


	string url(KRAKEN_HOST_PRIVATE);
	url += "/api/v1/userDataStream";

	vector <string> extra_http_header;
	string header_chunk("X-MBX-APIKEY: ");
	

	header_chunk.append( api_key );
	extra_http_header.push_back(header_chunk);

	string action = "PUT";
	string post_data("listenKey=");
	post_data.append( listenKey );

	KrakCPP_logger::write_log( "<KrakCPP::keep_userDataStream> url = |%s|, post_data = |%s|" , url.c_str() , post_data.c_str() ) ;
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;

	if ( str_result.size() > 0 ) {
		
		KrakCPP_logger::write_log( "<KrakCPP::keep_userDataStream> Done." ) ;
	
	} else {
		KrakCPP_logger::write_log( "<KrakCPP::keep_userDataStream> Failed to get anything." ) ;
	}

	KrakCPP_logger::write_log( "<KrakCPP::keep_userDataStream> Done.\n" ) ;

}





//--------------------
//Keepalive user data stream (API-KEY)
void 
KrakCPP::close_userDataStream( const char *listenKey ) 
{	
	KrakCPP_logger::write_log( "<KrakCPP::close_userDataStream>" ) ;

	if ( api_key.size() == 0 ) {
		KrakCPP_logger::write_log( "<KrakCPP::close_userDataStream> API Key has not been set." ) ;
		return ;
	}


	string url(KRAKEN_HOST_PRIVATE);
	url += "/api/v1/userDataStream";

	vector <string> extra_http_header;
	string header_chunk("X-MBX-APIKEY: ");
	

	header_chunk.append( api_key );
	extra_http_header.push_back(header_chunk);

	string action = "DELETE";
	string post_data("listenKey=");
	post_data.append( listenKey );

	KrakCPP_logger::write_log( "<KrakCPP::close_userDataStream> url = |%s|, post_data = |%s|" , url.c_str() , post_data.c_str() ) ;
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;

	if ( str_result.size() > 0 ) {
		
		KrakCPP_logger::write_log( "<KrakCPP::close_userDataStream> Done." ) ;
	
	} else {
		KrakCPP_logger::write_log( "<KrakCPP::close_userDataStream> Failed to get anything." ) ;
	}

	KrakCPP_logger::write_log( "<KrakCPP::close_userDataStream> Done.\n" ) ;

}






//-----------------
// Curl's callback
size_t 
KrakCPP::curl_cb( void *content, size_t size, size_t nmemb, std::string *buffer ) 
{	
	KrakCPP_logger::write_log( "<KrakCPP::curl_cb> " ) ;

	buffer->append((char*)content, size*nmemb);

	KrakCPP_logger::write_log( "<KrakCPP::curl_cb> done" ) ;
	return size*nmemb;
}






//--------------------------------------------------
void 
KrakCPP::curl_api( string &url, string &result_json ) {
	vector <string> v;
	string action = "GET";
	string post_data = "";
	curl_api_with_header( url , result_json , v, post_data , action );	
} 

// Ray mods to include followlocation option
void
KrakCPP::curl_api_loc( string &url, string &result_json ) {
    vector <string> v;
    string action = "GET";
    string post_data = "";
    curl_api_with_header_loc( url , result_json , v, post_data , action );
}

//--------------------
// Do the curl
void 
KrakCPP::curl_api_with_header( string &url, string &str_result, vector <string> &extra_http_header , string &post_data , string &action ) 
{
	KrakCPP_logger::write_log( "<KrakCPP::curl_api>" ) ;

	CURLcode res;

	if( KrakCPP::curl ) {

		curl_easy_setopt(KrakCPP::curl, CURLOPT_URL, url.c_str() );
		curl_easy_setopt(KrakCPP::curl, CURLOPT_WRITEFUNCTION, KrakCPP::curl_cb);
		curl_easy_setopt(KrakCPP::curl, CURLOPT_WRITEDATA, &str_result );
		curl_easy_setopt(KrakCPP::curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(KrakCPP::curl, CURLOPT_ENCODING, "gzip");

		if ( extra_http_header.size() > 0 ) {
			
			struct curl_slist *chunk = NULL;
			for ( int i = 0 ; i < extra_http_header.size() ;i++ ) {
				chunk = curl_slist_append(chunk, extra_http_header[i].c_str() );
			}
			curl_easy_setopt(KrakCPP::curl, CURLOPT_HTTPHEADER, chunk);
		}

		if ( post_data.size() > 0 || action == "POST" || action == "PUT" || action == "DELETE" ) {

			if ( action == "PUT" || action == "DELETE" ) {
				curl_easy_setopt(KrakCPP::curl, CURLOPT_CUSTOMREQUEST, action.c_str() );
			}
			std::cout << "POST request: " << post_data.c_str() << std::endl;
			curl_easy_setopt(KrakCPP::curl, CURLOPT_POSTFIELDS, post_data.c_str() );
 		}

		res = curl_easy_perform(KrakCPP::curl);

		/* Check for errors */ 
		if ( res != CURLE_OK ) {
			KrakCPP_logger::write_log( "<KrakCPP::curl_api> curl_easy_perform() failed: %s" , curl_easy_strerror(res) ) ;
		} 	

	}

	KrakCPP_logger::write_log( "<KrakCPP::curl_api> done" ) ;

}

// Ray changes to include follow location
// // Do the curl
void
KrakCPP::curl_api_with_header_loc( string &url, string &str_result, vector <string> &extra_http_header , string &post_data , string &action )
{
    KrakCPP_logger::write_log( "<KrakCPP::curl_api>" ) ;

    CURLcode res;

    if( KrakCPP::curl ) {

        curl_easy_setopt(KrakCPP::curl, CURLOPT_URL, url.c_str() );
        curl_easy_setopt(KrakCPP::curl, CURLOPT_WRITEFUNCTION, KrakCPP::curl_cb);
        curl_easy_setopt(KrakCPP::curl, CURLOPT_WRITEDATA, &str_result );
        curl_easy_setopt(KrakCPP::curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(KrakCPP::curl, CURLOPT_ENCODING, "gzip");
		curl_easy_setopt(KrakCPP::curl, CURLOPT_FOLLOWLOCATION, 1L);

        if ( extra_http_header.size() > 0 ) {

            struct curl_slist *chunk = NULL;
            for ( int i = 0 ; i < extra_http_header.size() ;i++ ) {
                chunk = curl_slist_append(chunk, extra_http_header[i].c_str() );
            }
            curl_easy_setopt(KrakCPP::curl, CURLOPT_HTTPHEADER, chunk);
        }

        if ( post_data.size() > 0 || action == "POST" || action == "PUT" || action == "DELETE" ) {

            if ( action == "PUT" || action == "DELETE" ) {
                curl_easy_setopt(KrakCPP::curl, CURLOPT_CUSTOMREQUEST, action.c_str() );
            }
            curl_easy_setopt(KrakCPP::curl, CURLOPT_POSTFIELDS, post_data.c_str() );
        }

        res = curl_easy_perform(KrakCPP::curl);

        /* Check for errors */
        if ( res != CURLE_OK ) {
            KrakCPP_logger::write_log( "<KrakCPP::curl_api> curl_easy_perform() failed: %s" , curl_easy_strerror(res) ) ;
        }

    }

    KrakCPP_logger::write_log( "<KrakCPP::curl_api> done" ) ;

}

