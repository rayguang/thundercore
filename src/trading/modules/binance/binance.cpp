#include "binance.h"
#include "mySqlStore.h"

#include <binacpp_websocket.h>

#include <json/json.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <Poco/Bugcheck.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
/******************************************************************************/
using namespace Trading::Binance;

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

    return Exchange::init(parseRequest) && binaCPP.init(key,secret);
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

	std::cout << root << std::endl;

    return fastWriter.write(root);
}

std::string Engine::postRequestProcessing(const std::string& ctRequestId)
{
    Json::FastWriter fastWriter;
	Json::Value root;
	Json::Value element;
	element["ctRequestId"] = ctRequestId;
	root.append(element);

	std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);
}

std::string Engine::postGetTickerProcessing(const std::string& ctRequestId)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

    root["ctRequestId"] = ctRequestId;
	root["reply"] = exchangeAnswer;

    std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);
}

std::string Engine::postGetMarketProcessing(const std::string& ctRequestId)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

    root["ctRequestId"] = ctRequestId;
	root["reply"] = exchangeAnswer;

    std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);
}

std::string Engine::postGetOrderBookProcessing(const std::string& ctRequestId, const std::string& symbol)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

	root["exchange"]		= "BINANCE";
	root["symbol"]			= symbol;
    root["ctRequestId"] 	= ctRequestId;
	root["reply"] 			= exchangeAnswer;

    std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);
}


std::string Engine::postGetOrderStatusProcessing(const std::string& ctRequestId)
{
    Json::FastWriter fastWriter;
    Json::Value root;
	Json::Value element;
	
	element["symbol"]       = exchangeAnswer["symbol"];
    element["orderId"]      = exchangeAnswer["orderId"].asString();
    element["status"]       = exchangeAnswer["status"];
    element["price"]        = exchangeAnswer["symbol"];
    element["stopPrice"]	= exchangeAnswer["stopPrice"];
	element["origQty"]      = exchangeAnswer["origQty"];
    element["side"]         = exchangeAnswer["side"];
    element["type"]         = exchangeAnswer["type"];
    element["ctOrderId"]    = exchangeAnswer["clientOrderId"];
	element["time"]			= exchangeAnswer["time"];
	element["updateTime"]	= exchangeAnswer["updateTime"];

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

    for (unsigned int i = 0 ; i < exchangeAnswer.size() ; i++)
    {
        element[i]["orderId"]            = exchangeAnswer[i]["orderId"].asString();
        element[i]["origQty"]  	         = exchangeAnswer[i]["origQty"];
        element[i]["price"]              = exchangeAnswer[i]["price"];
        element[i]["stopPrice"]          = exchangeAnswer[i]["stopPrice"];
        element[i]["side"]               = exchangeAnswer[i]["side"];
        element[i]["status"]             = exchangeAnswer[i]["status"];
        element[i]["symbol"]             = exchangeAnswer[i]["symbol"];
        element[i]["type"]               = exchangeAnswer[i]["type"];
		element[i]["time"]   	         = exchangeAnswer[i]["time"];
    	element[i]["updateTime"]  		 = exchangeAnswer[i]["updateTime"];
	}

	root["reply"] = element;
	root["ctRequestId"] = ctRequestId;
	
	std::cout << "root:" << root << std::endl;

    std::cout << "[REPLY: postGetOpenOrdersProcessing]: " << root << std::endl;

    return fastWriter.write(root);
}

std::string Engine::postGetAllOrdersProcessing(const std::string& ctRequestId)
{
    Json::FastWriter fastWriter;
    Json::Value root;
	Json::Value element;

	// std::cout << "exchangeAnswer[0]: " << exchangeAnswer[0] << "size: " << exchangeAnswer.size()<< std::endl;

    for (unsigned int i = 0 ; i < exchangeAnswer.size() ; i++)
    {
        // send ctRequestId, clientOrderId, orderId, origQty, price, side, status, symbol, type, updateTime
        // break the batch into 20 orders every batch
        
		double cummulativeQuoteQty, executedQty;
		executedQty = std::stof(exchangeAnswer[i]["executedQty"].asString());
		cummulativeQuoteQty = std::stof(exchangeAnswer[i]["cummulativeQuoteQty"].asString());

		element[i]["orderId"]            = exchangeAnswer[i]["orderId"].asString();
        element[i]["origQty"]  	         = exchangeAnswer[i]["origQty"];
        element[i]["side"]               = exchangeAnswer[i]["side"];
        element[i]["symbol"]             = exchangeAnswer[i]["symbol"];
        element[i]["type"]               = exchangeAnswer[i]["type"];
        element[i]["status"]             = exchangeAnswer[i]["status"];
        // element[i]["stopPrice"]          = exchangeAnswer[i]["stopPrice"];
        element[i]["time"]           	 = exchangeAnswer[i]["time"];

		if (exchangeAnswer[i]["type"].asString() == "MARKET") {
			element[i]["price"]			= std::to_string(cummulativeQuoteQty / executedQty);
		} else {
			element[i]["price"]         = exchangeAnswer[i]["price"];
		}
		
	}

	root["reply"] = element;
	root["ctRequestId"] = ctRequestId;

    std::cout << "[REPLY: postGetAllOrdersProcessing]: " << root << std::endl;

    return fastWriter.write(root);
}

