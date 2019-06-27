/*
	Author: Ray Guang
	Date  : 2019/04/24
	
	C++ library for Coinbase API.
*/

#include "coincpp.h"
#include "coincpp_logger.h"
#include "coincpp_utils.h"
#include "coincpp_hmac.h"

#include <iostream>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "cryptopp/cryptlib.h"
using CryptoPP::Exception;

#include "cryptopp/hmac.h"

#include "cryptopp/sha.h"
//using CryptoPP::SHA256;

#include "cryptopp/base64.h"
using CryptoPP::Base64Decoder;
using CryptoPP::Base64Encoder;

#include "cryptopp/filters.h"
using CryptoPP::StringSink;
using CryptoPP::StringSource;
//using CryptoPP::HashFilter;

CoinCPP::CoinCPP()
	: curl(NULL)
{
}

CoinCPP::~CoinCPP()
{
	if (NULL != CoinCPP::curl)
		cleanup();
}

//---------------------------------
bool
CoinCPP::init( string &api_key, string &secret_key ) 
{	
	CoinCPP::api_key = api_key;
	CoinCPP::secret_key = secret_key;

	CoinCPP::curl = curl_easy_init();

	return NULL != CoinCPP::curl;
}

// Signature required: key, nonce, and user_id
// Constructor to include user_id
bool
CoinCPP::init( string &api_key, string &secret_key, string &passphrase ) 
{	
	CoinCPP::api_key = api_key;
	CoinCPP::secret_key = secret_key;
	CoinCPP::passphrase = passphrase;

	CoinCPP::curl = curl_easy_init();

	return NULL != CoinCPP::curl;
}

void
CoinCPP::cleanup()
{
	curl_easy_cleanup(CoinCPP::curl);	
}

std::string
CoinCPP::signature(const string& time_stamp, const string& method, const string& path, const string& body)
{
    string mac, encoded, secret;
	string plain = time_stamp + method + path + body;

	/*
	std::cout << "timestamp:\t" << time_stamp << std::endl;
    std::cout << "method:\t" << method << std::endl;
    std::cout << "path:\t" << path << std::endl;
	std::cout << "body:\t" << body << std::endl;
	std::cout << "plain:\t" << plain << std::endl;
	*/

	 StringSource(secret_key, true,
          new Base64Decoder(
              new StringSink(secret)));

	try
  	{
		CryptoPP::HMAC<CryptoPP::SHA256> hmac((unsigned char*)secret.c_str(), secret.length());
	  	StringSource(plain, true,
			  new CryptoPP::HashFilter(hmac,
				  new StringSink(mac)));
  	}
  	catch(const CryptoPP::Exception& e)
  	{
    	std::cerr << e.what() << std::endl;
  	}
  		StringSource(mac, true,
		  new Base64Encoder(
			  new StringSink(encoded)));
	encoded.erase(44, 1);
	
	// std::cout << "encoded(signature): " << encoded << std::endl;
	return encoded;
}

const vector<string>
CoinCPP::build_header()
{
	vector<string> extra_http_header;
	string header_chunk("Accept:");
    header_chunk.append( "application/json" );
    extra_http_header.push_back(header_chunk);

	header_chunk="User-Agent:";
	header_chunk.append("coinbase/cpp");
	extra_http_header.push_back(header_chunk);

	return extra_http_header;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++
// Public API endpoints begins here
//++++++++++++++++++++++++++++++++++++++++++++++++++
// Result: Server's time
// Note: This is to aid in approximating the skew time between the server and client.
void 
CoinCPP::get_serverTime( Json::Value &json_result ) 
{
	CoinCPP_logger::write_log( "<CoinCPP::get_serverTime>" ) ;

	string url(COINBASE_HOST);  
	url += "/time";
	string action = "GET";
	string post_data ="";

	std::cout << "URL request: " << url << std::endl;

	vector<string> extra_http_header = build_header();
	
	// Coinbase requires User-Agent header
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data  , action ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
		} catch ( exception &e ) {
		 	CoinCPP_logger::write_log( "<CoinCPP::get_serverTime> Error ! %s", e.what() ); 
		}   
		CoinCPP_logger::write_log( "<CoinCPP::get_serverTime> Done." ) ;
	
	} else {
		CoinCPP_logger::write_log( "<CoinCPP::get_serverTime> Failed to get anything." ) ;
	}
}

// Get Products
// Get a list of available currency pairs for trading.
// GET /products
void 
CoinCPP::get_products( Json::Value &json_result ) 
{
	CoinCPP_logger::write_log( "<CoinCPP::get_products>" ) ;

	string url(COINBASE_HOST);  
	url += "/products";
	string action = "GET";
	string post_data ="";
	string str_result;

	std::cout << "URL request: " << url << std::endl;

	vector<string> extra_http_header = build_header();
	curl_api_with_header( url, str_result , extra_http_header , post_data  , action ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
		} catch ( exception &e ) {
		 	CoinCPP_logger::write_log( "<CoinCPP::get_products> Error ! %s", e.what() ); 
		}   
		CoinCPP_logger::write_log( "<CoinCPP::get_products> Done." ) ;
	
	} else {
		CoinCPP_logger::write_log( "<CoinCPP::get_products> Failed to get anything." ) ;
	}
}

