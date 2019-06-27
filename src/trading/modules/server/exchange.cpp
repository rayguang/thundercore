#include "exchange.h"
#include "mySqlStore.h"

#include <json/json.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <crypto++/cryptlib.h>
#include <crypto++/osrng.h>
#include <crypto++/modes.h>
#include <crypto++/aes.h>
#include <crypto++/filters.h>
#include <crypto++/hex.h>
#include <crypto++/base64.h>
#include <crypto++/sha3.h>

#define SEPARATOR           "::"

/******************************************************************************/
using namespace Trading;

std::string getJSONStr(rapidjson::Document& document, const char* memberName)
{
    std::string memberValue;
    
    try
    {
        if (document.HasMember(memberName)) {
            if (document[memberName].IsString()){
                memberValue = document[memberName].GetString();
            } else {
                std::cout << "[JSON Parsing ERROR]: " << memberName << std::endl;
                return "parsing_error";
            }
        } 
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return memberValue; 
}

unsigned int getJSONUint(rapidjson::Document& document, const char* memberName)
{
    unsigned int memberValue;

    try
    {
        if (document.HasMember(memberName)) {
            if (document[memberName].IsUint()){
                memberValue = document[memberName].GetUint();
            } else {
                std::cout << "[JSON Parsing ERROR]: " << memberName << std::endl;
                return 0;
            }
        }    
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return memberValue;
}

long long int getJSONLLInt(rapidjson::Document& document, const char* memberName)
{
    long long int memberValue;

    try
    {
        if (document.HasMember(memberName)) {
            if (document[memberName].IsUint64()){
                memberValue = document[memberName].GetUint64();
            } else {
                std::cout << "[JSON Parsing ERROR]: " << memberName << std::endl;
                return 0;
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return memberValue; 
}

double getJSONDouble(rapidjson::Document& document, const char* memberName)
{
    double memberValue;

    try
    {
        if (document.HasMember(memberName)) {
            if (document[memberName].GetDouble()){
                memberValue = document[memberName].GetDouble();
            } else {
                std::cout << "[JSON Parsing ERROR]: " << memberName << std::endl;
                return 0;
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return memberValue; 
}

ParseRequest::ParseRequest(const std::string& request)
    :   userID(0),
        quantity(0),
        price(0),
		stopPrice(0),
		limitPrice(0),
		icebergQty(0),
        orderId("")    
{
    std::cout << "[REQUEST]: " << request << std::endl;

    // prepare to parse JSON
    const char* request_json = request.c_str();
	rapidjson::Document document;
	try{
		document.Parse(request_json);	
	} catch (const std::exception&) {
		std::cout << "Request is REJECTED: invalid parsing." << std::endl;
		return;
	}

    // parse JSON    
    ctRequestId 	= getJSONStr(document,      "ctRequestId"); // Unique ID to identify crypto terminal order request
    requestType 	= getJSONStr(document,      "requestType"); // send_order, cancel_order etc
    userID      	= getJSONUint(document,     "uid");     // Unique ID to identify user    
    exchange    	= getJSONStr(document,      "exchange");    // Exchange name
    symbol      	= getJSONStr(document,      "symbol");      // Security symbol
    orderSide   	= getJSONStr(document,      "side");        // Buy or Sell
    subType     	= getJSONStr(document,      "subType");     // Route request based on Exchange::Type::subType
    quantity    	= getJSONDouble(document,   "quantity");    // Order quantity    
    icebergQty  	= getJSONDouble(document,   "icebergQty");  // Iceberg quantity
    price       	= getJSONDouble(document,   "price");       // Security price
    stopPrice   	= getJSONDouble(document,   "stopPrice");   // Stop price
    limitPrice  	= getJSONDouble(document,   "limitPrice");  // Limit price, Bitstamp required parameter
    timeInForce 	= getJSONStr(document,      "timeInForce"); // GTC, IOC, FOK; Bitstamp DAY (daily_order, cancel at UTC 00:00 if not filled)
    limit       	= getJSONUint(document,     "limit");      // Number of orders to return, default 500, max 1000
    clientOrderId 	= getJSONStr(document,	    "clientOrderId"); // Clientside orderID, e.g., binance, kraken
	orderId     	= getJSONStr(document,      "orderId");     // Numeric orderID
    fromId      	= getJSONStr(document,  	"fromId");      // TradeId to fetch from, default gets most recent trades
	type	    	= getJSONStr(document,		"type");	    // Market, Limit, Stop/Stop Limit etc, depending on exchanges
    positionId  	= getJSONLLInt(document,    "positionId");  // Bitfinex positionId in marginfudning and positions
    currency    	= getJSONStr(document,      "currency");    // Bitfinex currency for past fund, withdraw, deposit
    walletaddr  	= getJSONStr(document,      "address");     // Wallet address, e.g., Bittrex
	twoFactor		= getJSONStr(document,		"twoFactor");   // Exchange specific requirement, e.g., Kraken
    startTime   	= getJSONStr(document,      "startTime");   // Exchange specific, Kraken
    expireTime  	= getJSONStr(document,      "expireTime");  // Exchange specific, Kraken
    leverage    	= getJSONStr(document,      "leverage");    // Exchange specific, Kraken
    accountId   	= getJSONStr(document,      "accountId");   // Exchange specific, Coinbase
    orderStatus 	= getJSONStr(document,      "orderStatus"); // Exchange specific, Coinbase
	bookLevel 		= getJSONStr(document, 		"bookLevel");	// Exchange specific, Coinbase
    fromtm          = getJSONStr(document,      "fromtm");      //from timestamp to timestamp, e.g., Bitfinex trade history
    totm            = getJSONStr(document,      "totm");
    std::cout << "JSON parsed: " << ctRequestId << ' ' << requestType << ' ' << userID << ' ' << exchange << ' ' << symbol << ' ' << orderSide << ' ' << subType << ' ' << timeInForce << ' ' << quantity << ' ' << price << ' ' << std::endl;
}

std::string ParseRequest::classFromRequest() const
{
    std::string className(exchange + SEPARATOR + requestType + SEPARATOR + subType);
    std::transform(className.begin(), className.end(), className.begin(), ::tolower);                                

    std::cout << "Route request to: " << className << std::endl;

    return className;
}

bool Exchange::init(const ParseRequest& parseRequest)
{
    // get/set user info
	util::MySqlStore mySqlStore;
    mySqlStore.fetchInfo(parseRequest.userID, parseRequest.exchange.c_str());

    key = mySqlStore.getAPIkey();
    // secret = mySqlStore.getAPIsecret();
    try
    {
        using namespace CryptoPP;
        mySqlStore.fetchInfo(parseRequest.userID, parseRequest.exchange.c_str());

        std::string cipher, encoded, decoded, recovered, secret_decoded;
        key = mySqlStore.getAPIkey(); 
        secret = mySqlStore.getAPIsecret();

        //byte b_key[AES::DEFAULT_KEYLENGTH];
        byte _iv[AES::BLOCKSIZE];

        // Get 钥匙
		/* SHA256 hash;
		std::string digest;
		StringSource s(key, true, new HashFilter(hash, new HexEncoder(new StringSink(digest))));
		std::cout << "Hash digest: " << digest << std::endl;
        prng.GenerateBlock( temp_key, temp_key.size() ); */
		//std::cout << "key : " << key << std::endl;
        SecByteBlock _key((const byte*)key.data(), 16); // key size 32 bytes
		

        // Retrieve iv and secret 
        secret_decoded.clear();
        StringSource(secret, true,
            new Base64Decoder(
                new StringSink(secret_decoded)
            )
        );
		
		// Get 初始化向量 
		encoded.clear();
		std::strcpy ((char *)_iv, secret_decoded.substr(0,16).c_str()); // convert string to byte
		StringSource(_iv, sizeof(_iv), true,
        new Base64Encoder(
            new StringSink(encoded)
            ) // Base64Encoder
        ); // StringSource
        //std::cout << "[iv(pretty)]: " << encoded << std::endl;

		// Get 秘密
		try
		{
			CBC_Mode< AES >::Decryption d;
    		d.SetKeyWithIV(_key , 16, _iv );

    		// The StreamTransformationFilter removes
    		//  padding as required.
			// std::cout << "******** Decrypt Message ********" << std::endl;
			// std::cout << "[secret(encrypted)]: " << secret_decoded.substr(16).c_str() << std::endl;
			cipher = secret_decoded.substr(16);
			//cipher.erase(std::remove(cipher.begin(), cipher.end(), '\0'), cipher.end());
			StringSource ss( cipher, true,
				new StreamTransformationFilter( d,
            		new StringSink( recovered )
        		) // StreamTransformationFilter
    		); // StringSource

			//std::cout << std::endl << "[recovered text]: " << recovered << std::endl;
		}
		catch ( const CryptoPP::Exception& e)
		{
			std::cerr << e.what() << std::endl;
		}

        secret = recovered;
        //std::cout << "[decrypted text]: " << secret << std::endl;


    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return true;
}

