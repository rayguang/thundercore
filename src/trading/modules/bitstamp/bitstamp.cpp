#include "bitstamp.h"
#include "mySqlStore.h"

#include <bitscpp_websocket.h>

#include <json/json.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <Poco/Bugcheck.h>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
/******************************************************************************/
using namespace Trading::Bitstamp;

Engine::Engine()
    :   recvWindow(10000)
{
}

bool Engine::init(const ParseRequest& parseRequest)
{
    // Get exchange user id for signature
    std::string exchangeUID;
    
	try 
	{
		util::MySqlStore uidSqlStore;
    	exchangeUID = uidSqlStore.fetchExchangeUID(parseRequest.userID, parseRequest.exchange.c_str());
	}
	catch (Poco::Exception& ex)
	{
		std::cerr << ex.displayText() << std::endl;
	}
	// std::cout << "bitstamp user customer id: " << exchangeUID << std::endl;
	
    return Exchange::init(parseRequest) && bitsCPP.init(key, secret, exchangeUID);
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

// std::string Engine::postProcessing(const std::string& ctRequestId)
// {
//     std::stringstream orderReply;
    
//     if (exchangeAnswer.isMember(CLIENTORDER_ID) && exchangeAnswer.isMember(ORDER_ID))
//     {
//         const std::string clientOrderId   		= exchangeAnswer[CLIENTORDER_ID].asString();
//         const int orderId                 		= exchangeAnswer[ORDER_ID].asInt();
//         const std::string status          		= exchangeAnswer["status"].asString();
// 		const std::string cummulativeQuoteQty	= exchangeAnswer["cummulativeQuoteQty"].asString();
// 		const std::string executedQty			= exchangeAnswer["executedQty"].asString();
// 		const Json::Value fills					= exchangeAnswer["fills"];
// 		const std::string price					= exchangeAnswer["price"].asString();
// 		const long int transacTime				= exchangeAnswer["transactTime"].asLargestUInt();
          
// 		// Check if server replies to the same ctRequestId sent from our side
//         if (clientOrderId != ctRequestId and exchangeAnswer["status"].asString() != "CANCELLED") 
//             std::cout << "[ERROR]: Server clientOrderId: " << clientOrderId << " vs ctRequestId: " << ctRequestId << std::endl;

//         orderReply << "{\"clientOrderId\":\"" << clientOrderId << "\"," << "\"orderId\":" << orderId <<"," 
// 					<< "\"status\":\"" << status << "\"," << "\"cummulativeQuoteQty\":\"" << cummulativeQuoteQty  << "\"," 
// 					<< "\"executedQty\":\"" << executedQty << "\"," << "\"price\":\"" << price << "\","
// 					<< "\"transacTime\":" << transacTime << "," <<  "\"fills\":" << fills << "}";

//         util::MySqlStore mySqlStore;
//         mySqlStore.saveOrderAnswer(ctRequestId, status, orderId, clientOrderId);
//     }
//     else
//     {
//         int error_code  = exchangeAnswer["code"].asInt();
//         std::string msg = exchangeAnswer["msg"].asString();

//         orderReply << "{\"clientOrderId\":\"" << ctRequestId << "\"," << "\"code\":" << error_code <<"," << "\"msg\":\"" << msg << "\"" << "}";

//         util::MySqlStore mySqlStore;
//         mySqlStore.saveOrderAnswerError(ctRequestId, error_code, msg);
//     }    
    
//     return orderReply.str();
// }

std::string Engine::postGetTickerProcessing(const std::string& ctRequestId, const std::string& symbol)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

    element["symbol"]   = symbol;
    element["price"]    = exchangeAnswer["last"];
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
    util::MySqlStore mySqlStore;

    element[0]["Symbo"]     = "BCH";
    element[0]["Free"]      = exchangeAnswer["bch_available"];
    element[0]["Locked"]    = exchangeAnswer["bch_reserved"];
    element[0]["Total"]     = exchangeAnswer["bch_available"];
    mySqlStore.updateAccountBalance(uid, "BITSTAMP", "BCH", std::atof(element[0]["Total"].asString().c_str()));

    element[1]["Symbo"]     = "BTC";
    element[1]["Free"]      = exchangeAnswer["btc_available"];
    element[1]["Locked"]    = exchangeAnswer["btc_reserved"];
    element[1]["Total"]     = exchangeAnswer["btc_available"];
    mySqlStore.updateAccountBalance(uid, "BITSTAMP", "BTC", std::atof(element[1]["Total"].asString().c_str()));

    element[2]["Symbo"]     = "ETH";
    element[2]["Free"]      = exchangeAnswer["eth_available"];
    element[2]["Locked"]    = exchangeAnswer["eth_reserved"];
    element[2]["Total"]     = exchangeAnswer["eth_available"];
    mySqlStore.updateAccountBalance(uid, "BITSTAMP", "ETH", std::atof(element[2]["Total"].asString().c_str()));

    element[3]["Symbo"]     = "LTC";
    element[3]["Free"]      = exchangeAnswer["ltc_available"];
    element[3]["Locked"]    = exchangeAnswer["ltc_reserved"];
    element[3]["Total"]     = exchangeAnswer["ltc_available"];
    mySqlStore.updateAccountBalance(uid, "BITSTAMP", "LTC", std::atof(element[3]["Total"].asString().c_str()));

    element[4]["Symbo"]     = "XRP";
    element[4]["Free"]      = exchangeAnswer["xrp_available"];
    element[4]["Locked"]    = exchangeAnswer["xrp_reserved"];
    element[4]["Total"]     = exchangeAnswer["xrp_available"];
    mySqlStore.updateAccountBalance(uid, "BITSTAMP", "XRP", std::atof(element[4]["Total"].asString().c_str()));

    element[5]["Symbo"]     = "EUR";
    element[5]["Free"]      = exchangeAnswer["eur_available"];
    element[5]["Locked"]    = exchangeAnswer["eur_reserved"];
    element[5]["Total"]     = exchangeAnswer["eur_available"];
    mySqlStore.updateAccountBalance(uid, "BITSTAMP", "EUR", std::atof(element[5]["Total"].asString().c_str()));

    element[6]["Symbo"]     = "USD";
    element[6]["Free"]      = exchangeAnswer["usd_available"];
    element[6]["Locked"]    = exchangeAnswer["usd_reserved"];
    element[6]["Total"]     = exchangeAnswer["usd_available"];
    mySqlStore.updateAccountBalance(uid, "BITSTAMP", "USD", std::atof(element[6]["Total"].asString().c_str()));

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

    //element["symbol"]   	= exchangeAnswer["symbol"].asString();
    //element["status"]   	= exchangeAnswer["status"].asString();
    element["orderId"]  	= exchangeAnswer["id"];
    element["price"]    	= exchangeAnswer["price"];
    element["origQty"] 		= exchangeAnswer["amount"];
    // element["side"]     	= exchangeAnswer["side"];
    element["type"]     	= order_type[std::atoi(exchangeAnswer["type"].asString().c_str())];
	
	if (exchangeAnswer.isMember("reason")) {
		root["msg"]			= exchangeAnswer["reason"]["__all__"][0];	
	}

    root["ctRequestId"] = ctRequestId;
	root["reply"] = element;

    std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);
}