// Get Product Order Book
// Get a list of open orders for a product. The amount of detail shown can be customized with the level parameter.
// GET /products/<product-id>/book
void 
CoinCPP::get_productOrderBook( const char* symbol, Json::Value &json_result, string level ) 
{
	CoinCPP_logger::write_log( "<CoinCPP::get_productOrderBook>" ) ;

	string url(COINBASE_HOST);  
	url += "/products/";
	string action = "GET";
	string post_data ="";
	string str_result;

	string querystring ( symbol );
	url.append(querystring);
	url.append("/book?level=");
	url.append( level );

	std::cout << "URL request: " << url << std::endl;

	vector<string> extra_http_header = build_header();
	curl_api_with_header( url, str_result , extra_http_header , post_data  , action ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
		} catch ( exception &e ) {
		 	CoinCPP_logger::write_log( "<CoinCPP::get_productOrderBook> Error ! %s", e.what() ); 
		}   
		CoinCPP_logger::write_log( "<CoinCPP::get_productOrderBook> Done." ) ;
	
	} else {
		CoinCPP_logger::write_log( "<CoinCPP::get_productOrderBook> Failed to get anything." ) ;
	}
}

// Get Product Ticker
// Snapshot information about the last trade (tick), best bid/ask and 24h volume.
// GET /products/<product-id>/ticker
void 
CoinCPP::get_productTicker( const char* symbol, Json::Value &json_result ) 
{
	CoinCPP_logger::write_log( "<CoinCPP::get_productTicker>" ) ;

	string url(COINBASE_HOST);  
	url += "/products/";
	string action = "GET";
	string post_data ="";
	string str_result;

	string querystring ( symbol );
	url.append(querystring);
	url.append("/ticker");

	std::cout << "URL request: " << url << std::endl;

	vector<string> extra_http_header = build_header();
	curl_api_with_header( url, str_result , extra_http_header , post_data  , action ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
		} catch ( exception &e ) {
		 	CoinCPP_logger::write_log( "<CoinCPP::get_productTicker> Error ! %s", e.what() ); 
		}   
		CoinCPP_logger::write_log( "<CoinCPP::get_productTicker> Done." ) ;
	
	} else {
		CoinCPP_logger::write_log( "<CoinCPP::get_productTicker> Failed to get anything." ) ;
	}
}

// Get Trades
// List the latest trades for a product.
// GET /products/<product-id>/trades
void 
CoinCPP::get_productTrades( const char* symbol, Json::Value &json_result ) 
{
	CoinCPP_logger::write_log( "<CoinCPP::get_productTrades>" ) ;

	string url(COINBASE_HOST);  
	url += "/products/";
	string action = "GET";
	string post_data ="";
	string str_result;

	string querystring ( symbol );
	url.append(querystring);
	url.append("/trades");

	std::cout << "URL request: " << url << std::endl;

	vector<string> extra_http_header = build_header();
	curl_api_with_header( url, str_result , extra_http_header , post_data  , action ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
		} catch ( exception &e ) {
		 	CoinCPP_logger::write_log( "<CoinCPP::get_productTrades> Error ! %s", e.what() ); 
		}   
		CoinCPP_logger::write_log( "<CoinCPP::get_productTrades> Done." ) ;
	
	} else {
		CoinCPP_logger::write_log( "<CoinCPP::get_productTrades> Failed to get anything." ) ;
	}
}

// Get currencies
// List known currencies.
// GET /currencies
void 
CoinCPP::get_currencies( Json::Value &json_result ) 
{
	CoinCPP_logger::write_log( "<CoinCPP::get_currencies>" ) ;

	string url(COINBASE_HOST);  
	url += "/currencies";
	string action = "GET";
	string post_data ="";
	string str_result;

	std::cout << "URL request: " << url << std::endl;

	vector<string> extra_http_header = build_header();
	curl_api_with_header( url, str_result , extra_http_header , post_data  , action ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
		} catch ( exception &e ) {
		 	CoinCPP_logger::write_log( "<CoinCPP::get_currencies> Error ! %s", e.what() ); 
		}   
		CoinCPP_logger::write_log( "<CoinCPP::get_currencies> Done." ) ;
	
	} else {
		CoinCPP_logger::write_log( "<CoinCPP::get_currencies> Failed to get anything." ) ;
	}
}


//++++++++++++++++++++++++++++++++++++++++++++++++++
// Private API endpoints begins here
//++++++++++++++++++++++++++++++++++++++++++++++++++
// Get current account information. (SIGNED)
// URL: https://api.kraken.com/0/private/Balance
// Result: array of asset names and balance amount
void 
CoinCPP::get_listAccounts( Json::Value &json_result ) 
{	

	CoinCPP_logger::write_log( "<CoinCPP::get_listAccounts>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		CoinCPP_logger::write_log( "<CoinCPP::get_listAccounts> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string signature_header;
	string nonce;
	string post_data;

	string url(COINBASE_HOST), path;
	path = "/accounts";
	url += path;
	string action = "GET";

	nonce = get_timestamp();
	
	post_data = "";

	// All REST requests must contain the following headers:

	// CB-ACCESS-KEY The api key as a string.
	// CB-ACCESS-SIGN The base64-encoded signature (see Signing a Message).
	// CB-ACCESS-TIMESTAMP A timestamp for your request.
	// CB-ACCESS-PASSPHRASE The passphrase you specified when creating the API key.
	// All request bodies should have content type application/json and be valid JSON.
	signature_header = signature( nonce, action, path, post_data );

	// std::cout << "path: " << path << std::endl;
	// std::cout << "url: " << url << std::endl;
	// std::cout << "key: " << api_key << std::endl;
	// std::cout << "secret: " << secret_key.c_str() << std::endl;
	// std::cout << "passphrase: " << passphrase << std::endl;
	// std::cout << "post data: " << post_data << std::endl;
	// std::cout << "signature_header: " << signature_header << std::endl;

	// Build signed HTTP header
	vector <string> extra_http_header;
	string header_key("CB-ACCESS-KEY: "), header_sign("CB-ACCESS-SIGN: "), header_timestamp( "CB-ACCESS-TIMESTAMP: " ), header_passphrase ( "CB-ACCESS-PASSPHRASE: " );
	header_key.append( api_key );
	header_sign.append( signature_header ); 
	header_timestamp.append( nonce );
	header_passphrase.append( passphrase );
	extra_http_header.push_back( header_key );
	extra_http_header.push_back( header_sign );
	extra_http_header.push_back( header_timestamp );
	extra_http_header.push_back ( header_passphrase );

	string header_chunk("Accept:");
    header_chunk.append( "application/json" );
    extra_http_header.push_back(header_chunk);

	header_chunk="User-Agent:";
	header_chunk.append("coinbase/cpp");
	extra_http_header.push_back(header_chunk);
	
	//  for ( auto & it : extra_http_header )
	//  	std::cout << it << "\n";

	CoinCPP_logger::write_log( "<CoinCPP::get_listAccounts> url = |%s|" , url.c_str() ) ;
	
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	CoinCPP_logger::write_log( "<CoinCPP::get_listAccounts> Error ! %s", e.what() ); 
		}   
		CoinCPP_logger::write_log( "<CoinCPP::get_listAccounts> Done." ) ;
	
	} else {
		CoinCPP_logger::write_log( "<CoinCPP::get_listAccounts> Failed to get anything." ) ;
	}

	CoinCPP_logger::write_log( "<CoinCPP::get_listAccounts> Done.\n" ) ;

}

