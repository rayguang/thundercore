#include "bittrex.h"
#include "mySqlStore.h"

#include <bitxcpp_websocket.h>

#include <json/json.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <Poco/Bugcheck.h>

#include <iostream>
#include <boost/lexical_cast.hpp>
/******************************************************************************/
using namespace Trading::Bittrex;

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

    return Exchange::init(parseRequest) && bitxCPP.init(key,secret);
}

#define ORDER_ID "orderId"
#define CLIENTORDER_ID "clientOrderId"
// Post API processing 
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


std::string Engine::postGetTickerProcessing(const std::string& ctRequestId, const std::string& symbol)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

    element["symbol"]   = symbol;
    element["price"]    = exchangeAnswer["result"]["Last"];
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

     for (unsigned int i = 0 ; i < exchangeAnswer["result"].size() ; i++)
    {
        element[i]["Symbol"]    =   exchangeAnswer["result"][i]["Currency"];
        element[i]["Free"]      =   exchangeAnswer["result"][i]["Available"];
        element[i]["Locked"]    =   exchangeAnswer["result"][i]["Pending"];
        element[i]["Total"]     =   exchangeAnswer["result"][i]["Balance"]; 

        util::MySqlStore mySqlStore;
		std::cout << "[DB - INSERT ]" << uid << ' ' << "BINANCE" << ' ' << element[i]["Symbol"] << ' ' << element[i]["Total"];
        mySqlStore.updateAccountBalance(uid, "BITTREX", element[i]["Symbol"].asString(), element[i]["Total"].asDouble());

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
    root["orderId"]     = exchangeAnswer["result"]["uuid"];              
    root["msg"]     	= exchangeAnswer["message"];

    std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);
}

std::string Engine::postGetOrderStatusProcessing(const std::string& ctRequestId)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

    element["orderId"]         = exchangeAnswer["result"]["OrderUuid"];
    element["symbol"]          = exchangeAnswer["result"]["Exchange"];
    element["price"]           = exchangeAnswer["result"]["Limit"];
    element["origQty"]         = exchangeAnswer["result"]["Quantity"];
    element["ctOrderId"]       = exchangeAnswer["result"]["OrderUuid"];
	element["time"]			   = exchangeAnswer["result"]["Opened"];
	element["updateTime"]	   = exchangeAnswer["result"]["Opened"];

    if (exchangeAnswer["result"]["IsOpen"].asBool()){
        element["status"]          = "NEW";
    }
    
    if (exchangeAnswer["Type"] == "LIMIT_SELL"){
        element["side"]        = "SELL";
        element["type"]        = "LIMIT";
    } else if (exchangeAnswer["Type"] == "LIMIT_BUY"){
        element["side"]        = "BUY";
        element["type"]        = "LIMIT";
    } else
    {
        element["side"]        = "NA";
        element["type"]        = "NA";
    }
    
   
    root["ctRequestId"] = ctRequestId;
    root["msg"]	 	    = exchangeAnswer["message"];
    root["reply"]       = element;

    std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);
}

std::string Engine::postCancelOneOrderProcessing(const std::string& ctRequestId, const std::string& orderId)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

    root["ctRequestId"]             = ctRequestId;             
    root["msg"] 	                = exchangeAnswer["message"];
    root["reply"]["orderId"]        = orderId;
    
    if (exchangeAnswer["success"].asBool()) {
        root["reply"]["status"] = "CANCELLED";
    } else {
        root["reply"]["status"] = "FAILED";
    }
	
	root["reply"] = element;

    std::cout << "[REPLY: postCancelOneOrderProcessing]: " << root << std::endl;

    return fastWriter.write(root);
}

