#include <boost/algorithm/string.hpp>

#include "bitfinex.h"
#include "mySqlStore.h"

#include "bitfinex/BitfinexAPI.hpp"

#include <iomanip>
#include <ctime>

#include <json/json.h>
#include <crypto++/cryptlib.h>
#include <crypto++/osrng.h>
#include <crypto++/modes.h>
#include <crypto++/aes.h>
#include <crypto++/filters.h>
#include <crypto++/hex.h>
#include <crypto++/base64.h>
#include <crypto++/sha3.h>

using namespace Trading::Bitfinex;

BfxAPI::BitfinexAPI bfxAPI;

Engine::Engine()
    : recvWindow(10000)
{
}

long long Engine::string_to_ul(const char* str){
	std::stringstream ssValue;
	ssValue << str;
	
	long long intValue;
	ssValue >> intValue;

	return intValue;
}

bool Engine::init(const ParseRequest& parseRequest)
{
    // Init Bitfinex API instance
    util::MySqlStore mySqlStore;
    //mySqlStore.fetchInfo(parseRequest.userID, parseRequest.exchange.c_str());

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
        encoded.clear();
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
            //std::cout << "******** Decrypt Message ********" << std::endl;
            //std::cout << "[secret(encrypted)]: " << secret_decoded.substr(16).c_str() << std::endl;
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

	bfxAPI.setKeys(key, secret);
	//bfxAPI.setKeys(mySqlStore.getAPIkey(),mySqlStore.getAPIsecret());

	// Fetch API to confirm exchange status
    /*std::cout << "Bitfinex Request with error checking: " << std::endl;
    if (!bfxAPI.hasApiError())
        std::cerr << bfxAPI.strResponse() << std::endl;
    else
    {
        std::cerr << "BfxApiStatusCode: ";
        std::cerr << bfxAPI.getBfxApiStatusCode() << std::endl;
        std::cerr << "CurlStatusCode: ";
        std::cerr << bfxAPI.getCurlStatusCode() << std::endl;
    }*/

    //std::cout << "Request without error checking" << std::endl;
    //std::cout << bfxAPI.getSummary().strResponse() << std::endl;
	return true;
}

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
    element["price"]    = exchangeAnswer["last_price"];
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
	std::string asset, amount;

	int j(0);
	
	for (unsigned int i =0; i < exchangeAnswer.size(); i++){
		amount = exchangeAnswer[i]["amount"].asString();
		if (amount != "0.0") {
			asset = exchangeAnswer[i]["currency"].asString();
            std::transform(asset.begin(), asset.end(), asset.begin(), ::toupper);
			element[j]["Symbol"]	= asset;
			element[j]["Free"]		= exchangeAnswer[i]["available"].asString();
        	element[j]["Total"]     = exchangeAnswer[i]["amount"].asString();
			element[j]["Locked"]	= "0";

        	util::MySqlStore mySqlStore;
			//std::cout << "[DB - INSERT ]" << uid << ' ' << "BITFINEX" << ' ' << element[j]["Symbol"] << ' ' << element[j]["Total"];
        	mySqlStore.updateAccountBalance(uid, "BITFINEX", asset, std::stof(element[j]["Total"].asString()));
			j++;
		}
	}

	root["ctRequestId"] = ctRequestId;
	root["balances"] = element;

    std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);
}