// Get an Account
// Information for a single account. Use this endpoint when you know the account_id.
// GET /accounts/<account-id>
void 
CoinCPP::get_account( const char* account_id, Json::Value &json_result ) 
{	

	CoinCPP_logger::write_log( "<CoinCPP::get_account>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		CoinCPP_logger::write_log( "<CoinCPP::get_account> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string signature_header;
	string nonce;
	string post_data;

	string url(COINBASE_HOST), path;
	path = "/accounts";
	
	string action = "GET";

	nonce = get_timestamp();
	
	path.append("/");
	path.append( account_id );

	url+= path;

	post_data = "";
	
	signature_header = signature( nonce, action, path, post_data );

	// Build signed HTTP header
	vector <string> extra_http_header;
	string header_key("CB-ACCESS-KEY: "), header_sign("CB-ACCESS-SIGN: "), header_timestamp( "CB-ACCESS-TIMESTAMP: " ), header_passphrase ( "CB-ACCESS-PASSPHRASE: " );
	header_key.append( api_key );
	header_sign.append( signature_header ); 
	header_timestamp.append( nonce );
	header_passphrase.append( passphrase );
	extra_http_header.push_back( header_key );
	extra_http_header.push_back( header_sign );
	extra_http_header.push_back( header_timestamp );
	extra_http_header.push_back ( header_passphrase );

	string header_chunk("Accept:");
    header_chunk.append( "application/json" );
    extra_http_header.push_back(header_chunk);

	header_chunk="User-Agent:";
	header_chunk.append("coinbase/cpp");
	extra_http_header.push_back(header_chunk);
	
	//  for ( auto & it : extra_http_header )
	//  	std::cout << it << "\n";

	CoinCPP_logger::write_log( "<CoinCPP::get_account> url = |%s|" , url.c_str() ) ;
	
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	CoinCPP_logger::write_log( "<CoinCPP::get_account> Error ! %s", e.what() ); 
		}   
		CoinCPP_logger::write_log( "<CoinCPP::get_account> Done." ) ;
	
	} else {
		CoinCPP_logger::write_log( "<CoinCPP::get_account> Failed to get anything." ) ;
	}

	CoinCPP_logger::write_log( "<CoinCPP::get_account> Done.\n" ) ;

}

// Get Account History
// List account activity. Account activity either increases or decreases your account balance. Items are paginated and sorted latest first. See the Pagination section for retrieving additional entries after the first page.
// GET /accounts/<account-id>/ledger
void 
CoinCPP::get_accountHistory( const char* account_id, Json::Value &json_result ) 
{	

	CoinCPP_logger::write_log( "<CoinCPP::get_accountHistory>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		CoinCPP_logger::write_log( "<CoinCPP::get_accountHistory> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string signature_header;
	string nonce;
	string post_data;

	string url(COINBASE_HOST), path;
	path = "/accounts";
	string action = "GET";

	nonce = get_timestamp();
	
	post_data = "";
	
	path.append( "/" );
	path.append( account_id );
	path.append( "/ledger" );
	
	url += path;

	signature_header = signature( nonce, action, path, post_data );

	// Build signed HTTP header
	vector <string> extra_http_header;
	string header_key("CB-ACCESS-KEY: "), header_sign("CB-ACCESS-SIGN: "), header_timestamp( "CB-ACCESS-TIMESTAMP: " ), header_passphrase ( "CB-ACCESS-PASSPHRASE: " );
	header_key.append( api_key );
	header_sign.append( signature_header ); 
	header_timestamp.append( nonce );
	header_passphrase.append( passphrase );
	extra_http_header.push_back( header_key );
	extra_http_header.push_back( header_sign );
	extra_http_header.push_back( header_timestamp );
	extra_http_header.push_back ( header_passphrase );

	string header_chunk("Accept:");
    header_chunk.append( "application/json" );
    extra_http_header.push_back(header_chunk);

	header_chunk="User-Agent:";
	header_chunk.append("coinbase/cpp");
	extra_http_header.push_back(header_chunk);
	
	//  for ( auto & it : extra_http_header )
	//  	std::cout << it << "\n";

	CoinCPP_logger::write_log( "<CoinCPP::get_accountHistory> url = |%s|" , url.c_str() ) ;
	
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	CoinCPP_logger::write_log( "<CoinCPP::get_accountHistory> Error ! %s", e.what() ); 
		}   
		CoinCPP_logger::write_log( "<CoinCPP::get_accountHistory> Done." ) ;
	
	} else {
		CoinCPP_logger::write_log( "<CoinCPP::get_accountHistory> Failed to get anything." ) ;
	}

	CoinCPP_logger::write_log( "<CoinCPP::get_accountHistory> Done.\n" ) ;

}