std::string Engine::postGetAllOpenOrdersProcessing(const std::string& ctRequestId)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

    for ( unsigned int i = 0; i < exchangeAnswer["result"].size(); i++ ){
        element[i]["orderId"]         = exchangeAnswer["result"][i]["OrderUuid"];
        element[i]["symbol"]          = exchangeAnswer["result"][i]["Exchange"];
        element[i]["price"]           = exchangeAnswer["result"][i]["Limit"];
        element[i]["origQty"]         = exchangeAnswer["result"][i]["Quantity"];
        element[i]["ctOrderId"]       = exchangeAnswer["result"][i]["OrderUuid"];
        element[i]["time"]			  = exchangeAnswer["result"][i]["Opened"];
        element[i]["updateTime"]	  = exchangeAnswer["result"][i]["Opened"];
        
        if (exchangeAnswer["result"][i]["OrderType"].asString() == "LIMIT_SELL"){
            element[i]["side"]        = "SELL";
            element[i]["type"]        = "LIMIT";
        } else if (exchangeAnswer["result"][i]["OrderType"].asString() == "LIMIT_BUY"){
            element[i]["side"]        = "BUY";
            element[i]["type"]        = "LIMIT";
        } else
        {
            element[i]["side"]        = "NA";
            element[i]["type"]        = "NA";
        }

		if ( exchangeAnswer["result"][i]["Closed"].asString() != "" ) {
			element[i]["status"]	 = "FILLED";
		}
    }

    root["ctRequestId"]             = ctRequestId;   
    root["reply"]                   = element;          
    root["msg"]  	                = exchangeAnswer["message"];

    std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);
}

std::string Engine::postGetAllOrdersProcessing(const std::string& ctRequestId)
{
	Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

    for ( unsigned int i = 0; i < exchangeAnswer["result"].size(); i++ ){
        element[i]["orderId"]         = exchangeAnswer["result"][i]["OrderUuid"];
        element[i]["symbol"]          = exchangeAnswer["result"][i]["Exchange"];
        element[i]["price"]           = exchangeAnswer["result"][i]["Limit"];
        element[i]["origQty"]         = exchangeAnswer["result"][i]["Quantity"];
        element[i]["ctOrderId"]       = exchangeAnswer["result"][i]["OrderUuid"];

		// Convert ISO 8601 datetime to YY-MM-DD 
		int y, M, d, h, m;
		float s;
		std::string date_string		  = exchangeAnswer["result"][i]["TimeStamp"].asString();
		sscanf(date_string.c_str(), "%d-%d-%dT%d%d%f", &y, &M, &d, &h, &m, &s);

        element[i]["time"]            = to_string(y) + "-" + to_string(M) + "-" + to_string(d);
        element[i]["updateTime"]      = to_string(y) + "-" + to_string(M) + "-" + to_string(d);

        if (exchangeAnswer["result"][i]["OrderType"].asString() == "LIMIT_SELL"){
            element[i]["side"]        = "SELL";
            element[i]["type"]        = "LIMIT";
        } else if (exchangeAnswer["result"][i]["OrderType"].asString() == "LIMIT_BUY"){
            element[i]["side"]        = "BUY";
            element[i]["type"]        = "LIMIT";
        } else
        {
            element[i]["side"]        = "NA";
            element[i]["type"]        = "NA";
        }
    }

    root["ctRequestId"]             = ctRequestId;   
    root["reply"]                   = element;          
    root["msg"] 	                = exchangeAnswer["message"];

    std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);
}

std::string Engine::postGetOrderBookProcessing(const std::string& ctRequestId, const std::string& symbol)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;
	
	unsigned int i(0), limit(20);
	//limit = exchangeAnswer["result"]["buy"].size();
	for (i = 0; i < limit; i++) {
		element["asks"][i][0] = exchangeAnswer["result"]["sell"][i]["Rate"];
		element["asks"][i][1] = exchangeAnswer["result"]["sell"][i]["Quantity"];

		element["bids"][i][0] = exchangeAnswer["result"]["buy"][i]["Rate"];
		element["bids"][i][1] = exchangeAnswer["result"]["buy"][i]["Quantity"];
	}
	
	root["exchange"]	= "BITTREX";
	root["symbol"]		= symbol;
    root["ctRequestId"]	= ctRequestId;
    root["reply"] 		= element;
	
	std::cout << root << std::endl;
    return fastWriter.write(root);
}