std::string Engine::postGetTradeHistoryProcessing(const std::string& ctRequestId)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

    for (unsigned int i = 0 ; i < exchangeAnswer.size() ; i++)
    {
        // send ctRequestId, clientOrderId, orderId, origQty, price, side, status, symbol, type, updateTime
        // break the batch into 20 orders every batch
        //element[i]["clientOrderId"]      = exchangeAnswer[i]["clientOrderId"].asString();
        element[i]["orderId"]            = exchangeAnswer[i]["orderId"].asString();
        element[i]["origQty"]            = exchangeAnswer[i]["qty"].asString();
        element[i]["price"]              = exchangeAnswer[i]["price"].asString();
        element[i]["symbol"]             = exchangeAnswer[i]["symbol"].asString();
		element[i]["tranTime"]           = exchangeAnswer[i]["time"].asString();
    	
		if (exchangeAnswer[i]["isBuyer"]== "true"){
			element[i]["side"] = "BUY";
		} else {
			element[i]["side"] = "SELL";
		}

    }

    root["reply"] = element;
    root["ctRequestId"] = ctRequestId;

    std::cout << "[REPLY: postGetAllOrdersProcessing]: " << root << std::endl;

    return fastWriter.write(root);
}

std::string Engine::postCancelOneOrderProcessing(const std::string& ctRequestId)
{
    Json::FastWriter fastWriter;
	Json::Value root;
    Json::Value element;
	
    element[0]["orderId"]            = exchangeAnswer["orderId"].asString();
    element[0]["status"]             = exchangeAnswer["status"].asString();
	element[0]["clientOrderId"]		 = exchangeAnswer["clientOrderId"].asString();
	
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


std::string Engine::postGetMyTradesProcessing()
{
    Json::FastWriter fastWriter;

    return fastWriter.write(exchangeAnswer);
}


std::string Engine::postGetBalanceProcessing(const std::string& ctRequestId, const int uid)
{
	if (exchangeAnswer["code"].asInt() < 0)
	{   Json::FastWriter fastWriter;
        return fastWriter.write(exchangeAnswer);
	}

	Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;
    element["ctRequestId"] = ctRequestId;
    root.append(element);

    std::stringstream accountReply;
    std::map<std::string, std::map<std::string, double> > userBalance;       // Exchange reply: all balances whether zero or non-zero balances  
    std::multimap<std::string, std::pair<std::string, double> > userSimpleBalance; // Balances with non-zero assets
    //std::map<std::string, std::map<std::string, double> >::iterator it_i;

    for (unsigned int i = 0; i < exchangeAnswer["balances"].size(); i++)
    {
        std::string symbol = exchangeAnswer["balances"][i]["asset"].asString().c_str();
        userBalance[symbol]["f"] = atof(exchangeAnswer["balances"][i]["free"].asString().c_str());
        userBalance[symbol]["l"] = atof(exchangeAnswer["balances"][i]["locked"].asString().c_str()); 
        userBalance[symbol]["t"] =  userBalance[symbol]["f"] + userBalance[symbol]["l"];

        //if ( userBalance[symbol]["f"] > 0 ) userSimpleBalance.insert(symbol, make_pair('free', userBalance[symbol]["f"] ));
        //if ( userBalance[symbol]["l"] > 0 ) userSimpleBalance.insert(symbol, make_pair('locked', userBalance[symbol]["l"] ));        
    }

	accountReply << "{\"ctRequestId\":\"" << ctRequestId << "\"," << "\"balances\":"  << "[";
    for (auto it = userBalance.begin();it != userBalance.end(); it++)
    {	
		std::stringstream tempBalance;
        std::string symbol = (*it).first;
        std::map<std::string, double> balance = (*it).second;
        if (balance["t"] > 0 ) 
		{
			cout << symbol << ", ";
        	printf("Free   : %.08f, ", balance["f"] );
			printf("Locked : %.08f " , balance["l"] );
        	printf("Total  : %.08f", balance["t"]);
			cout << " " << endl;

			tempBalance << "{ \"Symbol\":\"" << symbol << "\"," << "\"Free\":" << balance["f"] << ","
						 << "\"Locked\":" << balance["l"] << "," << "\"Total\":" << balance["t"] << "},";
			accountReply << tempBalance.str();			

            // Update database with new balance
            util::MySqlStore mySqlStore;
			std::cout << "[DB - INSERT ]" << uid << ' ' << "BINANCE" << ' ' << symbol << ' ' << balance["t"];
            mySqlStore.updateAccountBalance(uid, "BINANCE", symbol, balance["t"]);
		}
    }

	// Add [] to the JSON stream and remove the last ,
	accountReply.seekp(-1, std::ios_base::end) << "]}";

    return accountReply.str();
}

std::string Engine::postSendOrderProcessing(const std::string& ctRequestId)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

    element["symbol"]   	= exchangeAnswer["symbol"].asString();
    element["orderId"]  	= exchangeAnswer["orderId"].asString();
    element["status"]   	= exchangeAnswer["status"].asString();
    element["price"]    	= exchangeAnswer["symbol"];
    element["origQty"] 		= exchangeAnswer["origQty"];
    element["side"]     	= exchangeAnswer["side"];
    element["type"]     	= exchangeAnswer["type"];
    element["ctOrderIdO"] 	= exchangeAnswer["clientOrderId"];

    root["ctRequestId"] = ctRequestId;
	root["reply"] = element;

	root["msg"] 		= exchangeAnswer["msg"];

    std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);
}

