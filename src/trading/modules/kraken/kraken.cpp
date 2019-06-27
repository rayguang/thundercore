#include "kraken.h"
#include "mySqlStore.h"

#include <krakcpp_websocket.h>

#include <json/json.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <Poco/Bugcheck.h>

#include <iostream>
#include <algorithm>
#include <string>
#include <boost/algorithm/string.hpp>

/******************************************************************************/
using namespace Trading::Kraken;

Engine::Engine()
    :   recvWindow(10000)
{
}

bool Engine::init(const ParseRequest& parseRequest)
{
    // get / set user info
	/*util::MySqlStore mySqlStore;
    try 
	{	
    	mySqlStore.fetchInfo(parseRequest.userID, parseRequest.exchange.c_str());
	}
	catch (Poco::Exception& ex)
	{
		std::cerr << ex.displayText() << std::endl;
			
	}
	
    BinaCPP::init(mySqlStore.getAPIkey(), mySqlStore.getAPIsecret());*/
	std::string tf = parseRequest.twoFactor;
	if ( tf == "")
    	return Exchange::init(parseRequest) && krakCPP.init( key, secret );
	else 
		return Exchange::init(parseRequest) && krakCPP.init( key, secret, tf );
}

#define ORDER_ID "orderId"
#define CLIENTORDER_ID "clientOrderId"
// Return JSON message received from server without further processing.
std::string Engine::postAnyProcessing(const std::string& ctRequestId)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

    root["ctRequestId"] = ctRequestId;
	root["reply"] = exchangeAnswer;

    return fastWriter.write(root);
}

std::string Engine::postGetTickerProcessing(const std::string& ctRequestId, const std::string& symbol)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

    element["symbol"]   = symbol;
    element["price"]    = exchangeAnswer["result"][symbol]["c"][0];
    root["ctRequestId"] = ctRequestId;
	root["reply"]       = element;

    std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);
}

std::string Engine::postGetBalanceProcessing(const std::string& ctRequestId, const int uid)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;
	int i = 0;
	
	for (Json::Value::iterator it =  exchangeAnswer["result"].begin(); it != exchangeAnswer["result"].end(); ++it){
		Json::Value value 	= (*it);
		element[i]["Symbol"]	= it.key();
		element[i]["Free"]		= (*it);
		element[i]["Locked"]	= 0;
        element[i]["Total"]     = (*it);

        util::MySqlStore mySqlStore;
		// std::cout << "[DB - INSERT ]" << uid << ' ' << "KRAKEN" << ' ' << element[i]["Symbol"].asString() << ' ' << element[i]["Total"].asString() << std::endl ;
        mySqlStore.updateAccountBalance(uid, "KRAKEN", element[i]["Symbol"].asString(), std::atof(element[i]["Total"].asString().c_str()));
		
		i++;
	}

    root["ctRequestId"] = ctRequestId;
	root["balances"] = element;

    std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);
}

std::string Engine::postGetOrderStatusProcessing(const std::string& ctRequestId, const std::string& orderId)
{
    Json::FastWriter fastWriter;
    Json::Value root;
	Json::Value element;

	std::string type, side;

	side = exchangeAnswer["result"][orderId]["descr"]["type"].asString();
	type = exchangeAnswer["result"][orderId]["descr"]["ordertype"].asString();
	boost::to_upper(side);
	boost::to_upper(type);
	
	element["symbol"]       = exchangeAnswer["result"][orderId]["descr"]["pair"];
    element["orderId"]      = orderId;
    element["status"]       = exchangeAnswer["result"][orderId]["status"];
    element["price"]        = exchangeAnswer["result"][orderId]["descr"]["price"];
    // element["stopPrice"]	= exchangeAnswer["stopPrice"];
	element["origQty"]      = exchangeAnswer["result"][orderId]["vol"];
    element["side"]         = side;
    element["type"]         = type;
    element["ctOrderId"]    = exchangeAnswer["result"][orderId]["userref"];
	element["time"]			= exchangeAnswer["result"][orderId]["opentm"];

    root["ctRequestId"] = ctRequestId;
	root["reply"] = element;
    std::cout << "[REPLY: postGetOrderStatusProcessing]: " << root << std::endl;
    return fastWriter.write(root);
}