std::string Engine::postGetTradeHistory(const std::string& ctRequestId, const std::string& symbol, const int uid)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;

    util::MySqlStore mySqlStore;
    mySqlStore.deleteTradeHistory_Bitfinex(uid, "BITFINEX");
	
	for (unsigned int i =0; i < exchangeAnswer.size(); i++){
		element[i]["symbol"]	         = symbol.substr(0,3);
		element[i]["price"]              = exchangeAnswer[i]["price"];
        element[i]["amount"]             = exchangeAnswer[i]["amount"];
        element[i]["time"]               = exchangeAnswer[i]["timestamp"];
        element[i]["type"]               = exchangeAnswer[i]["type"];
        element[i]["fee_currency"]       = exchangeAnswer[i]["fee_currency"];
        element[i]["fee_amount"]         = exchangeAnswer[i]["fee_amount"];
        element[i]["tid"]                = exchangeAnswer[i]["tid"];
        element[i]["order_id"]           = exchangeAnswer[i]["order_id"];

		std::string tid		= to_string(exchangeAnswer[i]["tid"].asUInt());
		double amount 		= std::atof(exchangeAnswer[i]["amount"].asString().c_str());
		double price		= std::atof(exchangeAnswer[i]["price"].asString().c_str());
		double fee_amount	= std::atof(exchangeAnswer[i]["fee_amount"].asString().c_str());
		std::string side	= exchangeAnswer[i]["type"].asString();

		if (side == "Sell") 
				amount = -1 * amount;

		//std::cout << "[DB - INSERT ]" << uid << ' ' << "BITFINEX" << ' ' << element[i]["symbol"] << ' ' << std::endl;
        mySqlStore.updateTradeHistory_Bitfinex(uid, "BITFINEX", symbol.substr(0,3), amount, price, exchangeAnswer[i]["type"].asString(),  exchangeAnswer[i]["order_id"].asString(), exchangeAnswer[i]["tid"].asString(), fee_amount, exchangeAnswer[i]["fee_currency"].asString(), exchangeAnswer[i]["timestamp"].asString());
	}

	root["ctRequestId"] = ctRequestId;
	root["reply"] = element;

    std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);
}


std::string Engine::postGetPortfolioProcessing(const std::string& ctRequestId, const int uid, Json::Value exchangeAnswer_balance)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;
	std::string symbol, asset, total;
	util::MySqlStore mySqlStore;

	unsigned int i(0), p(0);
	
	mySqlStore.clearAccountBalance(uid, "BITFINEX");

	for (i =0; i < exchangeAnswer_balance.size(); i++){
        symbol = exchangeAnswer_balance[i]["currency"].asString();
        total  = exchangeAnswer_balance[i]["amount"].asString();
		std::transform(symbol.begin(), symbol.end(), symbol.begin(),::toupper);

		if (std::stof(total) > 0) {
			mySqlStore.updateAccountBalance(uid, "BITFINEX", symbol, std::stof(total));
        	for (int j =0; j < exchangeAnswer.size(); j++){
            	asset = (exchangeAnswer[j]["asset"].asString());
				std::cout << "balance symbol: " << symbol << " portfolio asset: " << asset << " total: " << std::stof(total) << std::endl;

            	if (symbol == asset) {
               		element[p]["symbol"]            = symbol;
                	element[p]["average_cost"]      = exchangeAnswer[j]["average_cost"].asDouble();
                	element[p]["total_amount"]      = std::stof(total.c_str());
                	element[p]["total_cost"]        = std::stof(total.c_str()) * exchangeAnswer[j]["average_cost"].asDouble();

                	std::cout << "element: " << element << std::endl;
					p++;
            	} else {
					element[p]["symbol"]            = symbol;
                    element[p]["average_cost"]      = 1;
                    element[p]["total_amount"]      = std::stof(total.c_str());
                    element[p]["total_cost"]        = std::stof(total.c_str());
					p++;
				}
        	}
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
	
	unsigned int i = 0;

	for ( i =0; i < exchangeAnswer["asks"].size(); i++) {
		element["asks"][i][0] = exchangeAnswer["asks"][i]["price"];
		element["asks"][i][1] = exchangeAnswer["asks"][i]["amount"];
		element["asks"][i][2] = exchangeAnswer["asks"][i]["timestamp"];

		element["bids"][i][0] = exchangeAnswer["bids"][i]["price"];
		element["bids"][i][1] = exchangeAnswer["bids"][i]["amount"];
        element["bids"][i][2] = exchangeAnswer["bids"][i]["timestamp"];
	}
	
	root["exchange"]	= "BITFINEX";
	root["symbol"]		= symbol;
    root["ctRequestId"] = ctRequestId;
    root["reply"] 		= element;

    std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);
}