std::string Engine::postGetRecentTradesProcessing(const std::string& ctRequestId, const std::string& symbol)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

	unsigned int i(0);

	for (i = 0; i< exchangeAnswer.size(); i++) {
		element[i]["price"]		= exchangeAnswer[i]["price"];
		element[i]["amount"]	= exchangeAnswer[i]["qty"];
		element[i]["time"]		= exchangeAnswer[i]["time"];
	}
	
	root["symbol"]		= symbol;
	root["exchange"]	= "BINANCE";
    root["ctRequestId"] = ctRequestId;
    root["reply"] 		= element;

    std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);
}


std::string Engine::postProcessing(const std::string& ctRequestId)
{
    std::stringstream orderReply;
    
    if (exchangeAnswer.isMember(CLIENTORDER_ID) && exchangeAnswer.isMember(ORDER_ID))
    {
        const std::string clientOrderId   		= exchangeAnswer[CLIENTORDER_ID].asString();
        const std::string orderId              	= exchangeAnswer[ORDER_ID].asString();
        const std::string status          		= exchangeAnswer["status"].asString();
		const std::string cummulativeQuoteQty	= exchangeAnswer["cummulativeQuoteQty"].asString();
		const std::string executedQty			= exchangeAnswer["executedQty"].asString();
		const Json::Value fills					= exchangeAnswer["fills"];
		const std::string price					= exchangeAnswer["price"].asString();
		const long int transacTime				= exchangeAnswer["transactTime"].asLargestUInt();
          
		// Check if server replies to the same ctRequestId sent from our side
        if (clientOrderId != ctRequestId and exchangeAnswer["status"].asString() != "CANCELLED") 
            std::cout << "[ERROR]: Server clientOrderId: " << clientOrderId << " vs ctRequestId: " << ctRequestId << std::endl;

        orderReply << "{\"ctRequestId\":\"" << ctRequestId << "\"," << "\"clientOrderId\":\"" << clientOrderId << "\"," << "\"orderId\":" << orderId <<"," 
					<< "\"status\":\"" << status << "\"," << "\"cummulativeQuoteQty\":\"" << cummulativeQuoteQty  << "\"," 
					<< "\"executedQty\":\"" << executedQty << "\"," << "\"price\":\"" << price << "\","
					<< "\"transacTime\":" << transacTime << "," <<  "\"fills\":" << fills << "}";

        util::MySqlStore mySqlStore;
        mySqlStore.saveOrderAnswer(ctRequestId, status, orderId, clientOrderId);
    }
    else
    {
        int error_code  = exchangeAnswer["code"].asInt();
        std::string msg = exchangeAnswer["msg"].asString();

        orderReply << "{\"ctRequestId\":\"" << ctRequestId << "\"," << "\"clientOrderId\":\"" << ctRequestId << "\"," << "\"code\":" << error_code <<"," << "\"msg\":\"" << msg << "\"" << "}";

        util::MySqlStore mySqlStore;
        mySqlStore.saveOrderAnswerError(ctRequestId, error_code, msg);
    }    
    
    return orderReply.str();
}


/******************************************************************************/
using namespace Trading::Binance::order;
// Placing a LIMIT order
std::string SendLimitOrder::process(const ParseRequest& parseRequest)
{    
	std::string tif = "GTC";

    std::cout << "[send_order::SendLimitOrder] ctRequestId: " << parseRequest.ctRequestId << " quantity: " << parseRequest.quantity << " price:" << parseRequest.price << std::endl;
	
	if ( !parseRequest.timeInForce.empty() )
			tif = parseRequest.timeInForce;

    // Place MARKET and LIMIT order. There is no difference between market and limit except the price for market is ""
    binaCPP.send_order(parseRequest.symbol.c_str(), parseRequest.orderSide.c_str(), "LIMIT", tif.c_str(), parseRequest.quantity, parseRequest.price, parseRequest.ctRequestId.c_str(), 0, 0, recvWindow, exchangeAnswer);
    std::cout << exchangeAnswer << std::endl;

	// Save request to DB
    util::MySqlStore mySqlStore;
    mySqlStore.saveOrderRequest(parseRequest.ctRequestId, parseRequest.requestType, parseRequest.subType,  parseRequest.userID, parseRequest.exchange, parseRequest.symbol, parseRequest.orderSide,parseRequest.quantity, parseRequest.price, parseRequest.timeInForce);     

    return Engine::postSendOrderProcessing(parseRequest.ctRequestId.c_str());
}