// Get Holds
// Holds are placed on an account for any active orders or pending withdraw requests. As an order is filled, the hold amount is updated. If an order is canceled, any remaining hold is removed. For a withdraw, once it is completed, the hold is removed.
// GET /accounts/<account_id>/holds
void 
CoinCPP::get_holds( const char* account_id, Json::Value &json_result ) 
{	

	CoinCPP_logger::write_log( "<CoinCPP::get_holds>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		CoinCPP_logger::write_log( "<CoinCPP::get_holds> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string signature_header;
	string nonce;
	string post_data;

	string url(COINBASE_HOST), path;
	path = "/accounts";
	string action = "GET";

	nonce = get_timestamp();
	
	post_data = "";
	
	path.append( "/" );
	path.append( account_id );
	path.append( "/holds" );

	url += path;

	signature_header = signature( nonce, action, path, post_data );

	// Build signed HTTP header
	vector <string> extra_http_header;
	string header_key("CB-ACCESS-KEY: "), header_sign("CB-ACCESS-SIGN: "), 
						header_timestamp( "CB-ACCESS-TIMESTAMP: " ), header_passphrase ( "CB-ACCESS-PASSPHRASE: " );
	header_key.append( api_key );
	header_sign.append( signature_header ); 
	header_timestamp.append( nonce );
	header_passphrase.append( passphrase );
	extra_http_header.push_back( header_key );
	extra_http_header.push_back( header_sign );
	extra_http_header.push_back( header_timestamp );
	extra_http_header.push_back ( header_passphrase );

	string header_chunk("Accept:");
    header_chunk.append( "application/json" );
    extra_http_header.push_back(header_chunk);

	header_chunk="User-Agent:";
	header_chunk.append("coinbase/cpp");
	extra_http_header.push_back(header_chunk);
	
	//  for ( auto & it : extra_http_header )
	//  	std::cout << it << "\n";

	CoinCPP_logger::write_log( "<CoinCPP::get_holds> url = |%s|" , url.c_str() ) ;
	
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	CoinCPP_logger::write_log( "<CoinCPP::get_holds> Error ! %s", e.what() ); 
		}   
		CoinCPP_logger::write_log( "<CoinCPP::get_holds> Done." ) ;
	
	} else {
		CoinCPP_logger::write_log( "<CoinCPP::get_holds> Failed to get anything." ) ;
	}

	CoinCPP_logger::write_log( "<CoinCPP::get_holds> Done.\n" ) ;

}

// Privated authenticated endpoints: orders

// Place a market Order
void 
CoinCPP::send_order_market( const char* symbol, const char* side, double quantity, const char* client_oid, Json::Value &json_result ) 
{	

	CoinCPP_logger::write_log( "<CoinCPP::send_order_market>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		CoinCPP_logger::write_log( "<CoinCPP::send_order_market> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string signature_header;
	string nonce;
	string post_data;

	string url(COINBASE_HOST), path;
	path = "/orders";
	url += path;
	string action = "POST";

	nonce = get_timestamp();
	
	// Build post data
	using namespace rapidjson;
	Document d;
	d.SetObject();
	rapidjson::Document::AllocatorType& allocator = d.GetAllocator();

	d.AddMember("type", "market", allocator);

	Value v_side;
	v_side = StringRef(side);
	d.AddMember("side", v_side, allocator);

	Value v_product_id;
	v_product_id = StringRef(symbol);
	d.AddMember("product_id", v_product_id, allocator);

	#pragma message "convert quantity double to const char"
	Value v_quantity;
	string size = to_string(quantity);
	v_quantity = StringRef(size.c_str());
	d.AddMember("size", v_quantity, allocator);

	StringBuffer strbuf;
	Writer<StringBuffer> writer(strbuf);
	d.Accept(writer);
	
	post_data.append(strbuf.GetString());
	std::cout << "Coinbase: send_order_market : " << strbuf.GetString() << std::endl;
	// End building post data

	signature_header = signature( nonce, action, path, post_data );

	// Build signed HTTP header
	vector <string> extra_http_header;
	string header_key("CB-ACCESS-KEY: "), header_sign("CB-ACCESS-SIGN: "), header_timestamp( "CB-ACCESS-TIMESTAMP: " ), header_passphrase ( "CB-ACCESS-PASSPHRASE: " );
	header_key.append( api_key );
	header_sign.append( signature_header ); 
	header_timestamp.append( nonce );
	header_passphrase.append( passphrase );
	extra_http_header.push_back( header_key );
	extra_http_header.push_back( header_sign );
	extra_http_header.push_back( header_timestamp );
	extra_http_header.push_back ( header_passphrase );

	//string header_chunk("Accept:");
	string header_chunk("Content-Type: ");
    header_chunk.append( "application/json" );
    extra_http_header.push_back(header_chunk);

	header_chunk="User-Agent:";
	header_chunk.append("libcurl/1.0");
	extra_http_header.push_back(header_chunk);
	
	//  for ( auto & it : extra_http_header )
	//  	std::cout << it << "\n";

	CoinCPP_logger::write_log( "<CoinCPP::send_order_market> url = |%s|" , url.c_str() ) ;
	
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	CoinCPP_logger::write_log( "<CoinCPP::send_order_market> Error ! %s", e.what() ); 
		}   
		CoinCPP_logger::write_log( "<CoinCPP::send_order_market> Done." ) ;
	
	} else {
		CoinCPP_logger::write_log( "<CoinCPP::send_order_market> Failed to get anything." ) ;
	}

	CoinCPP_logger::write_log( "<CoinCPP::send_order_market> Done.\n" ) ;

}