std::string Engine::postSendOrderProcessing(const std::string& ctRequestId)
{
    Json::FastWriter fastWriter;
    Json::Value root;
    Json::Value element;
	
	element["symbol"]   	= exchangeAnswer["symbol"];
    element["orderId"]  	= exchangeAnswer["order_id"];
    element["price"]    	= exchangeAnswer["price"];
    element["origQty"] 		= exchangeAnswer["original_amount"];
    element["side"]     	= exchangeAnswer["side"];
    element["type"]     	= exchangeAnswer["type"];
	
    root["ctRequestId"] = ctRequestId;
    root["reply"] 		= element;

    std::cout << "root: " << root << std::endl;
    return fastWriter.write(root);
}


/******************************************************************************/
using namespace Trading::Bitfinex::order;
// Placing a single order
std::string SendNewOrder::process(const ParseRequest& parseRequest)
{
    std::cout << "[order::SendNewOrder] " << "Symbol: " << parseRequest.symbol << " quantity: " << parseRequest.quantity << " price: " << parseRequest.price << " side: " << parseRequest.orderSide.c_str()  << " type: " << parseRequest.type.c_str() <<  std::endl;
   
    // New order example
    // bfxAPI.newOrder("btcusd",
    //                  0.01,
    //                  983,
    //                  "sell",
    //                  "exchange limit",
    //                  false,
    //                  true,
    //                  false,
    //                  false,
    //                  0);
    #pragma message "Disable swithch for is_hidden, is_postonly, use_allavailable, ocoorder"
    bfxAPI.newOrder(boost::algorithm::to_lower_copy(parseRequest.symbol), parseRequest.quantity, parseRequest.price, boost::algorithm::to_lower_copy(parseRequest.orderSide), boost::algorithm::to_lower_copy(parseRequest.type), false, true, false, false, 0);
	
	//bfxAPI.newOrder("btcusd", 0.004, 2000, "buy", "limit", false, true, false, false, 0);
    std::cout << bfxAPI.strResponse() << std::endl;

    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());

}

std::string SendMarketOrder::process(const ParseRequest& parseRequest)
{
    std::cout << "[order::SendMarketOrder] " << "Symbol: " << parseRequest.symbol << " quantity: " << parseRequest.quantity << " price: " << parseRequest.price << " side: " << parseRequest.orderSide.c_str()  << " type: " << parseRequest.type.c_str() <<  std::endl;
   
    // New order example
    // bfxAPI.newOrder("btcusd",
    //                  0.01,
    //                  983,
    //                  "sell",
    //                  "exchange limit",
    //                  false,
    //                  true,
    //                  false,
    //                  false,
    //                  0);
    bfxAPI.newOrder(boost::algorithm::to_lower_copy(parseRequest.symbol), parseRequest.quantity, parseRequest.price, boost::algorithm::to_lower_copy(parseRequest.orderSide), "market", false, true, false, false, 0);
	
	//bfxAPI.newOrder("btcusd", 0.004, 2000, "buy", "limit", false, true, false, false, 0);
    std::cout << bfxAPI.strResponse() << std::endl;

    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postSendOrderProcessing(parseRequest.ctRequestId.c_str());

}

std::string SendLimitOrder::process(const ParseRequest& parseRequest)
{
    std::cout << "[order::SendLimitOrder] " << "Symbol: " << parseRequest.symbol << " quantity: " << parseRequest.quantity << " price: " << parseRequest.price << " side: " << parseRequest.orderSide.c_str()  << " type: " << parseRequest.type.c_str() <<  std::endl;
   
    // New order example
    // bfxAPI.newOrder("btcusd",
    //                  0.01,
    //                  983,
    //                  "sell",
    //                  "exchange limit",
    //                  false,
    //                  true,
    //                  false,
    //                  false,
    //                  0);
    bfxAPI.newOrder(boost::algorithm::to_lower_copy(parseRequest.symbol), parseRequest.quantity, parseRequest.price, boost::algorithm::to_lower_copy(parseRequest.orderSide), "limit", false, true, false, false, 0);
	
	//bfxAPI.newOrder("btcusd", 0.004, 2000, "buy", "limit", false, true, false, false, 0);
    std::cout << bfxAPI.strResponse() << std::endl;

    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postSendOrderProcessing(parseRequest.ctRequestId.c_str());

}