// Placing a MARKET order
std::string SendMarketOrder::process(const ParseRequest& parseRequest)
{    
    std::cout << "[send_order::SendMarketOrder] ctRequestId:" << parseRequest.ctRequestId << " quantity: " << parseRequest.quantity << " price:" << parseRequest.price << std::endl;
    
    binaCPP.send_order_market(parseRequest.symbol.c_str(), parseRequest.orderSide.c_str(), "MARKET", parseRequest.quantity, parseRequest.ctRequestId.c_str(), 0, 0, recvWindow, exchangeAnswer);
    //BinaCPP::send_order_market( "BNBBTC", "BUY", "MARKET", 1.6, "",0,0, recvWindow, exchangeAnswer);

	std::cout << exchangeAnswer << std::endl;

	util::MySqlStore mySqlStore;
	mySqlStore.saveOrderRequest(parseRequest.ctRequestId, parseRequest.requestType, "MARKET", parseRequest.userID, parseRequest.exchange, parseRequest.symbol, parseRequest.orderSide,parseRequest.quantity, 0.0, "");

    return Engine::postSendOrderProcessing(parseRequest.ctRequestId.c_str());
}

// Placing an Iceberg order
std::string SendIcebergOrder::process(const ParseRequest& parseRequest)
{
    std::cout << "[send_order::SendIcebergOrder] ctRequestId:" << parseRequest.ctRequestId << " quantity:" << parseRequest.quantity << " IcebergQty:" << parseRequest.icebergQty << std::endl;
    
    //BinaCPP::send_order_market(parseRequest.symbol.c_str(), parseRequest.orderSide.c_str(), "MARKET", parseRequest.quantity, parseRequest.ctRequestId.c_str(), 0, parseRequest.icebergQty, recvWindow, exchangeAnswer);
	binaCPP.send_order_market("BNBBTC", "BUY","MARKET",0.3,"",0,2,recvWindow,exchangeAnswer);
    std::cout << exchangeAnswer << std::endl;

	//util::MySqlStore mySqlStore;
	//mySqlStore.saveOrderRequest(parseRequest.ctRequestId, parseRequest.requestType, parseRequest.userID, parseRequest.exchange, parseRequest.symbol, parseRequest.orderSide,parseRequest.subType,parseRequest.quantity, "", "");

    return Engine::postSendOrderProcessing(parseRequest.ctRequestId.c_str());
}

// Placing a TAKE_PROFIT order, a market order
std::string SendTakeProfitOrder::process(const ParseRequest& parseRequest)
{
    std::cout << "[send_order::SendTakeProfitOrder] ctRequestId:" << parseRequest.ctRequestId << " quantity: " << parseRequest.quantity << " stopPrice:" << parseRequest.stopPrice << std::endl;
    
    binaCPP.send_order_market(parseRequest.symbol.c_str(), parseRequest.orderSide.c_str(), "TAKE_PROFIT", parseRequest.quantity, parseRequest.ctRequestId.c_str(), parseRequest.stopPrice, 0, recvWindow, exchangeAnswer);
	std::cout << exchangeAnswer << std::endl;

	util::MySqlStore mySqlStore;
	mySqlStore.saveOrderRequest(parseRequest.ctRequestId, parseRequest.requestType, "TAKE_PROFIT", parseRequest.userID, parseRequest.exchange, parseRequest.symbol, parseRequest.orderSide,parseRequest.quantity, 0.0, "");

    return Engine::postSendOrderProcessing(parseRequest.ctRequestId.c_str());
}

// Placing a TAKE_PROFIT_LIMIT order
std::string SendTakeProfitLimitOrder::process(const ParseRequest& parseRequest)
{
    std::cout << "[send_order::SendTakeProfitLimitOrder] ctRequestId:" << parseRequest.ctRequestId << " quantity: " << parseRequest.quantity << " price:" << parseRequest.price << " stopPrice: " << parseRequest.stopPrice << std::endl;
    
    binaCPP.send_order(parseRequest.symbol.c_str(), parseRequest.orderSide.c_str(), "TAKE_PROFIT_LIMIT", parseRequest.timeInForce.c_str(), parseRequest.quantity, parseRequest.price, parseRequest.ctRequestId.c_str(), parseRequest.stopPrice, 0, recvWindow, exchangeAnswer);
	std::cout << exchangeAnswer << std::endl;

	util::MySqlStore mySqlStore;
	mySqlStore.saveOrderRequest(parseRequest.ctRequestId, parseRequest.requestType, parseRequest.subType, parseRequest.userID, parseRequest.exchange, parseRequest.symbol, parseRequest.orderSide,parseRequest.quantity, 0.0, "");

    return Engine::postSendOrderProcessing(parseRequest.ctRequestId.c_str());
}

// Placing a Stop_Loss order
std::string SendStopLossOrder::process(const ParseRequest& parseRequest)
{
    std::cout << "[send_order::SendStopLossOrder] ctRequestId:" << parseRequest.ctRequestId << " quantity: " << parseRequest.quantity << " stopPrice:" << parseRequest.stopPrice << std::endl;
    
    binaCPP.send_order_market(parseRequest.symbol.c_str(), parseRequest.orderSide.c_str(), "STOP_LOSS", parseRequest.quantity, parseRequest.ctRequestId.c_str(), parseRequest.stopPrice, 0, recvWindow, exchangeAnswer);
	std::cout << exchangeAnswer << std::endl;

	util::MySqlStore mySqlStore;
	mySqlStore.saveOrderRequest(parseRequest.ctRequestId, parseRequest.requestType, "STOP_LOSS", parseRequest.userID, parseRequest.exchange, parseRequest.symbol, parseRequest.orderSide,parseRequest.quantity, 0.0, "");

    return Engine::postSendOrderProcessing(parseRequest.ctRequestId.c_str());
}

