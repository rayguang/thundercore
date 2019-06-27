#include "coinbase.h"
#include "mySqlStore.h"

#include <coincpp_websocket.h>

#include <json/json.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <Poco/Bugcheck.h>

#include <iostream>
#include <boost/algorithm/string.hpp>
#include <string>
/******************************************************************************/
using namespace Trading::Coinbase;

Engine::Engine()
    :   recvWindow(10000)
{
}

bool Engine::init(const ParseRequest& parseRequest)
{
    std::string passphrase;

    try
    {
        util::MySqlStore uidSqlStore;
        passphrase = uidSqlStore.fetchExchangeUID(parseRequest.userID, parseRequest.exchange.c_str());
    }
    catch (Poco::Exception& ex)
    {
        std::cerr << ex.displayText() << std::endl;
    }
    // std::cout << "coinbase passphrase: " << passphrase << std::endl;

    return Exchange::init(parseRequest) && coinCPP.init(key, secret, passphrase);
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

std::string Engine::postGetOrderBookProcessing(const std::string& ctRequestId, const std::string& symbol)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;
	
	root["exchange"]	= "COINBASE";
	root["symbol"]		= symbol;
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
    element["price"]    = exchangeAnswer["price"];
    root["ctRequestId"] = ctRequestId;
	root["reply"] = element;

    std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);
}

std::string Engine::postGetBalanceProcessing(const std::string& ctRequestId, const int uid)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

	int index = 0;

     for (unsigned int i = 0 ; i < exchangeAnswer.size() ; i++)
    {
        if (exchangeAnswer[i]["available"].asString() != "0"  || exchangeAnswer[i]["balance"].asString() != "0.0000000000000000")
        {
            element[index]["Symbol"]    =   exchangeAnswer[i]["currency"].asString();
            element[index]["Free"]      =   exchangeAnswer[i]["available"].asString();
            element[index]["Locked"]    =   exchangeAnswer[i]["hold"].asString();
            element[index]["Total"]     =   std::to_string(std::stof(element[index]["Free"].asString()) + std::stof(element[index]["Locked"].asString()));

            util::MySqlStore mySqlStore;
			// std::cout << "[DB - INSERT ]" << uid << ' ' << "COINBASE" << ' ' << element[index]["Symbol"]  << ' ' << element[index]["Total"] << std::endl;
            mySqlStore.updateAccountBalance(uid, "COINBASE", element[index]["Symbol"].asString() , std::atof(element[index]["Total"].asString().c_str()));
			index++;
        }
	}

    root["ctRequestId"] = ctRequestId;
	root["balances"] = element;

    std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);
}

std::string Engine::postSendOrderProcessing(const std::string& ctRequestId)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

    root["ctRequestId"] = ctRequestId;
    root["orderId"]     = exchangeAnswer["id"];
	root["status"]		= exchangeAnswer["status"];
    root["price"]		= exchangeAnswer["price"];
	root["symbol"]		= exchangeAnswer["product_id"];
	root["side"]		= exchangeAnswer["side"];
	root["type"]		= exchangeAnswer["type"];
	root["msg"]     	= exchangeAnswer["message"];

    std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);
}

std::string Engine::postGetOpenOrdersProcessing(const std::string& ctRequestId)
{
    Json::FastWriter fastWriter;
    Json::Value root;
	Json::Value element;

    for (unsigned int i = 0 ; i < exchangeAnswer.size() ; i++)
    {
		std::string type, side, status;
		type 	= exchangeAnswer[i]["type"].asString();
		side	= exchangeAnswer[i]["side"].asString();
		status 	= exchangeAnswer[i]["status"].asString();
		boost::to_upper(type);
		boost::to_upper(side);
		boost::to_upper(status);


        element[i]["orderId"]            = exchangeAnswer[i]["id"];
        element[i]["origQty"]  	         = exchangeAnswer[i]["size"];
        element[i]["price"]              = exchangeAnswer[i]["price"];
        element[i]["stopPrice"]          = exchangeAnswer[i]["stp"];
        element[i]["side"]               = side;
        element[i]["status"]             = status;
        element[i]["symbol"]             = exchangeAnswer[i]["product_id"];
        element[i]["type"]               = type;
		element[i]["tif"]				 = exchangeAnswer[i]["time_in_force"];
		element[i]["filled_size"]		 = exchangeAnswer[i]["filled_size"];
		element[i]["executed_value"]     = exchangeAnswer[i]["executed_value"];
		
		int y, M, d, h, m;
		float s;
		std::string date_string		  = exchangeAnswer[i]["created_at"].asString();
		sscanf(date_string.c_str(), "%d-%d-%dT%d%d%f", &y, &M, &d, &h, &m, &s);

        element[i]["time"]            = to_string(y) + "-" + to_string(M) + "-" + to_string(d);
	}

	root["reply"] = element;
	root["ctRequestId"] = ctRequestId;
    std::cout << "[REPLY: postGetOpenOrdersProcessing]: " << root << std::endl;

    return fastWriter.write(root);
}