std::string Engine::postGetOpenOrdersProcessing(const std::string& ctRequestId)
{
    Json::FastWriter fastWriter;
    Json::Value root;
	Json::Value element;
    Json::Value kraken_root;

	std::string type, side;

	int i(0);

    kraken_root = exchangeAnswer["result"]["open"];
    for ( Json::Value::const_iterator itr = kraken_root.begin(); itr != kraken_root.end(); ++itr){

		type = (*itr)["descr"]["ordertype"].asString();
		side = (*itr)["descr"]["type"].asString(); 
		boost::to_upper(type);
		boost::to_upper(side);

        element[i]["orderId"]   = itr.key();
        element[i]["origQty"]   = (*itr)["vol"];
		element[i]["price"]		= (*itr)["descr"]["price"];
		element[i]["side"]		= side;
		element[i]["status"]	= (*itr)["status"];
		element[i]["symbol"]	= (*itr)["descr"]["pair"];
		element[i]["type"]		= type;
		element[i]["time"]		= (*itr)["opentm"].asDouble();
		i++;
    }

	root["reply"] = element;
	root["ctRequestId"] = ctRequestId;
    std::cout << "[REPLY: postGetOpenOrdersProcessing]: " << root << std::endl;

    return fastWriter.write(root);
}


std::string Engine::postSendOrderProcessing(const std::string& ctRequestId)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

    element["orderId"]  	= exchangeAnswer["result"]["txid"];
    element["msg"] 			= exchangeAnswer["result"]["descr"];

    root["ctRequestId"] = ctRequestId;
	root["reply"] 		= element;

	if (exchangeAnswer.isMember("error")){
		root["msg"]         = exchangeAnswer["error"][0];
	}

    std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);
}


std::string Engine::postCancelOneOrderProcessing(const std::string& ctRequestId, const std::string& orderId)
{
    Json::FastWriter fastWriter;
	Json::Value root;
    Json::Value element;
	
    element["orderId"]       = orderId;
    if (exchangeAnswer["result"]["count"].asInt() == 1) {
        element["status"]    = "CANCELLED";
    } else {
        element["status"]    = "CANCEL FAILED";
    }
    
	
	root["reply"] = element;
	root["ctRequestId"] = ctRequestId;
    std::cout << "[REPLY: postCancelOneOrderProcessing]: " << root << std::endl;
	
    return fastWriter.write(root);
}

std::string Engine::postCancelAllOrdersProcessing(const std::string& ctRequestId)
{
    Json::FastWriter fastWriter;
	
    return fastWriter.write(exchangeAnswer);
}

std::string Engine::postGetTradeHistory(const std::string& ctRequestId, const std::string& symbol, const int uid)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;
    Json::Value kraken_root;

	int i(0);

	std::string type, side;

    util::MySqlStore mySqlStore;
    mySqlStore.deleteTradeHistory_Kraken(uid);

    kraken_root = exchangeAnswer["result"]["trades"];
    for ( Json::Value::const_iterator itr = kraken_root.begin(); itr != kraken_root.end(); ++itr ){
        type = (*itr)["ordertype"].asString();
		side = (*itr)["type"].asString();
		boost::to_upper(side);
		boost::to_upper(type);
			
		element[i]["orderId"]           = itr.key();
        element[i]["origQty"]           = (*itr)["vol"];
		element[i]["price"]		        = (*itr)["cost"];
		element[i]["side"]		        = side;
		element[i]["status"]	        = "CLOSED";
		element[i]["symbol"]	        = (*itr)["pair"];
		element[i]["type"]		        = type;
		element[i]["time"]		        = (*itr)["time"].asDouble();
        element[i]["fee_currency"]      = (*itr)["fee_currency"];
        element[i]["fee_amount"]        = (*itr)["fee"];
        element[i]["tid"]               = (*itr)["postxid"];

		
		std::string side    = (*itr)["type"].asString();
		double amount       = std::atof((*itr)["vol"].asString().c_str());
        double price        = std::atof((*itr)["cost"].asString().c_str());
        double fee_amount   = std::atof((*itr)["fee"].asString().c_str());

		if (side == "sell") 
				amount = -1 * amount;

        mySqlStore.updateTradeHistory_Kraken(uid, element[i]["symbol"].asString(), amount, price, (*itr)["ordertype"].asString(),itr.key().asString(), (*itr)["postxid"].asString(), fee_amount, (*itr)["fee_currency"].asString(), (*itr)["time"].asString());

        i++;
    }

	root["ctRequestId"] = ctRequestId;
	root["reply"]       = element;
    root["msg"]  	    = exchangeAnswer["error"];

    std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);
}