std::string Engine::postGetOrderStatusProcessing(const std::string& ctRequestId, const std::string& orderId)
{
    Json::FastWriter fastWriter;
    Json::Value root;
	Json::Value element;

    if (exchangeAnswer["error"].asString() != "") {
        root["reply"] = exchangeAnswer["error"];
        root["ctRequestId"] = ctRequestId;
        return fastWriter.write(root);
    } else {
        //element["symbol"]       = exchangeAnswer["symbol"];
        element["orderId"]      = orderId;
        element["status"]       = exchangeAnswer["status"];
        //element["price"]        = exchangeAnswer["symbol"];
        //element["stopPrice"]	= exchangeAnswer["stopPrice"];
        //element["origQty"]      = exchangeAnswer["origQty"];
        //element["side"]         = exchangeAnswer["side"];
        //element["type"]         = exchangeAnswer["type"];
        //element["ctOrderId"]    = exchangeAnswer["clientOrderId"];
        //element["time"]			= exchangeAnswer["time"];
        //element["updateTime"]	= exchangeAnswer["updateTime"];

        root["ctRequestId"] = ctRequestId;
        root["reply"] = element;
        std::cout << "[REPLY: postGetOrderStatusProcessing]: " << root << std::endl;
        return fastWriter.write(root);
    }
}