// Place a limit order
void 
CoinCPP::send_order_limit( const char* symbol, const char* side, double quantity, double price, const char* timeInForce, const char* client_oid, Json::Value &json_result ) 
{	

	CoinCPP_logger::write_log( "<CoinCPP::send_order_limit>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		CoinCPP_logger::write_log( "<CoinCPP::send_order_limit> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string signature_header;
	string nonce;
	string post_data;

	string url(COINBASE_HOST), path;
	path = "/orders";
	url += path;
	string action = "POST";

	nonce = get_timestamp();
	
	// Build post data
	using namespace rapidjson;
	Document d;
	d.SetObject();
	rapidjson::Document::AllocatorType& allocator = d.GetAllocator();

	d.AddMember("type", "limit", allocator);

	Value v_side;
	v_side = StringRef(side);
	d.AddMember("side", v_side, allocator);

	Value v_product_id;
	v_product_id = StringRef(symbol);
	d.AddMember("product_id", v_product_id, allocator);

	Value v_price;
	string p = to_string(price);
	v_price = StringRef(p.c_str());
	d.AddMember("price", v_price, allocator);

	#pragma message "convert quantity double to const char"
    Value v_quantity;
    string size = to_string(quantity);
    v_quantity = StringRef(size.c_str());
    d.AddMember("size", v_quantity, allocator);

	d.AddMember("post_only", true, allocator);

	StringBuffer strbuf;
	Writer<StringBuffer> writer(strbuf);
	d.Accept(writer);
	
	post_data.append(strbuf.GetString());
	// End building post data
	signature_header = signature( nonce, action, path, post_data );

	// Build signed HTTP header
	vector <string> extra_http_header;
	string header_key("CB-ACCESS-KEY: "), header_sign("CB-ACCESS-SIGN: "), header_timestamp( "CB-ACCESS-TIMESTAMP: " ), header_passphrase ( "CB-ACCESS-PASSPHRASE: " );
	header_key.append( api_key );
	header_sign.append( signature_header ); 
	header_timestamp.append( nonce );
	header_passphrase.append( passphrase );
	extra_http_header.push_back( header_key );
	extra_http_header.push_back( header_sign );
	extra_http_header.push_back( header_timestamp );
	extra_http_header.push_back ( header_passphrase );

	//string header_chunk("Accept:");
	string header_chunk("Content-Type: ");
    header_chunk.append( "application/json" );
    extra_http_header.push_back(header_chunk);

	header_chunk="User-Agent:";
	header_chunk.append("libcurl/1.0");
	extra_http_header.push_back(header_chunk);
	
	//  for ( auto & it : extra_http_header )
	//  	std::cout << it << "\n";

	CoinCPP_logger::write_log( "<CoinCPP::send_order_limit> url = |%s|" , url.c_str() ) ;
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	CoinCPP_logger::write_log( "<CoinCPP::send_order_limit> Error ! %s", e.what() ); 
		}   
		CoinCPP_logger::write_log( "<CoinCPP::send_order_limit> Done." ) ;
	
	} else {
		CoinCPP_logger::write_log( "<CoinCPP::send_order_limit> Failed to get anything." ) ;
	}

	CoinCPP_logger::write_log( "<CoinCPP::send_order_limit> Done.\n" ) ;

}


// Cancel an order
void 
CoinCPP::cancel_oneOrder( const char* orderId, Json::Value &json_result ) 
{	

	CoinCPP_logger::write_log( "<CoinCPP::cancel_oneOrder>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		CoinCPP_logger::write_log( "<CoinCPP::cancel_oneOrder> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string signature_header;
	string nonce;
	string post_data;

	string url(COINBASE_HOST), path;
	path = "/orders";
	string action = "DELETE";

	nonce = get_timestamp();

	path.append("/");
	path.append( orderId );
	
	url += path;

	post_data = "";

	signature_header = signature( nonce, action, path, post_data );

	// Build signed HTTP header
	vector <string> extra_http_header;
	string header_key("CB-ACCESS-KEY: "), header_sign("CB-ACCESS-SIGN: "), header_timestamp( "CB-ACCESS-TIMESTAMP: " ), header_passphrase ( "CB-ACCESS-PASSPHRASE: " );
	header_key.append( api_key );
	header_sign.append( signature_header ); 
	header_timestamp.append( nonce );
	header_passphrase.append( passphrase );
	extra_http_header.push_back( header_key );
	extra_http_header.push_back( header_sign );
	extra_http_header.push_back( header_timestamp );
	extra_http_header.push_back ( header_passphrase );

	//string header_chunk("Accept:");
	string header_chunk("Content-Type: ");
    header_chunk.append( "application/json" );
    extra_http_header.push_back(header_chunk);

	header_chunk="User-Agent:";
	header_chunk.append("coinbase/cpp");
	extra_http_header.push_back(header_chunk);
	
	//  for ( auto & it : extra_http_header )
	//  	std::cout << it << "\n";

	CoinCPP_logger::write_log( "<CoinCPP::cancel_oneOrder> url = |%s|" , url.c_str() ) ;
	
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	CoinCPP_logger::write_log( "<CoinCPP::cancel_oneOrder> Error ! %s", e.what() ); 
		}   
		CoinCPP_logger::write_log( "<CoinCPP::cancel_oneOrder> Done." ) ;
	
	} else {
		CoinCPP_logger::write_log( "<CoinCPP::cancel_oneOrder> Failed to get anything." ) ;
	}

	CoinCPP_logger::write_log( "<CoinCPP::cancel_oneOrder> Done.\n" ) ;

}

