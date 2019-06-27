#include "binance.h"
#include <iostream>
#include <tuple>

// Binance API library
#include "binacpp.h"
#include "binacpp_websocket.h"
#include <json/json.h>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "dbconnector.h"

using namespace Trading::Binance;

Engine::Engine()
{
}

/******************************************************************************/
std::string BUY::process(const std::string& request)
{
    std::cout << "Binance Request is: " << request << std::endl;
	
	const char* request_json = request.c_str();
	rapidjson::Document document;
	document.Parse(request_json);
	std::move(document);
	
    // Route trade message based on RequestType
    int userID                      = document["UserID"].GetInt();
    const char* exchangeName        = document["Exchange"].GetString();
	const char* requestType         = document["RequestType"].GetString();
    std::cout << "UserID:" << userID << " Exchange:" << exchangeName << " request: " <<requestType << std::endl;

    // Parse request 
	std::pair<std::string, std::string> api_pair;

    // Retrieve API key and secrect from database
    api_pair = getAPIkey(userID, exchangeName);
	std::string api_key 	= std::get<0>(api_pair);
	std::string api_secret	= std::get<1>(api_pair);
	
    BinaCPP::init(api_key, api_secret);
    long recvWindow = 10000;
    

    if ( !std::strcmp(requestType, "send_order") ){
        const char* symbol              = document["Symbol"].GetString();
        const char* orderSide           = document["Side"].GetString();         // Buy or Sell
        const char* orderType           = document["OrderType"].GetString();    // Market or Limit
        const char* timeInForce         = document["TimeInForce"].GetString();  // GTC, IOC, FOK
        double quantity                 = document["Quantity"].GetDouble();    
        double price                    = document["Price"].GetDouble();
        std::cout << "[send_order] " << exchangeName << ' '  << symbol << ' '  << orderSide << ' ' << orderType << ' ' << timeInForce << ' ' << quantity << ' ' << price << ' ' << std::endl;
		
		Json::Value result;

        if ( !std::strcmp(orderType, "LIMIT") ) {
            // Place MARKET and LIMIT order. There is no difference between market and limit except the price for market is ""
			BinaCPP::send_order( symbol, orderSide, orderType, timeInForce, quantity , price,"",0,0, recvWindow, result );
            std::cout << result << std::endl;

            std::string clientOrderId   = result["clientOrderId"].asString();
            int orderId                 = result["orderId"].asInt();
            std::string orderStatus     = result["status"].asString();

            #pragma message "Save order reply to DB."
            std::stringstream orderReply;
            orderReply << "{\"clientOrderId\":\"" << clientOrderId << "\"," << "\"orderId\":" << orderId << "}";
            return orderReply.str();
        }
        else if ( !std::strcmp(orderType,"MARKET") ) {
            BinaCPP::send_order_market( symbol, orderSide, orderType, quantity , "",0,0, recvWindow, result );
            std::cout << result << std::endl;

            std::string clientOrderId   = result["clientOrderId"].asString();
            int orderId                 = result["orderId"].asInt();
            std::string status          = result["status"].asString();

            std::stringstream orderReply;
            orderReply << "{\"clientOrderId\":\"" << clientOrderId << "\"," << "\"orderId\":" << orderId << "}";
            return orderReply.str();

        }
    } else if ( !std::strcmp(requestType, "cancel_order") ){
        const char* symbol              = document["Symbol"].GetString();
        long orderId                    = document["OrderId"].GetInt();
 		Json::Value cancel_order_result;       

		std::cout << "[cancel_order]" << "Symbol: " << symbol << "OrderId: " <<orderId << std::endl;

        BinaCPP::cancel_order(symbol, orderId,"","", recvWindow, cancel_order_result);
		std::cout << cancel_order_result << std::endl;
		return "cancel_order completed successfully!";
	} else if ( !std::strcmp(requestType, "get_order")) {
        const char* symbol              = document["Symbol"].GetString();
        long orderId                    = document["OrderId"].GetInt();
        std::cout << "[get_order]" << "Symbol: " << symbol << "OrderId: " <<orderId << std::endl;
		Json::Value get_order_result;
        BinaCPP::get_order(symbol, orderId, "", recvWindow, get_order_result);
		std::cout << get_order_result << std::endl;
		return "get_order complted successfully!";
    } else {
		std::cout << "Message request_type is UNKNOWN!" << std::endl;
		return "requestType UNKNOWN.";
	}

    // Test Case #1: Get server time
    // BinaCPP::get_serverTime(result);

    //return std::string("{\"RequestID\":\"123456789\",\"Answer\":\"OK\"}");
}