std::string Engine::postGetAllOpenOrdersProcessing(const std::string& ctRequestId)
{
    Json::FastWriter fastWriter;
    Json::Value root;
	Json::Value element;

	// std::cout << "exchangeAnswer[0]: " << exchangeAnswer[0] << "size: " << exchangeAnswer.size()<< std::endl;

    for (unsigned int i = 0 ; i < exchangeAnswer.size() ; i++)
    {
        // send ctRequestId, clientOrderId, orderId, origQty, price, side, status, symbol, type, updateTime
        // break the batch into 20 orders every batch
        //element[i]["clientOrderId"]      = exchangeAnswer[i]["clientOrderId"].asString();
        element[i]["orderId"]            = exchangeAnswer[i]["id"];
        element[i]["origQty"]  	         = exchangeAnswer[i]["amount"];
        element[i]["price"]              = exchangeAnswer[i]["price"];
        //element[i]["side"]               = exchangeAnswer[i]["side"];
        //element[i]["symbol"]             = exchangeAnswer[i]["symbol"];
        
        //element[i]["status"]             = exchangeAnswer[i]["status"];
        // element[i]["stopPrice"]          = exchangeAnswer[i]["stopPrice"];
        element[i]["time"]           	 = exchangeAnswer[i]["datetime"];

        // limit order
        double price, limit_price, order_side;
        price = std::atof(exchangeAnswer[i]["price"].asString().c_str());
        limit_price = std::atof(exchangeAnswer[i]["limit_price"].asString().c_str());
        order_side  = std::stof(exchangeAnswer[i]["type"].asString());
        if ( order_side == 0){
            element[i]["side"]           = "BUY";
        } else if ( order_side == 1) {
            element[i]["side"]           = "SELL";
        } else {
            std::cout << "Invalid type: Type: 0 - buy; 1 - sell." << std::endl;
        }
        
	}

	root["reply"] = element;
	root["ctRequestId"] = ctRequestId;
    std::cout << "[REPLY: postGetAllOpenOrdersProcessing]: " << root << std::endl;

    return fastWriter.write(root);
}