// Cancel all orders
void 
CoinCPP::cancel_allOrders( const char* symbol, Json::Value &json_result ) 
{	

	CoinCPP_logger::write_log( "<CoinCPP::cancel_allOrders>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		CoinCPP_logger::write_log( "<CoinCPP::cancel_allOrders> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string signature_header;
	string nonce;
	string post_data;

	string url(COINBASE_HOST), path;
	path = "/orders";
	url += path;
	string action = "DELETE";

	nonce = get_timestamp();

	//url.append("?product_id=");
	//url.append( symbol );
	using namespace rapidjson;
	Document d;
	d.SetObject();
	rapidjson::Document::AllocatorType& allocator = d.GetAllocator();

	Value v_product_id;
	v_product_id = StringRef(symbol);
	d.AddMember("product_id", v_product_id, allocator);

	StringBuffer strbuf;
	Writer<StringBuffer> writer(strbuf);
	d.Accept(writer);

	post_data.append(strbuf.GetString());
	
	signature_header = signature( nonce, action, path, post_data );

	// Build signed HTTP header
	vector <string> extra_http_header;
	string header_key("CB-ACCESS-KEY: "), header_sign("CB-ACCESS-SIGN: "), header_timestamp( "CB-ACCESS-TIMESTAMP: " ), header_passphrase ( "CB-ACCESS-PASSPHRASE: " );
	header_key.append( api_key );
	header_sign.append( signature_header ); 
	header_timestamp.append( nonce );
	header_passphrase.append( passphrase );
	extra_http_header.push_back( header_key );
	extra_http_header.push_back( header_sign );
	extra_http_header.push_back( header_timestamp );
	extra_http_header.push_back ( header_passphrase );

	//string header_chunk("Accept:");
	string header_chunk("Content-Type: ");
    header_chunk.append( "application/json" );
    extra_http_header.push_back(header_chunk);

	header_chunk="User-Agent:";
	header_chunk.append("coinbase/cpp");
	extra_http_header.push_back(header_chunk);
	
	//  for ( auto & it : extra_http_header )
	//  	std::cout << it << "\n";

	CoinCPP_logger::write_log( "<CoinCPP::cancel_allOrders> url = |%s|" , url.c_str() ) ;
	
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	CoinCPP_logger::write_log( "<CoinCPP::cancel_allOrders> Error ! %s", e.what() ); 
		}   
		CoinCPP_logger::write_log( "<CoinCPP::cancel_allOrders> Done." ) ;
	
	} else {
		CoinCPP_logger::write_log( "<CoinCPP::cancel_allOrders> Failed to get anything." ) ;
	}

	CoinCPP_logger::write_log( "<CoinCPP::cancel_allOrders> Done.\n" ) ;

}

// List Orders
// List your current open orders. Only open or un-settled orders are returned. As soon as an order is no longer open and settled, it will no longer appear in the default request.
// GET /orders
void 
CoinCPP::get_allOrders( const char* symbol, const char* status, Json::Value &json_result ) 
{	

	CoinCPP_logger::write_log( "<CoinCPP::get_allOrders>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		CoinCPP_logger::write_log( "<CoinCPP::get_allOrders> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string signature_header;
	string nonce;
	string post_data;

	string url(COINBASE_HOST), path;
	path = "/orders";
	string action = "GET";

	nonce = get_timestamp();

	if ( status[0] == '\0' && status != NULL ){
		// const char* is empty
		path.append("?status=done&status=settled");
	} else {
		path.append("?status=");
		path.append( status );
	}

	if ( symbol[0] == '\0' && symbol != NULL ){
		
	} else {
		path.append("&product_id=");
		path.append( symbol );
	}
	
	url += path;

	
	signature_header = signature( nonce, action, path, post_data );

	// Build signed HTTP header
	vector <string> extra_http_header;
	string header_key("CB-ACCESS-KEY: "), header_sign("CB-ACCESS-SIGN: "), header_timestamp( "CB-ACCESS-TIMESTAMP: " ), header_passphrase ( "CB-ACCESS-PASSPHRASE: " );
	header_key.append( api_key );
	header_sign.append( signature_header ); 
	header_timestamp.append( nonce );
	header_passphrase.append( passphrase );
	extra_http_header.push_back( header_key );
	extra_http_header.push_back( header_sign );
	extra_http_header.push_back( header_timestamp );
	extra_http_header.push_back ( header_passphrase );

	string header_chunk("Content-Type: ");
    header_chunk.append( "application/json" );
    extra_http_header.push_back(header_chunk);

	header_chunk="User-Agent:";
	header_chunk.append("coinbase/cpp");
	extra_http_header.push_back(header_chunk);
	
	//  for ( auto & it : extra_http_header )
	//  	std::cout << it << "\n";

	CoinCPP_logger::write_log( "<CoinCPP::get_oneOrder> url = |%s|" , url.c_str() ) ;
	
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	CoinCPP_logger::write_log( "<CoinCPP::get_allOrders> Error ! %s", e.what() ); 
		}   
		CoinCPP_logger::write_log( "<CoinCPP::get_allOrders> Done." ) ;
	
	} else {
		CoinCPP_logger::write_log( "<CoinCPP::get_allOrders> Failed to get anything." ) ;
	}

	CoinCPP_logger::write_log( "<CoinCPP::get_allOrders> Done.\n" ) ;

}