std::string Engine::postGetOrderStatusProcessing(const std::string& ctRequestId)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

    element["orderId"]         = exchangeAnswer["id"];
    element["symbol"]          = exchangeAnswer["product_id"];
    element["price"]           = exchangeAnswer["price"];
    element["origQty"]         = exchangeAnswer["size"];
	element["side"]			   = exchangeAnswer["side"];
	element["type"]			   = exchangeAnswer["type"];
    element["funds"]       	   = exchangeAnswer["funds"];
	element["status"]		   = exchangeAnswer["status"];
	element["tif"]			   = exchangeAnswer["time_in_force"];

	int y, M, d, h, m;
    float s;
    std::string date_string       = exchangeAnswer["created_at"].asString();
    sscanf(date_string.c_str(), "%d-%d-%dT%d%d%f", &y, &M, &d, &h, &m, &s);

	element["time"]			   = to_string(y) + "-" + to_string(M) + "-" + to_string(d);

    root["ctRequestId"] = ctRequestId;
    root["reply"]       = element;

    std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);
}

std::string Engine::postGetAllOrdersProcessing(const std::string& ctRequestId)
{
	Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;
	
	if (exchangeAnswer.size() == 1) {
		root["ctRequestId"] = ctRequestId;
		root["msg"]     	= exchangeAnswer["message"];
	} else {
		for (unsigned int i = 0 ; i < exchangeAnswer.size() ; i++)
    	{
			std::string type, side, status;
        	type    = exchangeAnswer[i]["type"].asString();
        	side    = exchangeAnswer[i]["side"].asString();
        	status  = exchangeAnswer[i]["status"].asString();
        	boost::to_upper(type);
        	boost::to_upper(side);
        	boost::to_upper(status);

			double executed_value, size;
			size = std::stof(exchangeAnswer[i]["size"].asString());
			executed_value = std::stof(exchangeAnswer[i]["executed_value"].asString());

        	element[i]["orderId"]            = exchangeAnswer[i]["id"];
        	element[i]["origQty"]            = exchangeAnswer[i]["size"];
        	element[i]["stopPrice"]          = exchangeAnswer[i]["stp"];
        	element[i]["side"]               = side;
        	element[i]["status"]             = status;
        	element[i]["symbol"]             = exchangeAnswer[i]["product_id"];
        	element[i]["type"]               = type;
        	element[i]["tif"]                = exchangeAnswer[i]["time_in_force"];
        	element[i]["filled_size"]        = exchangeAnswer[i]["filled_size"];
        	element[i]["executed_value"]     = exchangeAnswer[i]["executed_value"];
			
			element[i]["price"]              = std::to_string(executed_value/size);

        	int y, M, d, h, m;
        	float s;
        	std::string date_string       = exchangeAnswer[i]["created_at"].asString();
        	sscanf(date_string.c_str(), "%d-%d-%dT%d%d%f", &y, &M, &d, &h, &m, &s);

        	element[i]["time"]            = to_string(y) + "-" + to_string(M) + "-" + to_string(d);
    	}

    	root["reply"]       = element;
    	root["ctRequestId"] = ctRequestId;
	}

    std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);
}

std::string Engine::postCancelOneOrderProcessing(const std::string& ctRequestId, const std::string& orderId)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

    root["ctRequestId"]             = ctRequestId;
    root["msg"]   		            = exchangeAnswer["message"];
    root["reply"]["orderId"]        = orderId;


    std::cout << "[REPLY: postCancelOneOrderProcessing]: " << root << std::endl;

    return fastWriter.write(root);
}

std::string Engine::postCancelAllOrdersProcessing(const std::string& ctRequestId)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

	for (unsigned i =0; i < exchangeAnswer.size(); i++){
		element[i]["orderId"] 		= exchangeAnswer[i];
	}

    root["ctRequestId"]             = ctRequestId;
    root["msg"]  	                = exchangeAnswer["message"];

    root["reply"] = element;

    std::cout << "[REPLY: postCancelAllOrdersProcessing]: " << root << std::endl;

    return fastWriter.write(root);
}