// Replace an existing order
std::string ReplaceOrder::process(const ParseRequest& parseRequest)
{
    std::cout << "[order::ReplaceOrder] " << "Symbol: " << parseRequest.symbol << " orderId: " << parseRequest.orderId.c_str() << " quantity: " << parseRequest.quantity << " price: " << parseRequest.price << " side: " << parseRequest.orderSide.c_str()  << " type: " << parseRequest.type.c_str() <<  std::endl;

    // replaceOrder not working: orderId could not be cancelled
    bfxAPI.replaceOrder(parseRequest.orderId.c_str(), parseRequest.symbol.c_str(), parseRequest.quantity, parseRequest.price, parseRequest.orderSide.c_str(), parseRequest.type.c_str(),false, false);
    std::cout << bfxAPI.strResponse() << std::endl;

    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// Get a single order status
std::string GetOrderStatus::process(const ParseRequest& parseRequest)
{
    std::cout << "[order::GetOrderStatus] " << "Symbol: " << parseRequest.symbol << " OrderId: " << parseRequest.orderId << std::endl;

    //bfxAPI.getOrderStatus(parseRequest.orderId);
    bfxAPI.getOrderStatus( parseRequest.orderId.c_str() );
    std::cout << bfxAPI.strResponse() << std::endl;

	exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}
// Get all active orders
std::string GetAllOpenOrders::process(const ParseRequest& parseRequest)
{
    std::cout << "[order::GetAllOpenOrders] " << " getting active orders ..." << std::endl;
    bfxAPI.getActiveOrders();

    std::cout << bfxAPI.strResponse() << std::endl;

    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// Get history orders
std::string GetAllOrders::process(const ParseRequest& parseRequest)
{
    std::cout << "[order::GetAllOrders] " << " Number of orders to retriece: " << parseRequest.limit << std::endl;
    bfxAPI.getOrdersHistory(parseRequest.limit);

    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}


// Cancel one order only
std::string CancelOneOrder::process(const ParseRequest& parseRequest)
{
    std::cout << "[order::CancelOneOrder] " << "Symbol: " << parseRequest.symbol << " orderId: " << parseRequest.orderId << std::endl;

    bfxAPI.cancelOrder( parseRequest.orderId.c_str() );
    std::cout << bfxAPI.strResponse() << std::endl;

    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// Cancel multiple orders
std::string CancelMultiOrders::process(const ParseRequest& parseRequest)
{
    std::cout << "[order::CancelMultiOrders] " << std::endl;
    return "CancelMultiOrders called.";
}


// Cancel all orders
std::string CancelAllOrders::process(const ParseRequest& parseRequest)
{
    std::cout << "[order::CancelAllOrders] " << std::endl;

    bfxAPI.cancelAllOrders();
    std::cout << bfxAPI.strResponse() << std::endl;

    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// Get past trades
std::string GetTradeHistory::process(const ParseRequest& parseRequest)
{
	Json::Reader reader;
	std::cout << "[order::GetTradeHistory] " << std::endl;

	int limit;
	if (parseRequest.limit < 1)
			limit = 20;
	else 
			limit = parseRequest.limit;

	std::string symbol(parseRequest.symbol);
    std::transform(symbol.begin(), symbol.end(), symbol.begin(),::tolower);
	
	if (!parseRequest.fromtm.empty() && !parseRequest.totm.empty()){
		std::time_t fromtm, totm;
    	fromtm  = std::stoi(parseRequest.fromtm);
    	totm    = std::stoi(parseRequest.totm);
		
		bfxAPI.getPastTrades(symbol.c_str(), fromtm, totm, parseRequest.limit, false);
	}
	else {
		bfxAPI.getPastTrades(symbol.c_str(), 0L, 0L, parseRequest.limit, false);
	}

    exchangeAnswer = bfxAPI.strResponse();
	reader.parse( (bfxAPI.strResponse()).c_str(), exchangeAnswer);
	std::cout << exchangeAnswer << std::endl;
    return Engine::postGetTradeHistory(parseRequest.ctRequestId.c_str(), parseRequest.symbol.c_str(), parseRequest.userID);
}

// Get order book
std::string GetOrderBook::process(const ParseRequest& parseRequest)
{
    Json::Reader reader;
	std::string symbol(parseRequest.symbol);
    std::transform(symbol.begin(), symbol.end(), symbol.begin(),::tolower);

    std::cout << "[order::GetOrderBook] " << std::endl;

	bfxAPI.getOrderBook( symbol, 20, 20, true );
	reader.parse( (bfxAPI.strResponse()).c_str(), exchangeAnswer);

    std::cout << exchangeAnswer << std::endl;
    return Engine::postGetOrderBookProcessing(parseRequest.ctRequestId.c_str(), parseRequest.symbol.c_str());
}

/******************************************************************************/
// In marginfunding, orderId in request JSON is the offerId in API
using namespace Trading::Bitfinex::marginfunding;

std::string NewOffer::process(const ParseRequest& parseRequest)
{
    std::cout << "[marginfunding::NewOffer] " << std::endl;

    std::cout << bfxAPI.strResponse() << std::endl;
    
    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

std::string CancelOffer::process(const ParseRequest& parseRequest)
{
    std::cout << "[marginfunding::CancelOffer] " << std::endl;

    bfxAPI.cancelOffer( parseRequest.orderId.c_str() );
    std::cout << bfxAPI.strResponse() << std::endl;
    
    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetOfferStatus::process(const ParseRequest& parseRequest)
{
    std::cout << "[marginfunding::GetOfferStatus] " << std::endl;

    bfxAPI.getOfferStatus( parseRequest.orderId.c_str() );
    std::cout << bfxAPI.strResponse() << std::endl;
    
    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetActiveCredits::process(const ParseRequest& parseRequest)
{
    std::cout << "[marginfunding::GetActiveCredits] " << std::endl;

    bfxAPI.getActiveCredits();
    std::cout << bfxAPI.strResponse() << std::endl;
    
    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// Get offers
std::string GetOffers::process(const ParseRequest& parseRequest)
{
    std::cout << "[marginfunding::GetOffers] " << std::endl;

    bfxAPI.getOffers();
    std::cout << bfxAPI.strResponse() << std::endl;
    
    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// Get offers history with x records, x = limit
std::string GetOffersHistory::process(const ParseRequest& parseRequest)
{
    std::cout << "[marginfunding::GetOffersHistory] " << std::endl;

    bfxAPI.getOffersHistory(parseRequest.limit);
    std::cout << bfxAPI.strResponse() << std::endl;
    
    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// Get past funding trades
std::string GetPastFundingTrades::process(const ParseRequest& parseRequest)
{
    std::cout << "[marginfunding::GetPastFundingTrades] " << std::endl;

    // bfxAPI.getPastFundingTrades("BTC", 0, 50);
    bfxAPI.getPastFundingTrades(parseRequest.currency, 0, parseRequest.limit);
    std::cout << bfxAPI.strResponse() << std::endl;
    
    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// Get taken funds
std::string GetTakenFunds::process(const ParseRequest& parseRequest)
{
    std::cout << "[marginfunding::GetTakenFunds] " << std::endl;

    bfxAPI.getTakenFunds();
    std::cout << bfxAPI.strResponse() << std::endl;
    
    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// Get unused taken funds
std::string GetUnusedTakenFunds::process(const ParseRequest& parseRequest)
{
    std::cout << "[marginfunding::GetUnusedTakenFunds] " << std::endl;

    bfxAPI.getUnusedTakenFunds();
    std::cout << bfxAPI.strResponse() << std::endl;
    
    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// Get total taken funds
std::string GetTotalTakenFunds::process(const ParseRequest& parseRequest)
{
    std::cout << "[marginfunding::GetTotalTakenFunds] " << std::endl;

    bfxAPI.getTotalTakenFunds();
    std::cout << bfxAPI.strResponse() << std::endl;
    
    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// CLose loan with offerId = orderId
std::string CloseLoan::process(const ParseRequest& parseRequest)
{
    std::cout << "[marginfunding::CloseLoan] " << std::endl;

    bfxAPI.closeLoan( parseRequest.orderId.c_str() );
    std::cout << bfxAPI.strResponse() << std::endl;
    
    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// Close position with postionId
std::string ClosePosition::process(const ParseRequest& parseRequest)
{
    std::cout << "[marginfunding::ClosePosition] " << "positionId: " << parseRequest.positionId << std::endl;

    bfxAPI.closePosition(parseRequest.positionId);
    std::cout << bfxAPI.strResponse() << std::endl;
    
    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

/******************************************************************************/
using namespace Trading::Bitfinex::positions;

// Get active positions
std::string GetActivePositions::process(const ParseRequest& parseRequest)
{
    std::cout << "[positions::GetActivePositions] " << std::endl;

    bfxAPI.getActivePositions();
    std::cout << bfxAPI.strResponse() << std::endl;

    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// Claim positions, dunno
std::string ClaimPositions::process(const ParseRequest& parseRequest)
{
    std::cout << "[positions::ClaimPositions] " << std::endl;

    // bfxAPI.claimPosition(156321412LL, 150);
    bfxAPI.claimPosition(parseRequest.positionId, parseRequest.quantity);
    std::cout << bfxAPI.strResponse() << std::endl;

    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}


/******************************************************************************/
using namespace Trading::Bitfinex::account;
std::string GetAccount::process(const ParseRequest& parseRequest)
{
    std::cout << "[account::GetAccount] " << std::endl;

    bfxAPI.getAccountInfo();
	std::cout << bfxAPI.strResponse() << std::endl;
    
    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetBalance::process(const ParseRequest& parseRequest)
{
	Json::Reader reader;
    std::cout << "[account::GetBalance] " << std::endl;

    bfxAPI.getBalances();
	
	bool parsingSuccessful = reader.parse(bfxAPI.strResponse().c_str() , exchangeAnswer );     //parse process
    if ( !parsingSuccessful )
    {
        std::cout  << "Failed to parse"
               << reader.getFormattedErrorMessages();
        return 0;
    }

	std::cout << exchangeAnswer << std::endl;
    
	return Engine::postGetBalanceProcessing(parseRequest.ctRequestId.c_str(), parseRequest.userID);
}

std::string GetPortfolio::process(const ParseRequest& parseRequest)
{
	Json::Reader reader;
	Json::Value  exchangeAnswer_balance;
	std::cout << "[account::GetPortfolio] " << std::endl;
	
	bfxAPI.getBalances();
	bool parsingSuccessful = reader.parse(bfxAPI.strResponse().c_str() , exchangeAnswer_balance );     //parse process
    if ( !parsingSuccessful )
    {
        std::cout  << "Failed to parse"
               << reader.getFormattedErrorMessages();
        return 0;
    }
	std::cout << exchangeAnswer_balance << std::endl;

    util::MySqlStore mySqlStore;
	exchangeAnswer = mySqlStore.calcPortfolioCostBitfinex(parseRequest.userID, "BITFINEX");
	std::cout << exchangeAnswer << std::endl;

    return Engine::postGetPortfolioProcessing(parseRequest.ctRequestId.c_str(), parseRequest.userID, exchangeAnswer_balance);
}

/******************************************************************************/
using namespace Trading::Bitfinex::generic;
std::string GetAccountFees::process(const ParseRequest& parseRequest)
{
    std::cout << "[generic::GetAccountFees] " << std::endl;

    bfxAPI.getAccountFees();
	std::cout << bfxAPI.strResponse() << std::endl;
    
    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetSummary::process(const ParseRequest& parseRequest)
{
    std::cout << "[generic::GetSummary] " << std::endl;

    bfxAPI.getSummary();
	std::cout << bfxAPI.strResponse() << std::endl;
    
    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

std::string Deposit::process(const ParseRequest& parseRequest)
{
    std::cout << "[generic::Deposit] " << std::endl;

    //bfxAPI.deposit();
	std::cout << bfxAPI.strResponse() << std::endl;
    
    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetKeyPermissions::process(const ParseRequest& parseRequest)
{
    std::cout << "[generic::GetKeyPermissions] " << std::endl;

    bfxAPI.getKeyPermissions();
	std::cout << bfxAPI.strResponse() << std::endl;
    
    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetMarginInfo::process(const ParseRequest& parseRequest)
{
    std::cout << "[generic::GetMarginInfo] " << std::endl;

    bfxAPI.getMarginInfos();
	std::cout << bfxAPI.strResponse() << std::endl;
    
    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}


std::string Transfer::process(const ParseRequest& parseRequest)
{
    std::cout << "[generic::Transfer] " << std::endl;

    //bfxAPI.transfer(parseRequest.quantity,);
	std::cout << bfxAPI.strResponse() << std::endl;
    
    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

std::string Withdraw::process(const ParseRequest& parseRequest)
{
    std::cout << "[generic::Withdraw] " << std::endl;

    //bfxAPI.withdraw();
	std::cout << bfxAPI.strResponse() << std::endl;
    
    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// Historical data //
// Get balances history
std::string GetBalanceHistory::process(const ParseRequest& parseRequest)
{
    std::cout << "[generic::GetBalanceHistory] " << std::endl;

    bfxAPI.getBalanceHistory(parseRequest.currency, 0L, 0L, parseRequest.limit, "all");
	std::cout << bfxAPI.strResponse() << std::endl;
    
    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// Get withdrawl history
std::string GetWithdrawalHistory::process(const ParseRequest& parseRequest)
{
    std::cout << "[generic::GetWithdrawalHistory] " << std::endl;

    bfxAPI.getBalanceHistory(parseRequest.currency, 0L, 0L, parseRequest.limit, "all");
	std::cout << bfxAPI.strResponse() << std::endl;
    
    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

// Get past trades
std::string GetPastTrades::process(const ParseRequest& parseRequest)
{
    std::cout << "[generic::GetPastTrades] " << std::endl;

    bfxAPI.getPastTrades(parseRequest.symbol, 0L, 0L, parseRequest.limit, false);
	std::cout << bfxAPI.strResponse() << std::endl;
    
    exchangeAnswer = bfxAPI.strResponse();
    return Engine::postAnyProcessing(parseRequest.ctRequestId.c_str());
}

std::string GetTicker::process(const ParseRequest& parseRequest)
{
	Json::Reader reader;

    std::cout << "[generic::GetTicker] " << std::endl;
	
	std::string symbol(parseRequest.symbol);
	std::transform(symbol.begin(), symbol.end(), symbol.begin(),::tolower);
    bfxAPI.getTicker(symbol);
	exchangeAnswer = bfxAPI.strResponse();
    	
	bool parsingSuccessful = reader.parse(bfxAPI.strResponse().c_str() , exchangeAnswer );     //parse process
    if ( !parsingSuccessful )
    {
        std::cout  << "Failed to parse"
               << reader.getFormattedErrorMessages();
        return 0;
    }

	std::cout << exchangeAnswer << std::endl;

    return Engine::postGetTickerProcessing(parseRequest.ctRequestId.c_str(), parseRequest.symbol.c_str());
}