// Get an Order
// Get a single order by order id.
// GET /orders/<order-id>
void 
CoinCPP::get_oneOrder( const char* orderId, Json::Value &json_result ) 
{	

	CoinCPP_logger::write_log( "<CoinCPP::get_oneOrder>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		CoinCPP_logger::write_log( "<CoinCPP::get_oneOrder> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string signature_header;
	string nonce;
	string post_data;

	string url(COINBASE_HOST), path;
	path = "/orders";
	string action = "GET";

	nonce = get_timestamp();

	path.append("/");
	path.append( orderId );
	
	url += path;

	
	signature_header = signature( nonce, action, path, post_data );

	// Build signed HTTP header
	vector <string> extra_http_header;
	string header_key("CB-ACCESS-KEY: "), header_sign("CB-ACCESS-SIGN: "), header_timestamp( "CB-ACCESS-TIMESTAMP: " ), header_passphrase ( "CB-ACCESS-PASSPHRASE: " );
	header_key.append( api_key );
	header_sign.append( signature_header ); 
	header_timestamp.append( nonce );
	header_passphrase.append( passphrase );
	extra_http_header.push_back( header_key );
	extra_http_header.push_back( header_sign );
	extra_http_header.push_back( header_timestamp );
	extra_http_header.push_back ( header_passphrase );

	string header_chunk("Content-Type: ");
    header_chunk.append( "application/json" );
    extra_http_header.push_back(header_chunk);

	header_chunk="User-Agent:";
	header_chunk.append("coinbase/cpp");
	extra_http_header.push_back(header_chunk);
	
	//  for ( auto & it : extra_http_header )
	//  	std::cout << it << "\n";

	CoinCPP_logger::write_log( "<CoinCPP::get_oneOrder> url = |%s|" , url.c_str() ) ;
	
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	CoinCPP_logger::write_log( "<CoinCPP::get_oneOrder> Error ! %s", e.what() ); 
		}   
		CoinCPP_logger::write_log( "<CoinCPP::get_oneOrder> Done." ) ;
	
	} else {
		CoinCPP_logger::write_log( "<CoinCPP::get_oneOrder> Failed to get anything." ) ;
	}

	CoinCPP_logger::write_log( "<CoinCPP::get_oneOrder> Done.\n" ) ;

}


// List Fills
// Get a list of recent fills.
// GET /fills
void 
CoinCPP::get_fills( const char* symbol, Json::Value &json_result ) 
{	
	CoinCPP_logger::write_log( "<CoinCPP::get_fills>" ) ;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		CoinCPP_logger::write_log( "<CoinCPP::get_fills> API Key and Secret Key has not been set." ) ;
		return ;
	}

	string signature_header;
	string nonce;
	string post_data;

	string url(COINBASE_HOST), path;
	path = "/fills";
	string action = "GET";

	nonce = get_timestamp();

	/*if ( orderId && orderId[0] )
	{
		url.append("?order_id=");
		url.append( orderId );
	}*/ 
	
	if ( symbol && symbol[0] )
	{
		path.append("?product_id=");
		path.append( symbol );
	} else 
	{
		path.append("?product_id=all");
	}
	
	url += path;

	signature_header = signature( nonce, action, path, post_data );

	// Build signed HTTP header
	vector <string> extra_http_header;
	string header_key("CB-ACCESS-KEY: "), header_sign("CB-ACCESS-SIGN: "), header_timestamp( "CB-ACCESS-TIMESTAMP: " ), header_passphrase ( "CB-ACCESS-PASSPHRASE: " );
	header_key.append( api_key );
	header_sign.append( signature_header ); 
	header_timestamp.append( nonce );
	header_passphrase.append( passphrase );
	extra_http_header.push_back( header_key );
	extra_http_header.push_back( header_sign );
	extra_http_header.push_back( header_timestamp );
	extra_http_header.push_back ( header_passphrase );

	//string header_chunk("Accept:");
	string header_chunk("Content-Type: ");
    header_chunk.append( "application/json" );
    extra_http_header.push_back(header_chunk);

	header_chunk="User-Agent:";
	header_chunk.append("coinbase/cpp");
	extra_http_header.push_back(header_chunk);
	
	//  for ( auto & it : extra_http_header )
	//  	std::cout << it << "\n";

	CoinCPP_logger::write_log( "<CoinCPP::get_fills> url = |%s|" , url.c_str() ) ;
	
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	CoinCPP_logger::write_log( "<CoinCPP::get_fills> Error ! %s", e.what() ); 
		}   
		CoinCPP_logger::write_log( "<CoinCPP::get_fills> Done." ) ;
	
	} else {
		CoinCPP_logger::write_log( "<CoinCPP::get_fills> Failed to get anything." ) ;
	}

	CoinCPP_logger::write_log( "<CoinCPP::get_fills> Done.\n" ) ;

}


//--------------------
//Keepalive user data stream (API-KEY)
void 
CoinCPP::keep_userDataStream( const char *listenKey ) 
{	
	CoinCPP_logger::write_log( "<CoinCPP::keep_userDataStream>" ) ;

	if ( api_key.size() == 0 ) {
		CoinCPP_logger::write_log( "<CoinCPP::keep_userDataStream> API Key has not been set." ) ;
		return ;
	}


	string url(COINBASE_HOST);
	url += "/api/v1/userDataStream";

	vector <string> extra_http_header;
	string header_chunk("X-MBX-APIKEY: ");
	

	header_chunk.append( api_key );
	extra_http_header.push_back(header_chunk);

	string action = "PUT";
	string post_data("listenKey=");
	post_data.append( listenKey );

	CoinCPP_logger::write_log( "<CoinCPP::keep_userDataStream> url = |%s|, post_data = |%s|" , url.c_str() , post_data.c_str() ) ;
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;

	if ( str_result.size() > 0 ) {
		
		CoinCPP_logger::write_log( "<CoinCPP::keep_userDataStream> Done." ) ;
	
	} else {
		CoinCPP_logger::write_log( "<CoinCPP::keep_userDataStream> Failed to get anything." ) ;
	}

	CoinCPP_logger::write_log( "<CoinCPP::keep_userDataStream> Done.\n" ) ;

}