std::string Engine::postGetPortfolioProcessing(const std::string& ctRequestId, const int uid, Json::Value exchangeAnswer_balance)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element, balance;
    std::string symbol, asset, total;
	util::MySqlStore mySqlStore;

    unsigned int i(0), j;
	
	mySqlStore.clearAccountBalance(uid, "KRAKEN");

	for (Json::Value::iterator it =  exchangeAnswer_balance["result"].begin(); it != exchangeAnswer_balance["result"].end(); ++it){
		Json::Value value 	= (*it);
		symbol      	        = it.key().asString().substr(1,3);
		total	            	= (*it).asString();
		
		if (std::stof(total) > 0) {
			mySqlStore.updateAccountBalance(uid, "KRAKEN", symbol, std::stof(total));

        	for (j =0; j < exchangeAnswer.size(); j++){
            	asset = (exchangeAnswer[j]["asset"].asString()).substr(1,3);
            	if (symbol == asset) {
            		element[i]["symbol"]    		= exchangeAnswer[j]["asset"].asString().substr(1,3);
            		element[i]["average_cost"]      = exchangeAnswer[j]["average_cost"].asDouble();
            		element[i]["total_amount"]		= std::stof(total.c_str());
					element[i]["total_cost"]        = std::stof(total.c_str()) * exchangeAnswer[j]["average_cost"].asDouble();

					std::cout << element << std::endl;
				}
        	}
			i++;
		}
	}

    root["ctRequestId"] = ctRequestId;
    root["reply"] = element;

    std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);
}

std::string Engine::postGetOrderBookProcessing(const std::string& ctRequestId, const std::string& symbol)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

	std::string quote, base, book_symbol;
	quote 		= symbol.substr(0,3);
	base 		= symbol.substr(symbol.size()-3);
	book_symbol = "X" + quote + "Z" + base;
	std::cout << "quote: " << quote << " base: " << base << ": book symbol: " << book_symbol << std::endl;
	
	element["asks"]			= exchangeAnswer["result"][book_symbol]["asks"];
    element["bids"]     	= exchangeAnswer["result"][book_symbol]["bids"];
	
	root["exchange"]	= "KRAKEN";
	root["symbol"]		= symbol;
    root["ctRequestId"] = ctRequestId;
    root["reply"] 		= element;

    std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);
}