std::string Engine::postGetTradeHistoryProcessing(const std::string& ctRequestId, const int uid)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

    util::MySqlStore mySqlStore;
    mySqlStore.deleteTradeHistory_Bittrex(uid);

    for ( unsigned int i = 0; i < exchangeAnswer["result"].size(); i++ ){
        element[i]["orderId"]         = exchangeAnswer["result"][i]["OrderUuid"];
        element[i]["symbol"]          = exchangeAnswer["result"][i]["Exchange"];
        element[i]["price"]           = exchangeAnswer["result"][i]["PricePerUnit"];
        element[i]["origQty"]         = exchangeAnswer["result"][i]["Quantity"];
        element[i]["ctOrderId"]       = exchangeAnswer["result"][i]["OrderUuid"];

        // Convert ISO 8601 datetime to YY-MM-DD
		double amount, price;
		std::tm t = {};
		std::string symbol, timestamp;
		std::stringstream ss;
		std::istringstream date_string(exchangeAnswer["result"][i]["TimeStamp"].asString());
        // sscanf(date_string.c_str(), "%d-%d-%dT%d:%d:%f", &y, &M, &d, &h, &m, &s);
		if (date_string >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S")) {
			std::cout << std::put_time(&t, "%c") << "\n" << std::mktime(&t) << "\n";
		} else {
			std::cout << "[Bittrex: postGetTradeHistoryProcessing]: timestamp parsing failed.";
		}

		symbol  = exchangeAnswer["result"][i]["Exchange"].asString();
		amount 	= exchangeAnswer["result"][i]["Quantity"].asDouble();
		price 	= exchangeAnswer["result"][i]["PricePerUnit"].asDouble(); 
		
        element[i]["time"]            = boost::lexical_cast<std::string>(std::mktime(&t));
		timestamp					  = boost::lexical_cast<std::string>(std::mktime(&t));

        if (exchangeAnswer["result"][i]["OrderType"].asString() == "LIMIT_SELL"){
            element[i]["side"]        = "SELL";
            element[i]["type"]        = "LIMIT";
        } else if (exchangeAnswer["result"][i]["OrderType"].asString() == "LIMIT_BUY"){
            element[i]["side"]        = "BUY";
            element[i]["type"]        = "LIMIT";
        } else
        {
            element[i]["side"]        = "NA";
            element[i]["type"]        = "NA";
        }
		
		// std::cout << "symbol: " << symbol << " amount: " << amount << " price: " << price << " type: " << element[i]["type"].asString() << " order_id: " << element[i]["orderId"].asString() << " fee: " << exchangeAnswer["result"][i]["Commission"].asDouble() << " fee_currency: " << symbol.substr(0,3) << " timestamp: " << timestamp << std::endl; 
 
        mySqlStore.updateTradeHistory_Bittrex(uid, symbol, amount, price, element[i]["type"].asString(), element[i]["orderId"].asString(), element[i]["orderId"].asString(), exchangeAnswer["result"][i]["Commission"].asDouble(),symbol.substr(0,3), timestamp);

    }

    root["ctRequestId"]             = ctRequestId;
    root["reply"]                   = element;
    root["msg"] 	                = exchangeAnswer["message"];

    std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);
}