std::string Engine::postGetAllOrdersProcessing(const std::string& ctRequestId)
{
    Json::FastWriter fastWriter;
    Json::Value root;
	Json::Value element;
	
	unsigned int i(0), j(0);
	std::string symbol, coin, base, fee_currency, symbol_upper;
    std::vector<std::string> symbol_list = {"ltc_usd", "eth_usd", "xrp_eur", "bch_usd", "bch_eur", "btc_eur", "xrp_btc", "eur_usd", "bch_btc", "ltc_eur", "btc_usd","ltc_btc","xrp_usd","eth_btc","eth_eur"};

	for ( i = 0; i < exchangeAnswer.size(); i++ ){
        if (std::stof(exchangeAnswer[i]["type"].asString()) == 2) {
        // Routine to get trading pair
            for (auto itr : symbol_list) {
                symbol  = itr;
                coin    = symbol.substr(0,3);
                base    = symbol.substr(4,3);
                //std::cout << "symbol: " << symbol << " coin: " << coin << " base: "<< base << std::endl; 
                if (exchangeAnswer[i].isMember(symbol)){
					symbol_upper 			= symbol;
					std::transform(symbol_upper.begin(), symbol_upper.end(), symbol_upper.begin(), ::toupper);
                    element[j]["origQty"]   = std::stof(exchangeAnswer[i][coin].asString());
                    element[j]["price"]     = exchangeAnswer[i][symbol];
                    element[j]["symbol"]    = symbol_upper;
                    fee_currency            = base;

                    if (element[j]["origQty"].asDouble() > 0){
                        element[j]["side"]  = "BUY";
                    } else {
                        element[j]["side"]  = "SELL";
                    }

                    element[j]["orderId"]           = exchangeAnswer[i]["order_id"];
                    element[j]["time"]              = exchangeAnswer[i]["datetime"];
                    element[j]["fee_amount"]        = exchangeAnswer[i]["fee"];
                    element[j]["tid"]               = exchangeAnswer[i]["id"];

                    j++;
                }
            }
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
	
    element["orderId"]            = exchangeAnswer["id"];
    element["status"]             = exchangeAnswer["status"].asString();
	
	root["reply"] = element;
	root["ctRequestId"] = ctRequestId;
    std::cout << "[REPLY: postCancelOneOrderProcessing]: " << root << std::endl;
	
    return fastWriter.write(root);
}

std::string Engine::postCancelAllOrdersProcessing(const std::string& ctRequestId)
{
    Json::FastWriter fastWriter;
	Json::Value root;
    Json::Value element;
	
    root["ctRequestId"]     = ctRequestId;
    root["reply"]["status"] = exchangeAnswer;
    return fastWriter.write(root);
}

std::string Engine::postGetOrderBookProcessing(const std::string& ctRequestId, const std::string& symbol)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;
	unsigned int i(0), limit(20);

	for (i = 0; i < limit; i++) {
		element["bids"][i][0] = exchangeAnswer["bids"][i][0];
		element["bids"][i][1] = exchangeAnswer["bids"][i][1];
		
		element["asks"][i][0] = exchangeAnswer["asks"][i][0];
        element["asks"][i][1] = exchangeAnswer["asks"][i][1];
	}

	root["exchange"]		= "BITSTAMP";
	root["symbol"]			= symbol;
    root["ctRequestId"]     = ctRequestId;
    root["reply"]			= element;

	std::cout << root << std::endl;
    return fastWriter.write(root);
}

std::string Engine::postGetTradeHistoryProcessing(const std::string& ctRequestId, const int uid)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

	unsigned int i(0), j(0);
    double order_side, amount, price;
    std::string symbol, coin, base, fee_currency;
    std::vector<std::string> symbol_list = {"ltc_usd", "eth_usd", "xrp_eur", "bch_usd", "bch_eur", "btc_eur", "xrp_btc", "eur_usd", "bch_btc", "ltc_eur", "btc_usd","ltc_btc","xrp_usd","eth_btc","eth_eur"};
    
    util::MySqlStore mySqlStore;
    mySqlStore.deleteTradeHistory_Bitstamp(uid);

    for ( i = 0; i < exchangeAnswer.size(); i++ ){
        if (std::stof(exchangeAnswer[i]["type"].asString()) == 2) {
		// Routine to get trading pair
            for (auto itr : symbol_list) {
                symbol  = itr;
                coin    = symbol.substr(0,3);
                base    = symbol.substr(4,3);
                //std::cout << "symbol: " << symbol << " coin: " << coin << " base: "<< base << std::endl; 
                if (exchangeAnswer[i].isMember(symbol)){
                    element[j]["origQty"]   = std::stof(exchangeAnswer[i][coin].asString());
                    element[j]["price"]		= exchangeAnswer[i][symbol];
                    element[j]["symbol"]	= symbol;
                    fee_currency 		    = base;

                    if (element[j]["origQty"].asDouble() > 0){
                        element[j]["side"]	= "BUY";
                    } else {
                        element[j]["side"]	= "SELL";
                    }       
                    
                    order_side = std::stof(exchangeAnswer[i]["type"].asString());
                    if (order_side == 0) {
                        element[i]["type"]          = "DEPOSIT";
                    } else if (order_side == 1) {
                        element[i]["type"]          = "WITHDRAWL";
                    } else if (order_side == 2) {
                        element[i]["type"]          = "TRADE";
                    }

                    element[j]["orderId"]           = exchangeAnswer[i]["order_id"];
                    element[j]["time"]              = exchangeAnswer[i]["datetime"];
                    element[j]["fee_amount"]        = exchangeAnswer[i]["fee"];
                    element[j]["tid"]               = exchangeAnswer[i]["id"];

                    amount = std::stof(element[j]["origQty"].asString());
                    price  = element[j]["price"].asDouble();

                    std::cout << "symbol: " << symbol << " amount: " << amount << " price: " << price << " type: " << element[j]["type"].asString() << " order_id: " << element[j]["orderId"].asString() << " fee: " << exchangeAnswer[i]["fee"] << " timestamp: " << element[j]["time"].asString() << std::endl;

                    mySqlStore.updateTradeHistory_Bitstamp(uid, coin, amount, price, exchangeAnswer[j]["liquidity"].asString(),exchangeAnswer[i]["order_id"].asString(), exchangeAnswer[i]["order_id"].asString(), std::atof(exchangeAnswer[i]["fee"].asString().c_str()), fee_currency, element[j]["time"].asString());
                    
                    j++;
                }
            }
        }
	}

	root["ctRequestId"] = ctRequestId;
	root["reply"]       = element;

    std::cout << "[REPLY: postGetTradeHistoryProcessing]: " << root << std::endl;
    return fastWriter.write(root);
}

