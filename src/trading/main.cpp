/*
Description: RayCore: C++ concurrent trading engine to place orders with multiple crypto exchanges 
Author: Lei Guang (Ray)
Date:	2019-02-18
Memo:   Dashedade
*/

#include "signalHandler.h"
#include "tradingFactory.h"
#include "raycore.h"
#include "binance.h"
#include "bitfinex.h"
#include "bittrex.h"
#include "bitstamp.h"
#include "kraken.h"
#include "coinbase.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#include <iostream>

#define MIN_ARG 			2


// Init DB environment
// TODO: load from .env
const std::string host="THIS_IS_THE_HOST_IP"; # e.g., 127.0.0.1
const std::string user="THIS_IS_THE_USER_NAME"; # e.g., trade_user
const std::string password="PASS_WORD"; # e.g., I_cannot_tell_you_my_password
const std::string db="database_schema"; # e.g., db_trading

std::string _dbConnectString = "host=" + host + ";user=" + user + ";password=" + password + ";db="+db +";compress=true;auto-reconnect=true";
std::string _dbConnectStringNoPass = "host=" + host + ";user=" + user + ";password=xxxx"+";db="+db +";compress=true;auto-reconnect=true";

void showUsage(char *programName)
{
	std::cout << "Usage: " << programName << " portNumber" << std::endl;
	std::cout << "\t " << std::endl;
	//std::cout << "Usage: " << programName << " portNumber MySqlConnectionString"  << std::endl;	
	//std::cout << "\tMySqlConnectionString: 'host=HOST;port=3306;db=DeckLink;user=USER;password=USER_password;compress=true;auto-reconnect=true'" << std::endl;
	//std::cout << "\tExample development:   'host=35.225.200.25;port=3306;db=trading;user=root;password=Bitcoin2019!!;compress=true;auto-reconnect=true'" << std::endl;
}