std::string Engine::postGetPortfolioProcessing(const std::string& ctRequestId, const int uid, Json::Value exchangeAnswer_balance)
{
	Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element, balance;
    std::string symbol, asset, total, base;
	util::MySqlStore mySqlStore;

    unsigned int i(0), j(0), index(0);
    double available_amount, balance_amount;
	
	mySqlStore.clearAccountBalance(uid, "BITTREX");

	for ( i = 0; i < exchangeAnswer_balance["result"].size(); i ++){
        available_amount   = exchangeAnswer_balance["result"][i]["Available"].asDouble();
        balance_amount     = exchangeAnswer_balance["result"][i]["Balance"].asDouble();
		symbol 			   = exchangeAnswer_balance["result"][i]["Currency"].asString();

        if ( available_amount > 0 || balance_amount > 0) {
			mySqlStore.updateAccountBalance(uid, "BITTREX", symbol, balance_amount);
           for ( j = 0 ; j < exchangeAnswer.size(); j++) {
		   		asset = exchangeAnswer[j]["asset"].asString().substr(0,3);
				base  = exchangeAnswer[j]["asset"].asString().substr(4,3);
		 		
			   if (base == symbol) {
			   		element[index]["symbol"]		= base;
					element[index]["average_cost"]	= exchangeAnswer[j]["average_cost"].asString();
					element[index]["total_amount"]  = to_string(balance_amount);
					element[index]["total_cost"]    = to_string(std::stof(exchangeAnswer[j]["average_cost"].asString()) * balance_amount);

					index++;
			   } else {
			   		element[index]["symbol"]        = symbol;
                    element[index]["average_cost"]  = exchangeAnswer_balance["result"][i]["Balance"].asString();;
                    element[index]["total_amount"]  = exchangeAnswer_balance["result"][i]["Balance"].asString();
					index++;
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
using namespace Trading::Bittrex::generic;

std::string GetMarkets::process(const ParseRequest& parseRequest)
{
    bitxCPP.get_markets(exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetCurrencies::process(const ParseRequest& parseRequest)
{    
    bitxCPP.get_currencies(exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetTicker::process(const ParseRequest& parseRequest)
{    
    std::cout << "[generic::GetTicker]: " << "symbol: " << parseRequest.symbol.c_str();
    
    bitxCPP.get_ticker(parseRequest.symbol.c_str(), exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postGetTickerProcessing(parseRequest.ctRequestId.c_str(), parseRequest.symbol.c_str());
}

std::string GetMarketSummaries::process(const ParseRequest& parseRequest)
{        
    bitxCPP.get_marketsummaries(exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetMarketSummary::process(const ParseRequest& parseRequest)
{        
    std::cout << "[generic::GetMarketSummary]: " << "symbol: " << parseRequest.symbol.c_str();

    bitxCPP.get_marketsummary(parseRequest.symbol.c_str(), exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetMarketHistory::process(const ParseRequest& parseRequest)
{        
    std::cout << "[generic::GetMarketHistory]: " << "symbol: " << parseRequest.symbol.c_str();

    bitxCPP.get_marketHistory(parseRequest.symbol.c_str(), exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

/******************************************************************************/
// Private endpoints
// GET /account/getbalance
// Used to retrieve the balance from your account for a specific currency.
using namespace Trading::Bittrex::account;

std::string GetAssetBalance::process(const ParseRequest& parseRequest)
{        
    std::cout << "[account::GetAssetBalance]: " << "symbol: " << parseRequest.symbol.c_str();

    bitxCPP.get_balance(parseRequest.symbol.c_str(), exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postGetBalanceProcessing(parseRequest.ctRequestId.c_str(), parseRequest.userID);
}

// GET /account/getbalances
// Used to retrieve all balances from your account.
std::string GetBalance::process(const ParseRequest& parseRequest)
{        
    //std::cout << "[account::GetBalance]: " << "symbol: " << parseRequest.symbol.c_str();

    bitxCPP.get_balances(exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postGetBalanceProcessing(parseRequest.ctRequestId.c_str(), parseRequest.userID);
}

// GET /account/getdepositaddress
// Used to retrieve or generate an address for a specific currency. 
// If one does not exist, the call will fail and return ADDRESS_GENERATING until one is available.
std::string GetDepositAddress::process(const ParseRequest& parseRequest)
{        
    std::cout << "[account::GetDepositAddress]: " << "symbol: " << parseRequest.symbol.c_str();

    bitxCPP.get_depositAddress(parseRequest.symbol.c_str(), exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// GET /account/withdraw
// Used to withdraw funds from your account. Note: please account for txfee.
std::string Withdraw::process(const ParseRequest& parseRequest)
{        
    std::cout << "[account::Withdraw]: " << "symbol: " << parseRequest.symbol.c_str() << " quantity: " << parseRequest.quantity << "  address " << parseRequest.walletaddr <<  std::endl;

    bitxCPP.withdraw(parseRequest.symbol.c_str(), parseRequest.quantity, parseRequest.walletaddr.c_str(), exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// GET /account/getwithdrawalhistory
// Used to retrieve your withdrawal history.
std::string GetWithdrawHistory::process(const ParseRequest& parseRequest)
{        
    std::cout << "[account::GetWithdrawalHistory]: " << std::endl;

    bitxCPP.get_withdrawalHistory(parseRequest.symbol.c_str(), exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}


// GET /account/getdeposithistory
// Used to retrieve your deposit history.
std::string GetDepositHistory::process(const ParseRequest& parseRequest)
{        
    std::cout << "[account::GetDepositHistory]: " << std::endl;

    bitxCPP.get_depositHistory(parseRequest.symbol.c_str(), exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    std::string uid 	= std::to_string(parseRequest.userID);
    std::string asset, txid, date;
	double amount;
	std::stringstream sstime;
	
	for (unsigned int i = 0; i < exchangeAnswer["result"].size(); i++){
		asset  = exchangeAnswer["result"][i]["Currency"].asString();
    	amount = exchangeAnswer["result"][i]["Amount"].asDouble();
        txid   = exchangeAnswer["result"][i]["TxId"].asString();
    	
		int y, M, d, h, m;
        float s;
        std::string date_string       = exchangeAnswer["result"][i]["LastUpdated"].asString();
        sscanf(date_string.c_str(), "%d-%d-%dT%d%d%f", &y, &M, &d, &h, &m, &s);

        date = to_string(y) + "-" + to_string(M) + "-" + to_string(d);
	}


	std::cout << "[DB: Insert]: " << "uid: " << uid << " BITTREX" << " asset: " << asset << " amount: " << amount << " date: " << date << std::endl;

    util::MySqlStore mySqlStore;
	mySqlStore.saveDepositHistory(uid, "BITTREX", asset, amount, date, txid);
    
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetPortfolio::process(const ParseRequest& parseRequest)
{        
    std::cout << "[account::GetPortfolio]: " << "symbol: " << parseRequest.symbol.c_str();
    
    util::MySqlStore mySqlStore;
	exchangeAnswer = mySqlStore.calcPortfolioCostBittrex(parseRequest.userID);

    Json::Value exchangeAnswer_balance;
    bitxCPP.get_balances( exchangeAnswer_balance );
    
    return Engine::postGetPortfolioProcessing(parseRequest.ctRequestId.c_str(), parseRequest.userID, exchangeAnswer_balance);
}

/******************************************************************************/
// Authenticated API market implementation
using namespace Trading::Bittrex::order;
// GET /market/buylimit
// Used to place a buy order in a specific market. Use buylimit to place limit orders. 
// Make sure you have the proper permissions set on your API keys for this call to work.
std::string SendLimitOrder::process(const ParseRequest& parseRequest)
{        
    std::cout << "[order::SendLimitOrder]: " << "symbol: " << parseRequest.symbol.c_str() << " side: " << parseRequest.orderSide.c_str() << " quantity: " << parseRequest.quantity << " price: " << parseRequest.price  <<  std::endl;

    if ( parseRequest.orderSide == "BUY" ){
        bitxCPP.buy_limit(parseRequest.symbol.c_str(), parseRequest.quantity, parseRequest.price, exchangeAnswer);
    } else if ( parseRequest.orderSide == "SELL" ) {
        bitxCPP.sell_limit(parseRequest.symbol.c_str(), parseRequest.quantity, parseRequest.price, exchangeAnswer);
    }

    std::cout << exchangeAnswer << std::endl;

    return Engine::postSendOrderProcessing(parseRequest.ctRequestId.c_str());
}

// std::string BuyLimit::process(const ParseRequest& parseRequest)
// {        
//     std::cout << "[send_order::BuyLimit]: " << "symbol: " << parseRequest.symbol.c_str() << " quantity: " << parseRequest.quantity << " price: " << parseRequest.price  << std::endl;

//     bitxCPP.buy_limit(parseRequest.symbol.c_str(), parseRequest.quantity, parseRequest.price, exchangeAnswer);

//     std::cout << exchangeAnswer << std::endl;

//     return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
// }

// std::string SellLimit::process(const ParseRequest& parseRequest)
// {        
//     std::cout << "[send_order::SellLimit]: " << "symbol: " << parseRequest.symbol.c_str() << " quantity: " << parseRequest.quantity << " price: " << parseRequest.price  << std::endl;

//     bitxCPP.sell_limit(parseRequest.symbol.c_str(), parseRequest.quantity, parseRequest.price, exchangeAnswer);

//     std::cout << exchangeAnswer << std::endl;

//     return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
// }


std::string CancelOneOrder::process(const ParseRequest& parseRequest)
{        
    std::stringstream orderReply;
    std::cout << "[order::CancelOneOrder]: " << "orderId: " << parseRequest.ctRequestId.c_str() << std::endl;

    bitxCPP.cancel_oneOrder(parseRequest.orderId.c_str(), exchangeAnswer);
	
	std::cout << exchangeAnswer << std::endl;

    return Engine::postCancelOneOrderProcessing(parseRequest.ctRequestId.c_str(), parseRequest.orderId.c_str());

	// The following is to enhance the reply, not necessarily to complete the cancel order request.
    // Bittrex cancel order API only return true if cancelled, no other details
    // Add logic to confirm order is indeed cancelled.
	/*std::cout << exchangeAnswer << std::endl;
    if ( !exchangeAnswer["success"].asBool() )
	{ 
        orderReply << "{\"clientOrderId\":\"" << parseRequest.ctRequestId.c_str() << "\","  << "\"msg\":\"" << "INVALID order status or oderId." << "\"" << "}";
        return orderReply.str();
    } 
    
    bitxCPP.get_order(parseRequest.ctRequestId.c_str(), exchangeAnswer);
	std::cout << exchangeAnswer << std::endl;
    if ( exchangeAnswer["result"]["CancelInitiated"].asBool()  )
    {
            orderReply << "{\"clientOrderId\":\"" << parseRequest.ctRequestId.c_str() << "\","  << "\"status\":\"" << "CANCELED" << "\"" << "}";
            return orderReply.str();
    }*/

    
}

std::string CancelAllOrders::process(const ParseRequest& parseRequest)
{        
    std::stringstream orderReply;
    std::cout << "[order::CancelAllOrders]: " << "orderId: " << parseRequest.ctRequestId.c_str() << std::endl;

    //bitxCPP.cancel_oneOrder(parseRequest.ctRequestId.c_str(), exchangeAnswer);
	
	std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}


// GET /account/getorder
// Used to retrieve a single order by uuid.
std::string GetOrderStatus::process(const ParseRequest& parseRequest)
{
    std::cout << "[order::GetOrderStatus]: " << "uuid: " << parseRequest.orderId.c_str() <<  std::endl;

    bitxCPP.get_order(parseRequest.orderId.c_str(), exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postGetOrderStatusProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetAllOpenOrders::process(const ParseRequest& parseRequest)
{        
    std::cout << "[order::GetAllOpenOrders]: " << "symbol: " << parseRequest.symbol.c_str() << " quantity: " << parseRequest.quantity << " price: " << parseRequest.price  << std::endl;

    bitxCPP.get_openOrders(parseRequest.symbol.c_str(), exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postGetAllOpenOrdersProcessing(parseRequest.ctRequestId.c_str());
}


// GET /account/getorderhistory
// Used to retrieve your order history.
std::string GetAllOrders::process(const ParseRequest& parseRequest)
{        
    std::cout << "[order::GetAllOrders]: " << std::endl;

    bitxCPP.get_orderHistory(exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postGetAllOrdersProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetOrderBook::process(const ParseRequest& parseRequest)
{
    std::cout << "[generic::GetOrderBook]: " << "symbol: " << parseRequest.symbol.c_str() << " type: " << parseRequest.type.c_str() << std::endl;

    bitxCPP.get_orderBook(parseRequest.symbol.c_str(), "both", exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postGetOrderBookProcessing(parseRequest.ctRequestId.c_str(), parseRequest.symbol.c_str());
}

std::string GetTradeHistory::process(const ParseRequest& parseRequest)
{
    std::cout << "[generic::GetTradeHistory]: " << "symbol: " << parseRequest.symbol.c_str() << " type: " << parseRequest.type.c_str() << std::endl;
	
	bitxCPP.get_orderHistory(exchangeAnswer);

	std::cout << exchangeAnswer << std::endl;
	
	return Engine::postGetTradeHistoryProcessing(parseRequest.ctRequestId.c_str(), parseRequest.userID);
}