// Placing a Stop_Loss_Limit order
std::string SendStopLossLimitOrder::process(const ParseRequest& parseRequest)
{
    std::cout << "[send_order::SendStopLossLimitOrder] ctRequestId:" << parseRequest.ctRequestId << " quantity: " << parseRequest.quantity << " price:" << parseRequest.price << " stopPrice: " << parseRequest.stopPrice << std::endl;
    
    binaCPP.send_order(parseRequest.symbol.c_str(), parseRequest.orderSide.c_str(), "STOP_LOSS_LIMIT", parseRequest.timeInForce.c_str(), parseRequest.quantity, parseRequest.price, parseRequest.ctRequestId.c_str(), parseRequest.stopPrice, 0, recvWindow, exchangeAnswer);
	std::cout << exchangeAnswer << std::endl;

	util::MySqlStore mySqlStore;
	mySqlStore.saveOrderRequest(parseRequest.ctRequestId, parseRequest.requestType, "STOP_LOSS_LIMIT", parseRequest.userID, parseRequest.exchange, parseRequest.symbol, parseRequest.orderSide,parseRequest.quantity, 0.0, "");

    return Engine::postSendOrderProcessing(parseRequest.ctRequestId.c_str());
}

// Placing a Limit_Maker order
std::string SendLimitMakerOrder::process(const ParseRequest& parseRequest)
{
    std::cout << "[send_order::SendLimitMakerOrder] ctRequestId:" << parseRequest.ctRequestId << " quantity: " << parseRequest.quantity << " price:" << parseRequest.price << std::endl;
    
	binaCPP.send_order(parseRequest.symbol.c_str(), parseRequest.orderSide.c_str(), "LIMIT_MAKER", "GTC", parseRequest.quantity, parseRequest.price, parseRequest.ctRequestId.c_str(), 0, 0, recvWindow, exchangeAnswer);    
	std::cout << exchangeAnswer << std::endl;

	util::MySqlStore mySqlStore;
	mySqlStore.saveOrderRequest(parseRequest.ctRequestId, parseRequest.requestType, "LIMIT_MAKER", parseRequest.userID, parseRequest.exchange, parseRequest.symbol, parseRequest.orderSide,parseRequest.quantity, 0.0, "");

    return Engine::postSendOrderProcessing(parseRequest.ctRequestId.c_str());
}

    
std::string CancelOneOrder::process(const ParseRequest& parseRequest)
{    
    std::cout << "[order::CancelOneOrder] " << "Symbol: " << parseRequest.symbol << " ctRequestId: " << parseRequest.ctRequestId << std::endl;

	if (! parseRequest.orderId.empty())
	{
		binaCPP.cancel_order(parseRequest.symbol.c_str(), parseRequest.orderId.c_str() , "", "", recvWindow, exchangeAnswer);
	} else if (! parseRequest.clientOrderId.empty())
	{
		binaCPP.cancel_order(parseRequest.symbol.c_str(), 0, parseRequest.clientOrderId.c_str() , "", recvWindow, exchangeAnswer);
	} else {
		std::cout << "Must provide either orderId or clientOrderId" << std::endl;
	}

    std::cout << exchangeAnswer << std::endl;

    return Engine::postCancelOneOrderProcessing(parseRequest.ctRequestId.c_str());
}

std::string CancelAllOrders::process(const ParseRequest& parseRequest)
{
	Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;
	int idx = 0;

	std::stringstream msg;
	std::multimap<std::string, std::string> orderIdMap;
    std::cout << "[order::CancelAllOrders]" << "Symbol: " << parseRequest.symbol << " ctRequestId: " << parseRequest.ctRequestId << std::endl;
	
	if (parseRequest.symbol.empty()){

            msg << "{ \"msg\":\"" << "\"INVALID SYMBOL\"}";
			std::cout << msg.str() << std::endl;
			return msg.str();
	}


    Json::Value openOrders;
    binaCPP.get_openOrders(parseRequest.symbol.c_str(), recvWindow, openOrders);

    for (Json::Value::ArrayIndex i = 0; i < openOrders.size(); i++)
    {
        orderIdMap.insert({openOrders[i]["symbol"].asString(), openOrders[i]["clientOrderId"].asString()});   
		std::cout << "symbol: " << openOrders[i]["symbol"].asString() << " orderID: " << openOrders[i]["clientOrderId"].asString() <<  std::endl;
    }
	
    for (auto const& itr : orderIdMap)
    {
        std::cout << itr.first << "\t" << itr.second << std::endl;
        binaCPP.cancel_order(itr.first.c_str(), 0, itr.second.c_str(), "", recvWindow, exchangeAnswer);
		
		element[idx]["orderId"]            = exchangeAnswer["orderId"].asString();
    	element[idx]["status"]             = exchangeAnswer["status"].asString();
    	element[idx]["clientOrderId"]      = exchangeAnswer["clientOrderId"].asString();
		idx++;
        std::cout << exchangeAnswer << std::endl;
    }
	
	root["ctRequestId"] = parseRequest.ctRequestId;
	root["reply"] = element;

	std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);

    //return Engine::postCancelAllOrdersProcessing(parseRequest.ctRequestId.c_str());
}