std::string Engine::postGetPortfolioProcessing(const std::string& ctRequestId, const int uid, Json::Value exchangeAnswer_balance)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;
    std::string symbol, base, asset, asset_balance, total;
    double balance;
    std::vector<std::string> symbol_list = {"bch_balance", "btc_balance", "eth_balance", "eur_balance", "ltc_balance", "usd_balance", "xrp_balance"};
    unsigned int index(0),j(0);
	util::MySqlStore mySqlStore;
	
	mySqlStore.clearAccountBalance(uid, "BITSTAMP");

	for ( auto itr : symbol_list){
		symbol = itr;
        balance = std::stof(exchangeAnswer_balance[symbol].asString());

        asset_balance = symbol.substr(0,3);
        std::transform(asset_balance.begin(), asset_balance.end(), asset_balance.begin(), ::toupper);

        if ( balance > 0 ){   
			mySqlStore.updateAccountBalance(uid, "BITSTAMP", asset_balance, balance);

            for ( j = 0 ; j < exchangeAnswer.size(); j++) {
		   		asset = exchangeAnswer[j]["asset"].asString().substr(0,3);
				base  = exchangeAnswer[j]["asset"].asString().substr(4,3);
		 		std::transform(asset.begin(), asset.end(), asset.begin(), ::toupper);
                 

			   if (asset == asset_balance) {
			   		element[index]["symbol"]		= asset;
					element[index]["average_cost"]	= exchangeAnswer[j]["average_cost"].asString();
					element[index]["total_amount"]  = to_string(balance);
					element[index]["total_cost"]    = to_string(std::stof(exchangeAnswer[j]["average_cost"].asString()) * balance);

					index++;
			   } else {
					std::transform(base.begin(), base.end(), base.begin(), ::toupper);
			   		element[index]["symbol"]        = base;
                    element[index]["average_cost"]  = exchangeAnswer_balance[symbol].asString();;
                    element[index]["total_amount"]  = exchangeAnswer_balance[symbol].asString();
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
using namespace Trading::Bitstamp::generic;

// https://www.bitstamp.net/api/ticker/
// Get the market info for currency pair: btcusd, btceur, eurusd, 
// xrpusd, xrpeur, xrpbtc, ltcusd, ltceur, ltcbtc, ethusd, etheur, ethbtc, bchusd, bcheur, bchbtc
std::string GetTicker::process(const ParseRequest& parseRequest)
{
    std::cout << "[generic::GetTicker]: " << "symbol: " << parseRequest.symbol.c_str() << std::endl;

    bitsCPP.get_ticker(parseRequest.symbol.c_str(), exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postGetTickerProcessing(parseRequest.ctRequestId.c_str(), parseRequest.symbol.c_str());
}

// https://www.bitstamp.net/api/v2/ticker/{currency_pair}/
// Get the market info for currency pair: btcusd, btceur, eurusd, 
// xrpusd, xrpeur, xrpbtc, ltcusd, ltceur, ltcbtc, ethusd, etheur, ethbtc, bchusd, bcheur, bchbtc
std::string GetMarkets::process(const ParseRequest& parseRequest)
{
    std::cout << "[generic::GetMarkets]: " << "symbol: " << parseRequest.symbol.c_str() << std::endl;

    bitsCPP.get_ticker(parseRequest.symbol.c_str(), exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// https://www.bitstamp.net/api/v2/ticker_hour/{currency_pair}/
// Get hourly ticker info
// Supported values for currency_pair: btcusd, btceur, eurusd, 
// xrpusd, xrpeur, xrpbtc, ltcusd, ltceur, ltcbtc, ethusd, etheur, ethbtc, bchusd, bcheur, bchbtc
std::string GetHourlyMarkets::process(const ParseRequest& parseRequest)
{
    std::cout << "[generic::GetHourlyMarkets]: " << "symbol: " << parseRequest.symbol.c_str() << std::endl;

    bitsCPP.get_hourlyticker(parseRequest.symbol.c_str(), exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// https://www.bitstamp.net/api/v2/transactions/{currency_pair}/
// Get the hourly transactions
// Supported values for currency_pair: btcusd, btceur, eurusd, 
// xrpusd, xrpeur, xrpbtc, ltcusd, ltceur, ltcbtc, ethusd, etheur, ethbtc, bchusd, bcheur, bchbtc
std::string GetTransactions::process(const ParseRequest& parseRequest)
{
    std::cout << "[generic::GetTransactions]: " << "symbol: " << parseRequest.symbol.c_str() << std::endl;

    bitsCPP.get_transactions(parseRequest.symbol.c_str(), exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetTradingPairs::process(const ParseRequest& parseRequest)
{
    std::cout << "[generic::GetTradingPairs]: "  << std::endl;

    bitsCPP.get_tradingpairs(exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetEURUSD::process(const ParseRequest& parseRequest)
{
    std::cout << "[generic::GetEURUSD]: "  << std::endl;

    bitsCPP.get_eurusd(exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

/******************************************************************************/
using namespace Trading::Bitstamp::account;
std::string GetBalance::process(const ParseRequest& parseRequest)
{
    std::cout << "[generic::GetBalance]: " << "symbol: " << parseRequest.symbol.c_str() << std::endl;

    bitsCPP.get_balance(parseRequest.symbol.c_str(), exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postGetBalanceProcessing(parseRequest.ctRequestId.c_str(), parseRequest.userID);
}


// Get all user transactions in a descending list
std::string GetMyTrades::process(const ParseRequest& parseRequest)
{
    std::cout << "[generic::GetMyTrades]: " << "symbol: " << parseRequest.symbol.c_str() << " limit: " << parseRequest.limit << std::endl;
    bitsCPP.get_userTransactions("0", (parseRequest.limit > 1000 ? 1000 : parseRequest.limit), "desc", parseRequest.symbol.c_str(), exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
} // namespace account

std::string GetPortfolio::process(const ParseRequest& parseRequest)
{
    std::cout << "[account::GetPortfolio] " << std::endl;

    Json::Value exchangeAnswer_balance;
    util::MySqlStore mySqlStore;
	exchangeAnswer = mySqlStore.calcPortfolioCostBitstamp(parseRequest.userID);
	
    bitsCPP.get_balance(parseRequest.symbol.c_str(), exchangeAnswer_balance);

    return Engine::postGetPortfolioProcessing( parseRequest.ctRequestId.c_str(), parseRequest.userID, exchangeAnswer_balance );
}
/******************************************************************************/
using namespace Trading::Bitstamp::order;
// Get a list of open orders
std::string GetAllOpenOrders::process(const ParseRequest& parseRequest)
{
    std::cout << "[orders::GetOpenOrders]: " << "symbol: " << parseRequest.symbol.c_str() << std::endl;
	std::string symbol;
	if (parseRequest.symbol == "")
			symbol = "all";
	else 
			symbol = parseRequest.symbol;
	bitsCPP.get_openOrders(symbol.c_str(), exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postGetAllOpenOrdersProcessing(parseRequest.ctRequestId.c_str());
}

// Get order #orderId status
std::string GetOrderStatus::process(const ParseRequest& parseRequest)
{
    std::cout << "[orders::GetOrderStatus]: " << "orderId: " << parseRequest.orderId << std::endl;
    bitsCPP.get_orderStatus(parseRequest.orderId.c_str(), exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postGetOrderStatusProcessing(parseRequest.ctRequestId.c_str(), parseRequest.orderId.c_str());
}

// Place limit order
std::string SendLimitOrder::process(const ParseRequest& parseRequest)
{
    double  limit_price; 
		
	std::cout << "[orders::SendLimitOrder]: " << "symbol: " << parseRequest.symbol.c_str() << " action: " << parseRequest.orderSide << " quantity: " << parseRequest.quantity << " price: " << parseRequest.price << " limit_price: " << parseRequest.limitPrice << " timeInForce: " << parseRequest.timeInForce.c_str() <<  std::endl;
	

    if ( parseRequest.orderSide == "BUY")
    {
        bitsCPP.buy_limitOrder(parseRequest.symbol.c_str(), parseRequest.quantity, parseRequest.price, parseRequest.limitPrice, parseRequest.timeInForce.c_str(), exchangeAnswer );
    } else if ( parseRequest.orderSide == "SELL" )
    {
		if ( parseRequest.limitPrice == parseRequest.price )
			limit_price = 0.01;
        
		bitsCPP.sell_limitOrder(parseRequest.symbol.c_str(), parseRequest.quantity, parseRequest.price, limit_price, parseRequest.timeInForce.c_str(), exchangeAnswer );
    } else 
    {
        std::cout << "[orders::SendLimitOrder]: orderSide: MUST be either BUY or SELL" << std::endl;
    }

    std::cout << exchangeAnswer << std::endl;

    return Engine::postSendOrderProcessing(parseRequest.ctRequestId.c_str());
}

// Place market order
std::string SendMarketOrder::process(const ParseRequest& parseRequest)
{
    std::cout << "[orders::SendMarketOrder]: " << "symbol: " << parseRequest.symbol.c_str() << " action: " << parseRequest.orderSide << " quantity: " << parseRequest.quantity <<  std::endl;

    if ( parseRequest.orderSide == "BUY")
    {
        bitsCPP.buy_marketOrder(parseRequest.symbol.c_str(), parseRequest.quantity, exchangeAnswer );
    } else if ( parseRequest.orderSide == "SELL" )
    {
        bitsCPP.sell_marketOrder(parseRequest.symbol.c_str(), parseRequest.quantity, exchangeAnswer );
    } else 
    {
        std::cout << "[orders::SendMarketOrder]: orderSide: MUST be either BUY or SELL" <<std::endl;
    }

    std::cout << exchangeAnswer << std::endl;

    return Engine::postSendOrderProcessing(parseRequest.ctRequestId.c_str());
}

// Place instant order
std::string SendInstantOrder::process(const ParseRequest& parseRequest)
{
    std::cout << "[orders::SendInstantOrder]: " << "symbol: " << parseRequest.symbol.c_str() << " action: " << parseRequest.orderSide << " quantity: " << parseRequest.quantity <<  std::endl;

    if ( parseRequest.orderSide == "BUY")
    {
        bitsCPP.buy_instantOrder(parseRequest.symbol.c_str(), parseRequest.quantity, exchangeAnswer );
    } else if ( parseRequest.orderSide == "SELL" )
    {
        bitsCPP.sell_instantOrder(parseRequest.symbol.c_str(), parseRequest.quantity, exchangeAnswer );
    } else 
    {
        std::cout << "[orders::SendInstantOrder]: orderSide: MUST be either BUY or SELL" << std::endl;
    }

    std::cout << exchangeAnswer << std::endl;

    return Engine::postSendOrderProcessing(parseRequest.ctRequestId.c_str());
}

// Cancel one order #orderId status
std::string CancelOneOrder::process(const ParseRequest& parseRequest)
{
    std::cout << "[order::CancelOneOrder]: " << "orderId: " << parseRequest.orderId << std::endl;
    bitsCPP.cancel_order(parseRequest.orderId.c_str(), exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postCancelOneOrderProcessing(parseRequest.ctRequestId.c_str());
}

// Cancel all orders
std::string CancelAllOrders::process(const ParseRequest& parseRequest)
{
    std::cout << "[order::CancelAllOrders]: "  << std::endl;
    bitsCPP.cancel_allOrders(exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postCancelAllOrdersProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetOrderBook::process(const ParseRequest& parseRequest)
{
    std::cout << "[order::GetOrderBook]: " << "symbol: " << parseRequest.symbol.c_str() << std::endl;
    bitsCPP.get_orderbook(parseRequest.symbol.c_str(), exchangeAnswer);

    //std::cout << exchangeAnswer << std::endl;

    return Engine::postGetOrderBookProcessing(parseRequest.ctRequestId.c_str(), parseRequest.symbol.c_str());
}  

std::string GetAllOrders::process(const ParseRequest& parseRequest)
{
    std::cout << "[order::GetAllOrders]: " << "symbol: " << parseRequest.symbol.c_str() << std::endl;
    int limit(10);
    if (parseRequest.limit <= 0)
        limit = 10;
    bitsCPP.get_userTransactions("0", limit, "desc", parseRequest.symbol.c_str(), exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;

    return Engine::postGetAllOrdersProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetTradeHistory::process(const ParseRequest& parseRequest)
{
    std::cout << "[order::GetTradeHistory]: " << "symbol: " << parseRequest.symbol.c_str() << std::endl;
    
    int limit(0);
    if (parseRequest.limit <= 0) {
        limit = 10;
    } else {
        limit = parseRequest.limit;
    }
        
    bitsCPP.get_userTransactions("0", limit, "desc", parseRequest.symbol.c_str(), exchangeAnswer);
	
	std::cout << exchangeAnswer << std::endl;

	return Engine::postGetTradeHistoryProcessing(parseRequest.ctRequestId.c_str(), parseRequest.userID);
}