int main(int argc, char *argv[])
{
	#pragma message "Googletest: https://github.com/google/googletest/tree/master/googletest"		

	int iArgc = 0;
	if (argc < MIN_ARG)
		showUsage(argv[iArgc]);
	else
	{	
		const unsigned short port=::atoi(argv[++iArgc]);
		//const char* dbConnectString = argv[++iArgc];
		
		// Print out runtime environment    
        std::cout << "++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
        std::cout << "++++++ Starting Crypto Trading Engine ++++++" << std::endl;
        std::cout << "++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
        std::cout << "Port: " << argv[1] << std::endl;
        std::cout << "DB param: " << _dbConnectStringNoPass << std::endl;

		// Activate Exchange/API calls
		Trading::Factory tradingFactory(port, _dbConnectString.c_str());

		//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// Raycore EXCHANGE
		//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// [RAYCORE API]: portfolio 
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Raycore::account::GetPortfolio));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Raycore::account::GetBalanceAsset));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Raycore::account::GetBalanceExchange));
		
		// [BINANCE API]: send_order
        tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Binance::order::SendLimitOrder));
        tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Binance::order::SendMarketOrder));
        // tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Binance::order::SendIcebergOrder));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Binance::order::SendTakeProfitLimitOrder));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Binance::order::SendStopLossLimitOrder));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Binance::send_order::Limit_Maker));

		// Binance currently  support LIMIT, LIMIT_MAKER, MARKET, STOP_LOSS_LIMIT, TAKE_PROFIT_LIMIT only.
		// Check https://api.binance.com/api/v1/exchangeInfo to find out what orderTypes are supported.
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Binance::send_order::Take_Profit));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Binance::send_order::Stop_Loss));

		// [BINANCE API]: cancel_order
        tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Binance::order::CancelOneOrder));
        tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Binance::order::CancelAllOrders));
        tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Binance::order::CancelAllLimitOrders));
        tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Binance::order::CancelAllStopLimitOrders));
       
		// [BINANCE API]: get_order
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Binance::order::GetOrderStatus));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Binance::order::GetAllOpenOrders));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Binance::order::GetAllOrders));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Binance::order::GetTradeHistory));

		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Binance::order::GetOrderBook));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Binance::order::GetRecentTrades));

        // [BINANCE API]: generic
        //tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Binance::generic::ServerTime));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Binance::generic::GetTicker));
        tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Binance::account::GetBalance));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Binance::account::GetPortfolio));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Binance::account::GetDepositHistory));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Binance::account::GetWithdrawHistory));

		//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // BITFINEX EXCHANGE
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// [BITFINEX API]: generic
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitfinex::account::GetBalance));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitfinex::account::GetPortfolio));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitfinex::generic::GetTicker));

		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitfinex::generic::GetAccountFees));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitfinex::generic::GetSummary));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitfinex::generic::GetKeyPermissions));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitfinex::generic::GetMarginInfo));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitfinex::generic::GetBalances));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitfinex::generic::GetBalanceHistory));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitfinex::generic::GetWithdrawalHistory));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitfinex::generic::GetPastTrades));
		//tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitfinex::generic::Deposit));
		//tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitfinex::generic::Transfer));
		//tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitfinex::generic::Withdraw));
	
		// [BITFINEX API]: order
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitfinex::order::GetOrderBook));

		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitfinex::order::GetTradeHistory));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitfinex::order::SendMarketOrder));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitfinex::order::SendLimitOrder));

		// [BITFINEX API]: get order status
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitfinex::order::GetOrderStatus));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitfinex::order::GetAllOpenOrders));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitfinex::order::GetAllOrders));
 
 		// [BITFINEX API]: cancel order
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitfinex::order::CancelOneOrder));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitfinex::order::CancelAllOrders));

		// [BITFINEX API]: Margin Funding
		// #pragma message "!!! Bitfinex margin funding NOT tested !!!"
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitfinex::marginfunding::NewOffer));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitfinex::marginfunding::CancelOffer));

		//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // BITTREX EXCHANGE
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// [BITTREX API]: generic
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bittrex::generic::GetTicker));
        tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bittrex::order::GetOrderBook));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bittrex::generic::GetMarkets));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bittrex::generic::GetCurrencies));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bittrex::generic::GetMarketSummaries));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bittrex::generic::GetMarketSummary));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bittrex::generic::GetMarketHistory));

		// [BITTREX API]: account
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bittrex::account::GetBalance));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bittrex::account::GetPortfolio));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bittrex::account::GetDepositAddress));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bittrex::account::GetWithdrawHistory));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bittrex::account::GetDepositHistory));

		// [BITTREX API]: order
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bittrex::order::SendLimitOrder));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bittrex::order::CancelOneOrder));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bittrex::order::CancelAllOrders));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bittrex::order::GetOrderStatus));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bittrex::order::GetAllOpenOrders));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bittrex::order::GetAllOrders));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bittrex::order::GetTradeHistory));

		// [BITSTAMP API]: generic
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitstamp::generic::GetTicker));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitstamp::generic::GetMarkets));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitstamp::generic::GetTradingPairs));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitstamp::generic::GetEURUSD));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitstamp::generic::GetHourlyMarkets));
        // tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitstamp::generic::GetTransactions));
		
		// [BITSTAMP API]: account 
        tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitstamp::account::GetBalance));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitstamp::account::GetPortfolio));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitstamp::account::GetMyTrades));

		// [BITSTAMP API]: orders 
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitstamp::order::SendLimitOrder));
        tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitstamp::order::SendMarketOrder));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitstamp::order::SendInstantOrder));

        tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitstamp::order::GetAllOpenOrders));
        tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitstamp::order::GetOrderStatus));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitstamp::order::GetAllOrders));

        tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitstamp::order::CancelOneOrder));
        tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitstamp::order::CancelAllOrders));
        tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitstamp::order::GetOrderBook));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitstamp::order::GetTradeHistory));

		// [KRAKEN API]: generic
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Kraken::generic::GetTicker));

		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Kraken::generic::GetAssetInfo));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Kraken::generic::GetAssetPairs));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Kraken::generic::GetTrades));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Kraken::generic::GetSpread));
		
		// [KRAKEN API]: account
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Kraken::account::GetBalance));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Kraken::account::GetTradeBalance));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Kraken::account::GetPortfolio));
		
		// [KRAKEN API]: order 
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Kraken::order::GetOrderStatus));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Kraken::order::GetTradeHistory));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Kraken::order::GetAllOpenOrders));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Kraken::order::GetClosedOrders));
        tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Kraken::order::GetAllOrders));

		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Kraken::trade::QueryTrades));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Kraken::trade::GetOpenPositions));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Kraken::order::GetLedgerInfo));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Kraken::order::QueryLedgers));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Kraken::trade::GetTradeVolume));

		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Kraken::order::SendMarketOrder));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Kraken::order::SendLimitOrder));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Kraken::order::CancelOneOrder));
        tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Kraken::order::GetOrderBook));

		// [COINBASE API]: generic
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Coinbase::generic::GetTicker));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Coinbase::generic::GetProductTrades));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Coinbase::generic::GetCurrencies));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Coinbase::generic::GetProducts));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Coinbase::generic::GetProductOrderBook));
		
		// [COINBASE API]: account
        tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Coinbase::account::GetBalance));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Coinbase::account::GetPortfolio));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Coinbase::account::GetAccountHistory));
		// tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Coinbase::account::GetHolds));
		
		// [COINBASE API]: order
        tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Coinbase::order::SendMarketOrder));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Coinbase::order::SendLimitOrder));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Coinbase::order::SendStopOrder));

		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Coinbase::order::CancelOneOrder));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Coinbase::order::CancelAllOrders));

		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Coinbase::order::GetOrderStatus));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Coinbase::order::GetAllOpenOrders));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Coinbase::order::GetAllOrders));
        tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Coinbase::order::GetOrderBook));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Coinbase::order::GetTradeHistory));


		// Signal handler
		util::SignalHandler signalHandler(tradingFactory);
		
		// Print out active API calls per exchange   
		std::cout << "\n++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
		std::cout << "++++++++ Active Exchange/API calls: ++++++++" << std::endl;
		std::cout << "++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
		tradingFactory.showActiveAPI();
		
		std::cout << "Press <CTRL+C> to stop" << std::endl;
		
		tradingFactory.start();		
	}	

	return 0;
}