/******************************************************************************/
// Public end points
using namespace Trading::Kraken::generic;
// Get server time
std::string GetServerTime::process(const ParseRequest& parseRequest)
{
    krakCPP.get_serverTime(exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// Get asset info
std::string GetAssetInfo::process(const ParseRequest& parseRequest)
{
    krakCPP.get_assetInfo( parseRequest.symbol.c_str(), exchangeAnswer );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// Get tradable asset pairs
std::string GetAssetPairs::process(const ParseRequest& parseRequest)
{
    krakCPP.get_assetPairs( parseRequest.symbol.c_str(), exchangeAnswer );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// Get ticker information
std::string GetTicker::process(const ParseRequest& parseRequest)
{
	std::cout << "[account::GetTicker]: " << "symbol: " << parseRequest.symbol.c_str() << std::endl;

    krakCPP.get_ticker( parseRequest.symbol.c_str(), exchangeAnswer );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postGetTickerProcessing(parseRequest.ctRequestId.c_str(), parseRequest.symbol.c_str());
}

// Get order book
std::string GetOrderbook::process(const ParseRequest& parseRequest)
{
    krakCPP.get_orderBook( parseRequest.symbol.c_str(), exchangeAnswer, parseRequest.limit );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// Get recent trades
std::string GetTrades::process(const ParseRequest& parseRequest)
{
	//krakCPP.get_recentTrades( parseRequest.symbol.c_str(), exchangeAnswer,  );
	std::cout << "Not implemented." << std::endl;
    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// Get recent spread data
std::string GetSpread::process(const ParseRequest& parseRequest)
{
	std::cout << "Not implemented" << std::endl;
    // krakCPP.get_spread(exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

/******************************************************************************/
// Private endpoints
// GET /account/getbalance
// Used to retrieve the balance from your account for a specific currency.
using namespace Trading::Kraken::account;

std::string GetBalance::process(const ParseRequest& parseRequest)
{        
    std::cout << "[account::GetBalance]: " << std::endl;

    krakCPP.get_balance( exchangeAnswer );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postGetBalanceProcessing(parseRequest.ctRequestId.c_str(), parseRequest.userID);
}

std::string GetTradeBalance::process(const ParseRequest& parseRequest)
{        
    std::cout << "[account::GetTradeBalance]: " << parseRequest.symbol << std::endl;

    krakCPP.get_tradeBalance( parseRequest.symbol.c_str(), exchangeAnswer );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetPortfolio::process(const ParseRequest& parseRequest)
{
    std::cout << "[account::GetPortfolio] " << std::endl;

    Json::Value exchangeAnswer_balance;
    util::MySqlStore mySqlStore;
	exchangeAnswer = mySqlStore.calcPortfolioCostKraken(parseRequest.userID);

    krakCPP.get_balance( exchangeAnswer_balance );
	
	std::cout << exchangeAnswer << std::endl;
    std::cout << exchangeAnswer_balance << std::endl;

    return Engine::postGetPortfolioProcessing( parseRequest.ctRequestId.c_str(), parseRequest.userID, exchangeAnswer_balance );
}


/******************************************************************************/
// Private endpoints for trade related API: orders and trade history
using namespace Trading::Kraken::order;
// Get trades history
std::string GetTradeHistory::process(const ParseRequest& parseRequest)
{        
    std::cout << "[order::GetTradeHistory]: " << std::endl;

    krakCPP.get_tradeHistory( exchangeAnswer );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postGetTradeHistory(parseRequest.ctRequestId.c_str(), parseRequest.symbol.c_str(), parseRequest.userID);
}

// Get open orders
std::string GetAllOpenOrders::process(const ParseRequest& parseRequest)
{        
    std::cout << "[order::GetAllOpenOrders]: " << std::endl;

    krakCPP.get_openOrders( exchangeAnswer );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postGetOpenOrdersProcessing(parseRequest.ctRequestId.c_str());
}

// Get closed orders
std::string GetClosedOrders::process(const ParseRequest& parseRequest)
{        
    std::cout << "[order::GetClosedOrders]: " << std::endl;

    krakCPP.get_closedOrders( exchangeAnswer );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// Query orders info
std::string GetOrderStatus::process(const ParseRequest& parseRequest)
{        
    std::cout << "[order::GetOrderStatus]: " << " userref: " << parseRequest.ctRequestId.c_str()  << std::endl;

    krakCPP.query_orders( parseRequest.orderId.c_str(), exchangeAnswer );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postGetOrderStatusProcessing(parseRequest.ctRequestId.c_str(), parseRequest.orderId.c_str());
}

// Query trades info
std::string QueryTrades::process(const ParseRequest& parseRequest)
{        
    std::cout << "[order::QueryTrades]: " << std::endl;

    krakCPP.query_trades( exchangeAnswer );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// Get open positions
std::string GetOpenPositions::process(const ParseRequest& parseRequest)
{        
    std::cout << "[order::GetOpenPositions]: " << std::endl;

    krakCPP.get_openPositions( exchangeAnswer );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// Get ledgers info
std::string GetLedgerInfo::process(const ParseRequest& parseRequest)
{        
    std::cout << "[order::GetLedgerInfo]: " << std::endl;

    krakCPP.get_ledgerInfo( exchangeAnswer );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// Query ledgers
std::string QueryLedgers::process(const ParseRequest& parseRequest)
{        
    std::cout << "[order::QueryLedgers]: " << std::endl;

    krakCPP.query_ledgers( exchangeAnswer );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// Get trade volume
std::string GetTradeVolume::process(const ParseRequest& parseRequest)
{        
    std::cout << "[order::GetTradeVolume]: " << std::endl;

    krakCPP.get_tradeVolume( exchangeAnswer );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// Send market order
std::string SendMarketOrder::process(const ParseRequest& parseRequest)
{        
    std::cout << "[order::SendMarketOrder]: " << "symbol: " << parseRequest.symbol << " side: " << parseRequest.orderSide << " quantity: " << parseRequest.quantity << " ctRequestId(userref): " << parseRequest.ctRequestId << " leverage: " << parseRequest.leverage.c_str() << std::endl;
	
	std::string side = parseRequest.orderSide; // buy or sell
    std::transform(side.begin(), side.end(), side.begin(), ::tolower);

    krakCPP.send_order_market(parseRequest.symbol.c_str(), side.c_str(), parseRequest.quantity, parseRequest.ctRequestId.c_str(), exchangeAnswer, parseRequest.leverage.c_str() );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postSendOrderProcessing(parseRequest.ctRequestId.c_str());
}


// Send limit order
std::string SendLimitOrder::process(const ParseRequest& parseRequest)
{
    std::cout << "[order::SendLimitOrder]: " << "symbol: " << parseRequest.symbol << " side: " << parseRequest.orderSide << " quantity: " << parseRequest.quantity << " price: " << parseRequest.price << " ctRequestId(userref): " << parseRequest.ctRequestId <<  " startTime: " << parseRequest.startTime.c_str() << " expireTime: " << parseRequest.expireTime.c_str()  << " leverage: " << parseRequest.leverage.c_str() << std::endl;
	
	std::string side = parseRequest.orderSide; // buy or sell
	std::transform(side.begin(), side.end(), side.begin(), ::tolower);

    krakCPP.send_order_limit( parseRequest.symbol.c_str(), side.c_str(), parseRequest.quantity, parseRequest.price, parseRequest.ctRequestId.c_str(), exchangeAnswer, parseRequest.startTime.c_str(), parseRequest.expireTime.c_str(), parseRequest.leverage.c_str() );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postSendOrderProcessing(parseRequest.ctRequestId.c_str());
}

std::string CancelOneOrder::process(const ParseRequest& parseRequest)
{
    std::cout << "[order::CancelOneOrder]: " << " ctRequestId(userref): " << parseRequest.ctRequestId << std::endl;

	krakCPP.cancel_order( parseRequest.orderId.c_str(), exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postCancelOneOrderProcessing(parseRequest.ctRequestId.c_str(), parseRequest.orderId.c_str());
}

std::string GetOrderBook::process(const ParseRequest& parseRequest)
{
    int count;
	
	if (!parseRequest.bookLevel.empty()){
		count = std::stoi(parseRequest.bookLevel);
	} else {
		count = 20;
	}

	std::cout << "[order::GetOrderBook]: " << " ctRequestId(userref): " << parseRequest.ctRequestId << " count: " << count <<std::endl;
    
	krakCPP.get_orderBook( parseRequest.symbol.c_str(), exchangeAnswer, count);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postGetOrderBookProcessing(parseRequest.ctRequestId.c_str(), parseRequest.symbol.c_str());
}

std::string GetAllOrders::process(const ParseRequest& parseRequest)
{
    std::cout << "[order::GetAllOrders]: " << std::endl;

    krakCPP.get_tradeHistory( exchangeAnswer );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postGetTradeHistory(parseRequest.ctRequestId.c_str(), parseRequest.symbol.c_str(), parseRequest.userID);
}