std::string CancelAllLimitOrders::process(const ParseRequest& parseRequest)
{
    std::multimap<std::string, std::string> orderIdMap;
    std::cout << "[order::CancelAllLimitOrders]" << "Symbol: " << parseRequest.symbol << std::endl;

    Json::Value openOrders;
    binaCPP.get_openOrders(parseRequest.symbol.c_str(), recvWindow, openOrders);

    for (Json::Value::ArrayIndex i =0; i < openOrders.size(); i++)
    {
        if (openOrders[i]["type"].asString() == "LIMIT")
            orderIdMap.insert({openOrders[i]["symbol"].asString(), openOrders[i]["clientOrderId"].asString()});   
            //std::cout << "symbol: " << openOrders[i]["symbol"].asString() << " orderID: " << openOrders[i]["clientOrderId"].asString() <<  std::endl;
    }

    for (auto const& itr : orderIdMap)
    {
        std::cout << itr.first << "\t" << itr.second << std::endl;
        binaCPP.cancel_order(itr.first.c_str(), 0, itr.second.c_str(), "", recvWindow, exchangeAnswer);
        std::cout << exchangeAnswer << std::endl;
    }

    return "CancelAllLimitOrders completed.";

}

std::string CancelAllStopLimitOrders::process(const ParseRequest& parseRequest)
{
    std::multimap<std::string, std::string> orderIdMap;
    std::cout << "[order::CancelAllStopLimitOrders]" << "Symbol: " << parseRequest.symbol << std::endl;

    Json::Value openOrders;
    binaCPP.get_openOrders(parseRequest.symbol.c_str(), recvWindow, openOrders);

    for (Json::Value::ArrayIndex i =0; i < openOrders.size(); i++)
    {
        if (openOrders[i]["type"].asString() == "TAKE_PROFIT_LIMIT" || openOrders[i]["type"].asString() == "STOP_LOSS_LIMIT")
            orderIdMap.insert({openOrders[i]["symbol"].asString(), openOrders[i]["clientOrderId"].asString()});   
            //std::cout << "symbol: " << openOrders[i]["symbol"].asString() << " orderID: " << openOrders[i]["clientOrderId"].asString() <<  std::endl;
    }

    for (auto const& itr : orderIdMap)
    {
        std::cout << itr.first << "\t" << itr.second << std::endl;
        binaCPP.cancel_order(itr.first.c_str(), 0, itr.second.c_str(), "", recvWindow, exchangeAnswer);
        std::cout << exchangeAnswer << std::endl;
    }

    return "CancelAllLimitOrders completed.";
}


// Check an order's status
std::string GetOrderStatus::process(const ParseRequest& parseRequest)
{   
    std::cout << "[order::GetOrderStatus]" << "Symbol: " << parseRequest.symbol << "ctRequestId: " << parseRequest.ctRequestId << std::endl;

    if ( !parseRequest.orderId.empty()  ) {
        binaCPP.get_order(parseRequest.symbol.c_str(), parseRequest.orderId.c_str(), "", recvWindow, exchangeAnswer);
    } else if (!parseRequest.ctRequestId.empty())
	{
        binaCPP.get_order(parseRequest.symbol.c_str(), 0, parseRequest.ctRequestId.c_str(), recvWindow, exchangeAnswer);
    } else
    {
        std::cout << "[ERROR]: either orderID or ctRequestId(origClientOrderID must be provided.";
        return "{\"code\": -9, \"msg\":, \"Either orderID or ctRequestId (origClientOrderID) MUST be provided.\"}";
    }
    
	std::cout << exchangeAnswer << std::endl;

    util::MySqlStore mySqlStore;    
	mySqlStore.saveOrderRequest(parseRequest.ctRequestId, parseRequest.requestType, parseRequest.subType, parseRequest.userID, parseRequest.exchange, parseRequest.symbol, parseRequest.orderSide,0.0, 0.0, "");

    //Test Case: binaCPP.get_order("BNBBTC",0,"f2b320d2-9aef-4794-b134-f61ced3acb7d",recvWindow,exchangeAnswer);
	return Engine::postGetOrderStatusProcessing(parseRequest.ctRequestId.c_str());
}