std::string Engine::postGetTradeHistoryProcessing(const std::string& ctRequestId, const int uid)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

	unsigned int i(0);

    util::MySqlStore mySqlStore;
    mySqlStore.deleteTradeHistory_Coinbase(uid);

    for ( i = 0; i < exchangeAnswer.size(); i++ ){
        element[i]["orderId"]           = exchangeAnswer[i]["order_id"];
        element[i]["origQty"]           = exchangeAnswer[i]["size"];
		element[i]["price"]		        = exchangeAnswer[i]["price"];
		element[i]["side"]		        = exchangeAnswer[i]["side"];
		element[i]["symbol"]	        = exchangeAnswer[i]["product_id"];
		element[i]["type"]		        = exchangeAnswer[i]["liquidity"];
		element[i]["time"]		        = exchangeAnswer[i]["created_at"];
        element[i]["fee_currency"]      = exchangeAnswer[i]["product_id"].asString().substr(4,3);
        element[i]["fee_amount"]        = exchangeAnswer[i]["fee"];
        element[i]["tid"]               = exchangeAnswer[i]["trade_id"];
        //element[i]["status"]	        = (*itr)["status"];
		
		std::string side, fee_currency;
		double amount       = std::atof(exchangeAnswer[i]["size"].asString().c_str());
        double price        = std::atof(exchangeAnswer[i]["price"].asString().c_str());
		side 				= exchangeAnswer[i]["side"].asString();
		fee_currency 		= exchangeAnswer[i]["product_id"].asString().substr(0,3);
		if (side == "sell") 
				amount = -1 * amount;

        mySqlStore.updateTradeHistory_Coinbase(uid, element[i]["symbol"].asString(), amount, price, exchangeAnswer[i]["liquidity"].asString(),exchangeAnswer[i]["order_id"].asString(), exchangeAnswer[i]["order_id"].asString(), std::atof(exchangeAnswer[i]["fee"].asString().c_str()), fee_currency, exchangeAnswer[i]["trade_id"].asString());
    }

	root["ctRequestId"] = ctRequestId;
	root["reply"]       = element;

    std::cout << "[REPLY: postGetTradeHistoryProcessing]: " << root << std::endl;
    return fastWriter.write(root);
}

std::string Engine::postGetPortfolioProcessing(const std::string& ctRequestId, const int uid, Json::Value exchangeAnswer_balance)
{
    //我知道正确的Portfolio计算方法，以下方法不对。公司急着上线，不给时间研发，没办法 
	Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element, balance;
    std::string symbol, asset, total, base;

    unsigned int i(0), j(0), index(0);
    double available_amount, balance_amount;

	util::MySqlStore mySqlStore;

	mySqlStore.clearAccountBalance(uid, "COINBASE");
	
	for ( i = 0; i < exchangeAnswer_balance.size(); i ++){
        available_amount   = std::stof(exchangeAnswer_balance[i]["available"].asString());
        balance_amount     = std::stof(exchangeAnswer_balance[i]["balance"].asString());
		symbol 			   = exchangeAnswer_balance[i]["currency"].asString();

        if ( available_amount > 0 || balance_amount > 0) {
		   mySqlStore.updateAccountBalance(uid, "COINBASE", symbol.substr(0,3), balance_amount);
           for ( j = 0 ; j < exchangeAnswer.size(); j++) {
		   		asset = exchangeAnswer[j]["asset"].asString().substr(0,3);
				base  = exchangeAnswer[j]["asset"].asString().substr(4,3);
		 		
			   if (asset == symbol) {
			   		element[index]["symbol"]		= exchangeAnswer_balance[i]["currency"].asString();
					element[index]["average_cost"]	= exchangeAnswer[j]["average_cost"].asString();
					element[index]["total_amount"]  = exchangeAnswer_balance[i]["balance"].asString();
					element[index]["total_cost"]    = std::stof(exchangeAnswer[j]["average_cost"].asString()) * std::stof(exchangeAnswer_balance[i]["balance"].asString());

					index++;
			   } else {
			   		element[index]["symbol"]        = exchangeAnswer_balance[i]["currency"].asString();
                    element[index]["average_cost"]  = exchangeAnswer_balance[i]["balance"].asString();;
                    element[index]["total_amount"]  = exchangeAnswer_balance[i]["balance"].asString();
			   }	   
		   }  
        }
    }

    root["ctRequestId"] = ctRequestId;
    root["reply"] = element;

    std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);
}

/******************************************************************************/
// Public end points
using namespace Trading::Coinbase::generic;