//--------------------
//Keepalive user data stream (API-KEY)
void 
CoinCPP::close_userDataStream( const char *listenKey ) 
{	
	CoinCPP_logger::write_log( "<CoinCPP::close_userDataStream>" ) ;

	if ( api_key.size() == 0 ) {
		CoinCPP_logger::write_log( "<CoinCPP::close_userDataStream> API Key has not been set." ) ;
		return ;
	}


	string url(COINBASE_HOST);
	url += "/api/v1/userDataStream";

	vector <string> extra_http_header;
	string header_chunk("X-MBX-APIKEY: ");
	

	header_chunk.append( api_key );
	extra_http_header.push_back(header_chunk);

	string action = "DELETE";
	string post_data("listenKey=");
	post_data.append( listenKey );

	CoinCPP_logger::write_log( "<CoinCPP::close_userDataStream> url = |%s|, post_data = |%s|" , url.c_str() , post_data.c_str() ) ;
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;

	if ( str_result.size() > 0 ) {
		
		CoinCPP_logger::write_log( "<CoinCPP::close_userDataStream> Done." ) ;
	
	} else {
		CoinCPP_logger::write_log( "<CoinCPP::close_userDataStream> Failed to get anything." ) ;
	}

	CoinCPP_logger::write_log( "<CoinCPP::close_userDataStream> Done.\n" ) ;

}






//-----------------
// Curl's callback
size_t 
CoinCPP::curl_cb( void *content, size_t size, size_t nmemb, std::string *buffer ) 
{	
	CoinCPP_logger::write_log( "<CoinCPP::curl_cb> " ) ;

	buffer->append((char*)content, size*nmemb);

	CoinCPP_logger::write_log( "<CoinCPP::curl_cb> done" ) ;
	return size*nmemb;
}






//--------------------------------------------------
void 
CoinCPP::curl_api( string &url, string &result_json ) {
	vector <string> v;
	string action = "GET";
	string post_data = "";
	curl_api_with_header( url , result_json , v, post_data , action );	
} 

// Ray mods to include followlocation option
void
CoinCPP::curl_api_loc( string &url, string &result_json ) {
    vector <string> v;
    string action = "GET";
    string post_data = "";
    curl_api_with_header_loc( url , result_json , v, post_data , action );
}

//--------------------
// Do the curl
void 
CoinCPP::curl_api_with_header( string &url, string &str_result, vector <string> &extra_http_header , string &post_data , string &action ) 
{
	CoinCPP_logger::write_log( "<CoinCPP::curl_api>" ) ;

	CURLcode res;

	if( CoinCPP::curl ) {

		curl_easy_setopt(CoinCPP::curl, CURLOPT_URL, url.c_str() );
		curl_easy_setopt(CoinCPP::curl, CURLOPT_WRITEFUNCTION, CoinCPP::curl_cb);
		curl_easy_setopt(CoinCPP::curl, CURLOPT_WRITEDATA, &str_result );
		curl_easy_setopt(CoinCPP::curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(CoinCPP::curl, CURLOPT_ENCODING, "gzip");

		if ( extra_http_header.size() > 0 ) {
			
			struct curl_slist *chunk = NULL;
			for ( int i = 0 ; i < extra_http_header.size() ;i++ ) {
				chunk = curl_slist_append(chunk, extra_http_header[i].c_str() );
			}
			curl_easy_setopt(CoinCPP::curl, CURLOPT_HTTPHEADER, chunk);
		}

		if ( post_data.size() > 0 || action == "POST" || action == "PUT" || action == "DELETE" ) {

			if ( action == "PUT" || action == "DELETE" ) {
				curl_easy_setopt(CoinCPP::curl, CURLOPT_CUSTOMREQUEST, action.c_str() );
			}
			std::cout << "POST request: " << post_data.c_str() << std::endl;
			curl_easy_setopt(CoinCPP::curl, CURLOPT_POSTFIELDS, post_data.c_str() );
 		}

		res = curl_easy_perform(CoinCPP::curl);

		/* Check for errors */ 
		if ( res != CURLE_OK ) {
			CoinCPP_logger::write_log( "<CoinCPP::curl_api> curl_easy_perform() failed: %s" , curl_easy_strerror(res) ) ;
		} 	

	}

	CoinCPP_logger::write_log( "<CoinCPP::curl_api> done" ) ;

}

// Ray changes to include follow location
// // Do the curl
void
CoinCPP::curl_api_with_header_loc( string &url, string &str_result, vector <string> &extra_http_header , string &post_data , string &action )
{
    CoinCPP_logger::write_log( "<CoinCPP::curl_api>" ) ;

    CURLcode res;

    if( CoinCPP::curl ) {

        curl_easy_setopt(CoinCPP::curl, CURLOPT_URL, url.c_str() );
        curl_easy_setopt(CoinCPP::curl, CURLOPT_WRITEFUNCTION, CoinCPP::curl_cb);
        curl_easy_setopt(CoinCPP::curl, CURLOPT_WRITEDATA, &str_result );
        curl_easy_setopt(CoinCPP::curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(CoinCPP::curl, CURLOPT_ENCODING, "gzip");
		curl_easy_setopt(CoinCPP::curl, CURLOPT_FOLLOWLOCATION, 1L);

        if ( extra_http_header.size() > 0 ) {

            struct curl_slist *chunk = NULL;
            for ( int i = 0 ; i < extra_http_header.size() ;i++ ) {
                chunk = curl_slist_append(chunk, extra_http_header[i].c_str() );
            }
            curl_easy_setopt(CoinCPP::curl, CURLOPT_HTTPHEADER, chunk);
        }

        if ( post_data.size() > 0 || action == "POST" || action == "PUT" || action == "DELETE" ) {

            if ( action == "PUT" || action == "DELETE" ) {
                curl_easy_setopt(CoinCPP::curl, CURLOPT_CUSTOMREQUEST, action.c_str() );
            }
            curl_easy_setopt(CoinCPP::curl, CURLOPT_POSTFIELDS, post_data.c_str() );
        }

        res = curl_easy_perform(CoinCPP::curl);

        /* Check for errors */
        if ( res != CURLE_OK ) {
            CoinCPP_logger::write_log( "<CoinCPP::curl_api> curl_easy_perform() failed: %s" , curl_easy_strerror(res) ) ;
        }

    }

    CoinCPP_logger::write_log( "<CoinCPP::curl_api> done" ) ;

}

