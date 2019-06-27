#include "binance.h"
#include "bitstamp.h"
#include "bittrex.h"
#include "coinbase.h"
#include "kraken.h"
#include "raycore.h"
#include "mySqlStore.h"

#include <json/json.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <Poco/Bugcheck.h>

#include <iostream>

using namespace Trading::Raycore;

Engine::Engine()
    :   recvWindow(10000)
{
}

bool Engine::init(const ParseRequest& parseRequest)
{
    // Get exchange user ID for BITSTAMP
    std::string exchangeUID;
	try 
	{
		util::MySqlStore uidSqlStore;
    	exchangeUID = uidSqlStore.fetchExchangeUID(parseRequest.userID, "BITSTAMP");
	}
	catch (Poco::Exception& ex)
	{
		std::cerr << ex.displayText() << std::endl;
	}

    return Exchange::init(parseRequest) && (binaCPP.init(key,secret) && bitsCPP.init(key, secret, exchangeUID));
}

/******************************************************************************/
using namespace Trading::Raycore::account;
std::string GetPortfolio::process(const ParseRequest& parseRequest)
{    
    std::cout << "[RAYCORE::account::GetPortfolio] ctRequestId: " << parseRequest.ctRequestId << std::endl;
	
	Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

	util::MySqlStore mySqlStore;
		
    std::cout << exchangeAnswer << std::endl;
    return fastWriter.write(root);
}


std::string GetBalanceAsset::process(const ParseRequest& parseRequest)
{    
    std::cout << "[RAYCORE::account::GetBalanceAsset] ctRequestId: " << parseRequest.ctRequestId << std::endl;
	
	// select asset, sum(amount)  from trading.account_balance
	// where amount <> 0
	// group by asset

	Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

	util::MySqlStore mySqlStore;
	element = mySqlStore.selectBalanceByAsset(parseRequest.userID);	
	
	root["ctRequestId"]	= parseRequest.ctRequestId;
    root["reply"]       = element;
	
	std::cout << root << std::endl;
    return fastWriter.write(root);
}

std::string GetBalanceExchange::process(const ParseRequest& parseRequest)
{    
    std::cout << "[RAYCORE::account::GetBalanceExchange] ctRequestId: " << parseRequest.ctRequestId << std::endl;
	
// 	select exchange, asset, amount from trading.account_balance
// 	where amount <> 0

	Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

	util::MySqlStore mySqlStore;
    element = mySqlStore.selectBalanceByExchange(parseRequest.userID);
	
	root["ctRequestId"]	= parseRequest.ctRequestId;
    root["reply"]		= element; 	
    	
	std::cout << root << std::endl;
	return fastWriter.write(root);
}