// Getting list of open orders for specific pair
std::string GetAllOpenOrders::process(const ParseRequest& parseRequest)
{
    std::cout << "[get_order::GetAllOpenOrders]" << "Symbol: " << parseRequest.symbol << std::endl;
    std::string symbol("BNBBTC");

	if (!parseRequest.symbol.empty()) 
        symbol = parseRequest.symbol;

    binaCPP.get_openOrders(symbol.c_str(), recvWindow, exchangeAnswer);
	std::cout << exchangeAnswer << std::endl;
    
    return Engine::postGetOpenOrdersProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetAllOrders::process(const ParseRequest& parseRequest)
{
    std::cout << "[get_order::Get_AllOrders]" << "Symbol: " << parseRequest.symbol << std::endl;
    std::string symbol("BNBBTC");

    if (!parseRequest.symbol.empty()) 
		symbol = parseRequest.symbol;

    binaCPP.get_allOrders(symbol.c_str(), parseRequest.fromId.c_str(), parseRequest.limit, recvWindow, exchangeAnswer);
	std::cout << exchangeAnswer << std::endl;
    
	return Engine::postGetAllOrdersProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetTradeHistory::process(const ParseRequest& parseRequest)
{
    std::cout << "[get_order::GetTradeHistory]" << "Symbol: " << parseRequest.symbol << std::endl;
    std::string symbol("BNBBTC");

    if (!parseRequest.symbol.empty()) 
			symbol = parseRequest.symbol;

    binaCPP.get_myTrades(symbol.c_str(), parseRequest.limit, parseRequest.fromId.c_str(), recvWindow, exchangeAnswer);
	std::cout << exchangeAnswer << std::endl;
    
	return Engine::postGetTradeHistoryProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetOrderBook::process(const ParseRequest& parseRequest)
{
    std::cout << "[get_order::GetOrderBook]" << ": requestId: " << parseRequest.ctRequestId << " limit: " << parseRequest.limit << std::endl;
	
	if (!parseRequest.symbol.empty() )
	{
    	binaCPP.get_depth(parseRequest.symbol.c_str(), 20, exchangeAnswer);
	} else 
	{
		binaCPP.get_depth("BNBBTC", 20, exchangeAnswer);
	}
	
	std::cout << exchangeAnswer << std::endl;

	return Engine::postGetOrderBookProcessing(parseRequest.ctRequestId.c_str(), parseRequest.symbol.c_str());
}

std::string GetRecentTrades::process(const ParseRequest& parseRequest)
{
    std::cout << "[get_order::GetRecentTrades]" << ": requestId: " << parseRequest.ctRequestId << " limit: " << parseRequest.limit << std::endl;

    if (!parseRequest.symbol.empty() )
    {
        binaCPP.get_recentTrades(parseRequest.symbol.c_str(), 20, exchangeAnswer);
    } else
    {
        binaCPP.get_depth("BNBBTC", 20, exchangeAnswer);
    }

    std::cout << exchangeAnswer << std::endl;

    return Engine::postGetRecentTradesProcessing(parseRequest.ctRequestId.c_str(), parseRequest.symbol.c_str());
}

/******************************************************************************/
using namespace Trading::Binance::generic;

std::string ServerTime::process(const ParseRequest& parseRequest)
{    
    binaCPP.get_serverTime(exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;    

    std::string serverTime;
    if (exchangeAnswer.isMember("serverTime"))
        serverTime = exchangeAnswer["serverTime"].asString();
    else
        serverTime = "[ERROR]: serverTime.";    

    return serverTime;
}


std::string GetTicker::process(const ParseRequest& parseRequest)
{
    std::cout << "[generic: GetTicker]: " << parseRequest.symbol.c_str(); 

	binaCPP.get_price(parseRequest.symbol.c_str(), exchangeAnswer);

    std::cout << "symbol: " << parseRequest.symbol.c_str() << " price: " << exchangeAnswer << std::endl;

    return Engine::postGetTickerProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetMarket::process(const ParseRequest& parseRequest)
{
    std::cout << "[generic: GetMarket]: " << parseRequest.symbol.c_str(); 

	binaCPP.get_allPrices(exchangeAnswer);

    std::cout <<  exchangeAnswer << std::endl;

    return Engine::postGetMarketProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetTradingPairs::process(const ParseRequest& parseRequest)
{
    std::cout << "[generic: GetTradingPairs]: " << parseRequest.symbol.c_str();

    binaCPP.get_tradingPairs(exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}


/******************************************************************************/
using namespace Trading::Binance::account;
std::string GetBalance::process(const ParseRequest& parseRequest)
{
	std::cout << "[account: GetBalance] " << "ctRequestId: " << parseRequest.ctRequestId.c_str() << std::endl;
    binaCPP.get_account(recvWindow, exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

	return Engine::postGetBalanceProcessing(parseRequest.ctRequestId.c_str(), parseRequest.userID);
	//return Engine::postRequestProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetWithdrawHistory::process(const ParseRequest& parseRequest)
{
    std::cout << "[account: GetWithdrawHistory]: " << parseRequest.symbol.c_str();

    binaCPP.get_withdrawHistory( parseRequest.symbol.c_str(),0,0,0, recvWindow, exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;
	
	util::MySqlStore mySqlStore;
    //mySqlStore.saveWithdrawHistory(std::to_string(parseRequest.userID), "BINANCE", exchangeAnswer["depositList"]["asset"].asString(), exchangeAnswer["depositList"]["amount"].asDouble(), exchangeAnswer["depositList"]["time"].asString());
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetDepositHistory::process(const ParseRequest& parseRequest)
{
    std::cout << "[account: GetDepositHistory]: " << parseRequest.symbol.c_str();

    binaCPP.get_depositHistory(parseRequest.symbol.c_str(), 0,0,0, recvWindow, exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;
		
	std::string uid 	= std::to_string(parseRequest.userID);
	std::string asset, txid;
	double amount;
	long long time;
	std::time_t t;
	std::stringstream sstime;
	
	for (unsigned int i = 0; i < exchangeAnswer["depositList"].size(); i++){
		asset  = exchangeAnswer["depositList"][i]["asset"].asString();
    	amount = exchangeAnswer["depositList"][i]["amount"].asDouble();
    	time   = exchangeAnswer["depositList"][i]["insertTime"].asLargestInt()/1000;
		txid   = exchangeAnswer["depositList"][i]["txId"].asString();

		t = time;
		sstime << std::put_time(std::gmtime(&t), "%F") << '\n';

		//ss << std::put_time(std::localtime(&t), "%c %Z");
		//std::cout << "stime: " << ss << std::endl;
	}


	std::cout << "[DB: Insert]: " << "uid: " << uid << " BINANCE" << " asset: " << asset << " amount: " << amount << " time: " << time << std::endl;

    util::MySqlStore mySqlStore;
	mySqlStore.saveDepositHistory(uid, "BINANCE", asset, amount, sstime.str(), txid);
    
	return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetPortfolio::process(const ParseRequest& parseRequest)
{
    std::string funding_date;
    std::string symbol;
    util::MySqlStore mySqlStore;

    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

    std::cout << "[account: GetPortfolio]: " << parseRequest.symbol.c_str();

    // 1. If blance > 0, check deposit history. If coin is deposited, use deposit price as history price
    // 2. If blance > 0, and no deposit hisotry then coin is purchased. Then use recent trades weighted price for cost 
    // Get account blance
    binaCPP.get_account(recvWindow, exchangeAnswer);

    // Find non-zero balance
    /*unsigned int i = 0;
    double histPrice = 0;
    std::stringstream accountReply;
    std::map<std::string, std::map<std::string, double> > balance;       // < symbol, <balance_type, balance_amount> > 
    std::map<std::string, std::pair<double, double> > portfolio;		// < symbol, <balance_amount, average_price>>

    for (i = 0; i < exchangeAnswer["balances"].size(); i++)
    {
        if (exchangeAnswer["balances"][i]["free"].asString() != "0.00000000" || exchangeAnswer["balances"][i]["locked"].asString() != "0.00000000"){
            symbol = exchangeAnswer["balances"][i]["asset"].asString().c_str();
            balance[symbol]["f"] = atof(exchangeAnswer["balances"][i]["free"].asString().c_str());
            balance[symbol]["l"] = atof(exchangeAnswer["balances"][i]["locked"].asString().c_str()); 
            balance[symbol]["t"] = balance[symbol]["f"] + balance[symbol]["l"];
        }  

    }

    // Get price from funding history
    for (auto itr = balance.begin(); itr != balance.end(); ++itr)
    {
		// Check if the symbol is funded through deposit
		// Check assets available both in balance and in the funding history
		// If YES, get funding date and use the history price at funding date for cost
		// If NO, check if it is from trading activity
        symbol = itr->first;
        funding_date = mySqlStore.selectFundingDate(to_string(parseRequest.userID), "BINANCE", symbol);
        histPrice = mySqlStore.selectHistoryPrice(symbol, funding_date);
        
        std::cout << "Symbol: " << symbol << " History price: " << histPrice << std::endl;
		portfolio.insert( make_pair(symbol, make_pair(1.0, histPrice)) );
    }
	
	// Print portfolio for test
	for ( auto const &out_itr : portfolio ){
	 	std::cout << "Symbol: " << symbol << "Balance: " << balance[symbol]["t"] <<" History price: " << histPrice << std::endl;
		element[out_itr.first]["amount"]		=	(out_itr.second).first;
		element[out_itr.first]["average_cost"]  =	(out_itr.second).second;
	}
	
	std::cout << "Portfolio called" << std::endl;*/
	
	unsigned int i(0), index(0);
	double total;
	std::string asset, free, locked;
	
	mySqlStore.clearAccountBalance(parseRequest.userID, "BINANCE");

	for (i = 0; i < exchangeAnswer.size(); i++){
		asset 	= exchangeAnswer["balances"][i]["asset"].asString();
		free 	= exchangeAnswer["balances"][i]["free"].asString();
		locked	= exchangeAnswer["balances"][i]["locked"].asString();
		total 	= std::stof(free) + std::stof(locked);

		if ( free != "0.00000000" || locked != "0.00000000" ) {
			element[index]["symbol"]		= exchangeAnswer["balances"][i]["asset"];
			element[index]["average_cost"]	= std::to_string(total);
			element[index]["total_amount"]  = std::to_string(total);
			element[index]["total_cost"]    = std::to_string(total);

			mySqlStore.updateAccountBalance(parseRequest.userID, "BINANCE", asset, total);
			index++;
		}

	}	

	root["ctRequestId"]             = parseRequest.ctRequestId;
    root["reply"]                   = element;

	std::cout << "[BINANCE: GetPortfolio]" << root << std::endl;

    return fastWriter.write(root);
}