std::string GetServerTime::process(const ParseRequest& parseRequest)
{
    coinCPP.get_serverTime(exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}


std::string GetProducts::process(const ParseRequest& parseRequest)
{
    std::cout << "[generic::GetProducts]: " << std::endl;

    coinCPP.get_products(exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}


std::string GetTicker::process(const ParseRequest& parseRequest)
{
    std::cout << "[generic::GetTicker]: " << "symbol: " << parseRequest.symbol.c_str() << std::endl;

    if (parseRequest.symbol.empty())
    {
        std::stringstream reply;
        reply << "{\"message\":\"" << "Symbol is not provided." << "}";
        return reply.str();
    }
         

    coinCPP.get_productTicker(parseRequest.symbol.c_str(), exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postGetTickerProcessing(parseRequest.ctRequestId.c_str(), parseRequest.symbol.c_str());
}

std::string GetProductTrades::process(const ParseRequest& parseRequest)
{
    std::cout << "[generic::GetProductTrades]: " << "symbol: " << parseRequest.symbol.c_str() << std::endl;

    coinCPP.get_productTrades(parseRequest.symbol.c_str(), exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetCurrencies::process(const ParseRequest& parseRequest)
{
    std::cout << "[generic::GetCurrencies]: " << std::endl;

    coinCPP.get_productTrades(parseRequest.symbol.c_str(), exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}


/******************************************************************************/
// Private endpoints
// GET /account/getbalance
// Used to retrieve the balance from your account for a specific currency.
using namespace Trading::Coinbase::account;
// List Accounts
std::string GetBalance::process(const ParseRequest& parseRequest)
{        
    std::cout << "[account::GetBalance]: " << std::endl;

    coinCPP.get_listAccounts( exchangeAnswer );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postGetBalanceProcessing(parseRequest.ctRequestId.c_str(), parseRequest.userID);
}

// Get an Account
std::string GetAccount::process(const ParseRequest& parseRequest)
{        
    std::cout << "[account::GetAccount]: " << " accountId: " << parseRequest.accountId << std::endl;

    coinCPP.get_account( parseRequest.accountId.c_str(), exchangeAnswer );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postGetBalanceProcessing(parseRequest.ctRequestId.c_str(), parseRequest.userID);
}

// Get Account History
std::string GetAccountHistory::process(const ParseRequest& parseRequest)
{        
    std::cout << "[account::GetAccountHistory]: " << " accountId: " << parseRequest.accountId << std::endl;

    coinCPP.get_accountHistory( parseRequest.accountId.c_str(), exchangeAnswer );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// Get Holds
std::string GetHolds::process(const ParseRequest& parseRequest)
{        
    std::cout << "[account::GetHolds]: " << " accountId: " << parseRequest.accountId << std::endl;

    coinCPP.get_holds( parseRequest.accountId.c_str(), exchangeAnswer );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetPortfolio::process(const ParseRequest& parseRequest)
{        
    std::cout << "[account::GetPortfolio]: " << " uid: " << parseRequest.userID << std::endl;
	
	util::MySqlStore mySqlStore;
	mySqlStore.clearAccountBalance(parseRequest.userID, "COINBASE");
	exchangeAnswer = mySqlStore.calcPortfolioCostCoinbase(parseRequest.userID);

    Json::Value exchangeAnswer_balance;
    coinCPP.get_listAccounts( exchangeAnswer_balance );

    return Engine::postGetPortfolioProcessing( parseRequest.ctRequestId.c_str(), parseRequest.userID, exchangeAnswer_balance );
}

/******************************************************************************/
using namespace Trading::Coinbase::order;
// Place a market order
std::string SendMarketOrder::process(const ParseRequest& parseRequest)
{        
    std::cout << "[order::SendMarketOrder]: " << " symbol: " << parseRequest.symbol << " quantity: " << parseRequest.quantity << std::endl;

    std::string side = parseRequest.orderSide; // buy or sell
    std::transform(side.begin(), side.end(), side.begin(), ::tolower);

	coinCPP.send_order_market( parseRequest.symbol.c_str(), side.c_str(), parseRequest.quantity, parseRequest.ctRequestId.c_str(), exchangeAnswer );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postSendOrderProcessing(parseRequest.ctRequestId.c_str());
}

// Place a limit order
std::string SendLimitOrder::process(const ParseRequest& parseRequest)
{        
    std::cout << "[order::SendLimitOrder]: " << " symbol: " << parseRequest.symbol << " quantity: " << parseRequest.quantity << " price: " << parseRequest.price << " timeInForce: " << parseRequest.timeInForce << std::endl;

    std::string side = parseRequest.orderSide; // buy or sell
    std::transform(side.begin(), side.end(), side.begin(), ::tolower);
	
	coinCPP.send_order_limit( parseRequest.symbol.c_str(), side.c_str(), parseRequest.quantity, parseRequest.price, parseRequest.timeInForce.c_str(), parseRequest.ctRequestId.c_str(), exchangeAnswer );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postSendOrderProcessing(parseRequest.ctRequestId.c_str());
}

// Place a stop order
std::string SendStopOrder::process(const ParseRequest& parseRequest)
{
    std::cout << "[order::SendStopOrder]: " << " symbol: " << parseRequest.symbol << " quantity: " << parseRequest.quantity << " price: " << parseRequest.price << " timeInForce: " << parseRequest.timeInForce << std::endl;

    //coinCPP.send_order_limit( parseRequest.symbol.c_str(), parseRequest.orderSide.c_str(), parseRequest.quantity, parseRequest.price, parseRequest.timeInForce.c_str(), parseRequest.ctRequestId.c_str(), exchangeAnswer );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postSendOrderProcessing(parseRequest.ctRequestId.c_str());
}

// Cancel one order
std::string CancelOneOrder::process(const ParseRequest& parseRequest)
{        
    std::cout << "[order::CancelOneOrder]: " << " symbol: " << parseRequest.symbol << " orderId: " << parseRequest.orderId << std::endl;

    coinCPP.cancel_oneOrder( parseRequest.orderId.c_str(), exchangeAnswer );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postCancelOneOrderProcessing(parseRequest.ctRequestId.c_str(), parseRequest.orderId.c_str());
}

// Cancel all orders
std::string CancelAllOrders::process(const ParseRequest& parseRequest)
{        
    std::cout << "[order::CancelAllOrders]: " << " symbol: " << parseRequest.symbol << std::endl;

    coinCPP.cancel_allOrders( parseRequest.symbol.c_str(), exchangeAnswer );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postCancelAllOrdersProcessing(parseRequest.ctRequestId.c_str());
}

// Get one order
std::string GetOrderStatus::process(const ParseRequest& parseRequest)
{        
    std::cout << "[order::GetOrderStatus]: " << " orderId: " << parseRequest.ctRequestId << std::endl;

    coinCPP.get_oneOrder( parseRequest.orderId.c_str(), exchangeAnswer );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postGetOrderStatusProcessing(parseRequest.ctRequestId.c_str());
}


// Get all orders
std::string GetAllOrders::process(const ParseRequest& parseRequest)
{        
    std::cout << "[order::GetAllOrders]: " << " symbol: " << parseRequest.symbol << std::endl;

    coinCPP.get_allOrders( parseRequest.symbol.c_str(), "all", exchangeAnswer );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postGetAllOrdersProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetAllOpenOrders::process(const ParseRequest& parseRequest)
{        
    std::cout << "[order::GetAllOpenOrders]: " << " symbol: " << parseRequest.symbol << std::endl;

	std::string status;
	status = parseRequest.orderStatus;
	
	coinCPP.get_allOrders( parseRequest.symbol.c_str(), "open", exchangeAnswer );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postGetOpenOrdersProcessing(parseRequest.ctRequestId.c_str());
}

// Get fills
std::string GetTradeHistory::process(const ParseRequest& parseRequest)
{
    std::cout << "[order::GetTradeHistory]: " << " symbol: " << parseRequest.symbol << std::endl;
	
	std::string symbol("BTC-USD");
	if (!parseRequest.symbol.empty())
			symbol = parseRequest.symbol;
	coinCPP.get_fills( symbol.c_str(), exchangeAnswer );

    std::cout << exchangeAnswer << std::endl;

    return Engine::postGetTradeHistoryProcessing(parseRequest.ctRequestId.c_str(), parseRequest.userID);
}

std::string GetOrderBook::process(const ParseRequest& parseRequest)
{
    std::cout << "[generic::GetOrderBook]: " << "symbol: " << parseRequest.symbol.c_str() << std::endl;

    // 1    Only the best bid and ask
    // 2    Top 50 bids and asks (aggregated)
    // 3    Full order book (non aggregated)
	std::string level("2");
	if (!parseRequest.bookLevel.empty())
			level = parseRequest.bookLevel;
    coinCPP.get_productOrderBook(parseRequest.symbol.c_str(), exchangeAnswer, level);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postGetOrderBookProcessing(parseRequest.ctRequestId.c_str(), parseRequest.symbol.c_str());
}
