#include "mySqlStore.h"

#include <Poco/Mutex.h>
#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionPool.h>
#include <Poco/Thread.h>
#include <Poco/Tuple.h>

#include <queue>
#include <iostream>
#include <vector>
#include <tuple>

#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>

using namespace util;

/******************************************************************************/
class RegisterConnector
{
public:
    RegisterConnector()
    {
        Poco::Data::MySQL::Connector::registerConnector();    
    }

    virtual ~RegisterConnector()
    {
        if (nullptr != sessions.get())
            sessions->shutdown();

        Poco::Data::MySQL::Connector::unregisterConnector();    
    }

    public:
        Poco::Mutex                                 mutex;
        Poco::SharedPtr<Poco::Data::SessionPool>    sessions;    
};
static RegisterConnector registerConnector;


/******************************************************************************/
void MySqlStore::initDB(const char* connectString, const int minSessions, const int maxSessions)
{
    poco_assert(nullptr == registerConnector.sessions.get());
    
    registerConnector.sessions.assign(new Poco::Data::SessionPool("MySql", connectString, minSessions, maxSessions));    
}

MySqlStore::MySqlStore()
{
    poco_assert(nullptr != registerConnector.sessions.get());
        
    while (nullptr == session.get())
    {
        try
        {
            Poco::Mutex::ScopedLock lock(registerConnector.mutex);
            session.assign(new Poco::Data::Session(registerConnector.sessions->get()));
        }
        catch(Poco::Data::SessionPoolExhaustedException& e)
        {
            Poco::Thread::yield();                
        }    
    }
}

void MySqlStore::fetchInfo(int userID, const char* exchangeName)
{
    poco_assert(nullptr != session.get());

    // Get ID
    Poco::Data::Statement select(*session);

    select  << "SELECT apikey, apisecret FROM exchange_API WHERE user_id=? AND exchange_name=?",
                    Poco::Data::Keywords::into(this->key),
                    Poco::Data::Keywords::into(this->secret),
                    Poco::Data::Keywords::use(userID),
                    Poco::Data::Keywords::bind(exchangeName);

    select.execute();
}

int MySqlStore::fetchUserID(int userID)
{
	poco_assert(nullptr != session.get());

	int id = 0;
	Poco::Data::Statement select(*session);
	select << "SELECT distinct user_id from exchange_API where user_id =?",
				Poco::Data::Keywords::into(id),
				Poco::Data::Keywords::use(userID);
	select.execute();
	return id;

}

std::string MySqlStore::fetchExchangeUID(int userID, const char* exchangeName)
{
    poco_assert(nullptr != session.get());

    // Get ID
    std::string misc = "";
    Poco::Data::Statement select(*session);

    select  << "SELECT distinct misc from exchange_API where user_id=? AND exchange_name=?",
                    Poco::Data::Keywords::into(misc),
                    Poco::Data::Keywords::use(userID),
                    Poco::Data::Keywords::bind(exchangeName);

    select.execute();
    return misc;
}

void MySqlStore::saveOrderRequest(const std::string& ctOrderID, const std::string& requestType, const std::string& subType, const int userID, const std::string& exchange, const std::string& symbol, const std::string& orderSide, const double quantity, const double price, const std::string& timeInForce)
{
    poco_assert(nullptr != session.get());
    
    // Insert order request
    Poco::Data::Statement sql_insert(*session);
	
	Poco::DateTime now_time;
	std::cout << "[DB UPDATE - request]: " << ctOrderID << ' ' << requestType << ' ' << userID<< ' ' << exchange
                                          << ' ' << symbol << ' ' << orderSide << ' ' << subType << ' ' 
                                          << quantity << ' ' << price << ' ' << timeInForce << ' ' <<std::endl;

	sql_insert << "INSERT into traderequest (ctOrderID, requestType, subType, userID,exchange, symbol, orderSide, quantity, price, timeInForce,reqReceived_at) \
                   VALUES (?,?,?,?,?,?,?,?,?,?,?)", 
				   Poco::Data::Keywords::bind(ctOrderID),
                   Poco::Data::Keywords::bind(requestType),
                   Poco::Data::Keywords::bind(subType),
                   Poco::Data::Keywords::bind(userID),
                   Poco::Data::Keywords::bind(exchange),
                   Poco::Data::Keywords::bind(symbol), 
                   Poco::Data::Keywords::bind(orderSide),
                   Poco::Data::Keywords::bind(quantity),
                   Poco::Data::Keywords::bind(price), 
                   Poco::Data::Keywords::bind(timeInForce),
                   Poco::Data::Keywords::bind(now_time);

    sql_insert.execute();
}
 
void MySqlStore::saveOrderAnswer(const std::string& ctOrderID, const std::string& status, const std::string& orderId, const std::string& clientOrderId)
{
    poco_assert(nullptr != session.get());

    Poco::Data::Statement sql_insert(*session);

    Poco::DateTime now_time;
    std::cout << "[DB UPDATE - reply]: " << status << ' ' << orderId << ' ' << clientOrderId <<std::endl;

    sql_insert << "UPDATE trades SET status=?, orderId=?, clientOrderId=?, lastUpdated=? WHERE ctOrderID=?", 
				   Poco::Data::Keywords::bind(status),
                   Poco::Data::Keywords::bind(orderId),
                   Poco::Data::Keywords::bind(clientOrderId),
                   Poco::Data::Keywords::bind(now_time),
                   Poco::Data::Keywords::bind(ctOrderID);

    sql_insert.execute();

}

void MySqlStore::saveOrderAnswerError(const std::string& ctOrderID, const int code, const std::string& msg)
{
    poco_assert(nullptr != session.get());

    Poco::Data::Statement sql_insert(*session);
    
    Poco::DateTime now_time;
    std::cout << "[DB UPDATE - reply]: " << code << ' ' << msg <<std::endl;

    sql_insert << "UPDATE trades SET code=?, msg=?,  lastUpdated=? WHERE ctOrderID=?", 
				   Poco::Data::Keywords::bind(code),
                   Poco::Data::Keywords::bind(msg),
                   Poco::Data::Keywords::bind(now_time),
                   Poco::Data::Keywords::bind(ctOrderID);

    sql_insert.execute();

}

void MySqlStore::saveDepositHistory(const std::string& uid, const std::string& exchange, const std::string& asset, double amount, const std::string& date, const std::string& txid)
{
    poco_assert(nullptr != session.get());
    
    // Insert order request
    Poco::Data::Statement sql_insert(*session);
	
	Poco::DateTime now_time;
	std::cout << "[DB UPDATE - request]: " << uid << ' ' << exchange << ' ' << asset << ' ' << amount
                                          << ' ' << date <<std::endl;

	sql_insert << "INSERT into deposit_history (uid, exchange, asset, amount, date, txid) \
                   VALUES (?,?,?,?,?,?)", 
				   Poco::Data::Keywords::bind(uid),
                   Poco::Data::Keywords::bind(exchange),
                   Poco::Data::Keywords::bind(asset),
                   Poco::Data::Keywords::bind(amount),
                   Poco::Data::Keywords::bind(date),
				   Poco::Data::Keywords::bind(txid);

    sql_insert.execute();

}

void MySqlStore::saveWithdrawHistory(const std::string& uid, const std::string& exchange, const std::string& asset, double amount, const std::string& date, const std::string& txid)
{
    poco_assert(nullptr != session.get());
    
    // Insert order request
    Poco::Data::Statement sql_insert(*session);
	
	Poco::DateTime now_time;
	std::cout << "[DB UPDATE - request]: " << uid << ' ' << exchange << ' ' << asset << ' ' << amount
                                          << ' ' << date <<std::endl;

	sql_insert << "INSERT into withdraw_history (uid, exchange, asset, amount, date, txid) \
                   VALUES (?,?,?,?,?,?)", 
				   Poco::Data::Keywords::bind(uid),
                   Poco::Data::Keywords::bind(exchange),
                   Poco::Data::Keywords::bind(asset),
                   Poco::Data::Keywords::bind(amount),
                   Poco::Data::Keywords::bind(date),
				   Poco::Data::Keywords::bind(txid);

    sql_insert.execute();

}

void MySqlStore::clearAccountBalance(const int uid, const std::string& exchange)
{
    poco_assert(nullptr != session.get());

    Poco::Data::Statement sql_delete(*session);
    Poco::DateTime now_time;

    std::cout << "[DB - delete existing balance] ... " << exchange << std::endl;
    sql_delete << "DELETE from account_balance where uid=? and exchange=? ",
            Poco::Data::Keywords::bind(uid),
            Poco::Data::Keywords::bind(exchange);

    sql_delete.execute();
}


void MySqlStore::updateAccountBalance(const int uid, const std::string& exchange, const std::string& asset, double amount)
{
    poco_assert(nullptr != session.get());

    Poco::Data::Statement sql_delete(*session);
    Poco::DateTime now_time;
	// std::cout << "[DB - updateAccountBalance]: " << uid << ' ' << exchange << ' ' << asset << ' ' << amount << std::endl;
	
    // std::cout << "[DB - delete existing balance...]" << std::endl;
    sql_delete << "DELETE from account_balance where uid=? and exchange=? and asset=? ",
            Poco::Data::Keywords::bind(uid),
            Poco::Data::Keywords::bind(exchange),
			Poco::Data::Keywords::bind(asset);

    sql_delete.execute();

    // std::cout << "[DB - insert new balance...]" << std::endl;
    Poco::Data::Statement sql_insert(*session);
    sql_insert << "INSERT into account_balance (uid, exchange, asset, amount) \
                   VALUES (?,?,?,?)", 
				   Poco::Data::Keywords::bind(uid),
                   Poco::Data::Keywords::bind(exchange),
                   Poco::Data::Keywords::bind(asset),
                   Poco::Data::Keywords::bind(amount);          
    sql_insert.execute();
}

double MySqlStore::selectHistoryPrice(const std::string& asset, const std::string& date)
{
    poco_assert(nullptr != session.get());
    double price(0);
    
    // Insert order request
    Poco::Data::Statement sql_select(*session);
	
	Poco::DateTime now_time;
	std::cout << "[DB - selectHistoryPrice]: " << asset << ' ' << date << std::endl;

	if (asset == "") {
		price = 0;
    } else {
   		sql_select << "SELECT close FROM historyprice_asset WHERE date=? and asset=?",
                Poco::Data::Keywords::into(price),
                Poco::Data::Keywords::bind(time),
				Poco::Data::Keywords::bind(asset);
        sql_select.execute();
   	}	
    
    std::cout << "asset: " << asset << " price: " << price << std::endl;

    return price;
}

std::string MySqlStore::selectFundingDate(const std::string&uid, const std::string& exchange, const std::string& asset)
{
    poco_assert(nullptr != session.get());
    std::string date("");
    
    // Insert order request
    Poco::Data::Statement sql_select(*session);
	
	Poco::DateTime now_time;
	std::cout << "[DB REQ - selectFundingDate]: " << exchange << ' ' << asset  << ' ' <<  std::endl;

	
    sql_select << "SELECT date FROM deposit_history WHERE uid=? and exchange=? and asset=?",
                Poco::Data::Keywords::into(date),
                Poco::Data::Keywords::bind(uid),
                Poco::Data::Keywords::bind(exchange),
				Poco::Data::Keywords::bind(asset);
  
    
    sql_select.execute();
    std::cout << "EXCHANGE: " << exchange << " asset: " << asset << " date: " << date << std::endl;

    return date;
}

Json::Value MySqlStore::selectPortfolio( const int uid, Json::Value exchangeAnswer)
{
     Json::Value element;

    poco_assert(nullptr != session.get());
    Poco::Data::Statement sql_select(*session);

    /* -- Populate portfolio
     select bal.uid, bal.exchange, bal.asset, bal.amount, dh.date, ha.close from trading.account_balance as bal
     join trading.deposit_history dh
     join trading.historyprice_asset ha
     where bal.asset = dh.asset
     and dh.date = ha.date
     and dh.asset = ha.asset
     and bal.exchange = dh.exchange
	*/

	Poco::DateTime now_time;
	std::cout << "[DB REQ - selectPortfolio]: " << uid << ' ' <<  std::endl;

    /*sql_select << "INSERT into account_balance (uid, exchange, asset, amount) \
                    VALUES (?,?,?,?)", 
	 			   Poco::Data::Keywords::bind(uid),
                   Poco::Data::Keywords::bind(exchange),
                   Poco::Data::Keywords::bind(asset),
                   Poco::Data::Keywords::bind(amount);
                   
     sql_insert.execute();   
	*/

	return exchangeAnswer;

}

Json::Value MySqlStore::selectBalanceByAsset(int uid)
{
    typedef Poco::Tuple<std::string, double> Balance;
    typedef std::vector<Balance> AssetBalance;
	AssetBalance balance;
	
    Json::Value element;
	int i(0);

    poco_assert(nullptr != session.get());
    Poco::Data::Statement sql_select(*session);

    // -- aggregate by assets
    // select asset, sum(amount)  from trading.account_balance
    // where amount <> 0
    // group by asset

    sql_select  << "SELECT asset, sum(amount) as amount from account_balance where amount<>0 and uid = ? group by asset",
                    Poco::Data::Keywords::into(balance),
                    Poco::Data::Keywords::bind(uid);

    sql_select.execute();
    
	std::cout << "[DB REQ - selectBalanceByAsset]: " << uid << ' ' <<  std::endl;
	
	for ( AssetBalance::const_iterator it = balance.begin(); it != balance.end(); ++it ) {
		//std::cout << "asset " << it->get<0>() << " amount: "<< it->get<1>() << std::endl;
		element[i]["asset"]		= it->get<0>();
		element[i]["amount"] 	= it->get<1>();
		i++;
	}
	
	//std::cout << element << std::endl;
	return element;
}

Json::Value MySqlStore::selectBalanceByExchange(int uid)
{
    typedef Poco::Tuple<std::string, std::string, double> Balance;
	typedef std::vector<Balance> ExchangeBalance;
	ExchangeBalance balance;
	
    Json::Value element;
    int i(0);

	poco_assert(nullptr != session.get());
    Poco::Data::Statement sql_select(*session);
    
    // -- balance aggregate by exchange 
    // // select exchange, asset, amount from trading.account_balance
    // // where amount <> 0

    sql_select  << "SELECT exchange, asset, amount from account_balance where amount <> 0 and uid = ?",
                     Poco::Data::Keywords::into(balance),
                     Poco::Data::Keywords::use(uid);

    sql_select.execute();
	std::cout << "[DB REQ - selectBalanceByAssets]: " << uid << ' ' <<  std::endl;

    for ( ExchangeBalance::const_iterator it = balance.begin(); it != balance.end(); ++it ) {
        //std::cout << "exchange " << it->get<0>() << " asset: "<< it->get<1>() << " amount: " << it->get<2>() << std::endl;
    	
		element[i]["exchange"] 	= it->get<0>();
        element[i]["asset"]     = it->get<1>();
		element[i]["amount"]	= it->get<2>();
        i++;
	}

	return element;
}


// ***********************************************************
// Orders and Trades
void MySqlStore::deleteTradeHistory_Bitfinex(const int uid, const std::string& exchange)
{
    poco_assert(nullptr != session.get());

    Poco::Data::Statement sql_delete(*session);
    Poco::DateTime now_time;
	std::cout << "[DB - deleteTradeHistory_Bitfinex]: " << uid << ' ' << exchange << std::endl;

    sql_delete << "DELETE from tradehistory_bitfinex where uid=? and exchange=?",
            Poco::Data::Keywords::bind(uid),
            Poco::Data::Keywords::bind(exchange);
    sql_delete.execute();
}

void MySqlStore::deleteTradeHistory_Kraken(const int uid)
{
    poco_assert(nullptr != session.get());

    Poco::Data::Statement sql_delete(*session);
    Poco::DateTime now_time;
	std::cout << "[DB - deleteTradeHistory_Kraken]: " << uid << ' ' << std::endl;

    sql_delete << "DELETE from tradehistory_kraken where uid=?",
            Poco::Data::Keywords::bind(uid);
            
    sql_delete.execute();
}

void MySqlStore::deleteTradeHistory_Binance(const int uid)
{
    poco_assert(nullptr != session.get());

    Poco::Data::Statement sql_delete(*session);
    Poco::DateTime now_time;
	std::cout << "[DB - deleteTradeHistory_Binance]: " << uid << ' ' << std::endl;

    sql_delete << "DELETE from tradehistory_binance where uid=?",
            Poco::Data::Keywords::bind(uid);
            
    sql_delete.execute();
}

void MySqlStore::deleteTradeHistory_Bitstamp(const int uid)
{
    poco_assert(nullptr != session.get());

    Poco::Data::Statement sql_delete(*session);
    Poco::DateTime now_time;
	std::cout << "[DB - deleteTradeHistory_Bitstamp]: " << uid << ' ' << std::endl;

    sql_delete << "DELETE from tradehistory_bitstamp where uid=?",
            Poco::Data::Keywords::bind(uid);
            
    sql_delete.execute();
}

void MySqlStore::deleteTradeHistory_Bittrex(const int uid)
{
    poco_assert(nullptr != session.get());

    Poco::Data::Statement sql_delete(*session);
    Poco::DateTime now_time;
	std::cout << "[DB - deleteTradeHistory_Bittrex]: " << uid << ' ' << std::endl;

    sql_delete << "DELETE from tradehistory_bittrex where uid=?",
            Poco::Data::Keywords::bind(uid);
            
    sql_delete.execute();
}

void MySqlStore::deleteTradeHistory_Coinbase(const int uid)
{
    poco_assert(nullptr != session.get());

    Poco::Data::Statement sql_delete(*session);
    Poco::DateTime now_time;
	std::cout << "[DB - deleteTradeHistory_Coinbase]: " << uid << ' ' << std::endl;

    sql_delete << "DELETE from tradehistory_coinbase where uid=?",
            Poco::Data::Keywords::bind(uid);
            
    sql_delete.execute();
}

void MySqlStore::updateTradeHistory_Bitfinex(const int uid, const std::string& exchange, const std::string& asset, double amount, double price, const std::string& type, const std::string& order_id, const std::string& tid, const double fee_amount, const std::string& fee_currency, const std::string& time)
{
    poco_assert(nullptr != session.get());

    Poco::Data::Statement sql_delete(*session);
    Poco::DateTime now_time;
	// std::cout << "[DB - updateTradeHistory_Bitfinex]: " << uid << ' ' << exchange << ' ' << asset << ' ' << amount << ' ' << price << ' ' << time << std::endl;
	
	std::time_t t = std::stof(time);
	std::stringstream sstime;
	sstime << std::put_time(std::gmtime(&t), "%F") << '\n';

    Poco::Data::Statement sql_insert(*session);
    sql_insert << "INSERT into tradehistory_bitfinex (uid, exchange, asset, amount, price, type, order_id, tid, fee_amount, fee_currency, time) \
                   VALUES (?,?,?,?,?,?,?,?,?,?,?)", 
				   Poco::Data::Keywords::bind(uid),
                   Poco::Data::Keywords::bind(exchange),
                   Poco::Data::Keywords::bind(asset),
                   Poco::Data::Keywords::bind(amount),
				   Poco::Data::Keywords::bind(price),
				   Poco::Data::Keywords::bind(type),
				   Poco::Data::Keywords::bind(order_id),
				   Poco::Data::Keywords::bind(tid),
				   Poco::Data::Keywords::bind(fee_amount),
				   Poco::Data::Keywords::bind(fee_currency),
				   Poco::Data::Keywords::bind(sstime.str());
                   
    sql_insert.execute();
	poco_assert (sql_insert.done());
}

void MySqlStore::updateTradeHistory_Kraken(const int uid, const std::string& asset, double amount, double price, const std::string& type, const std::string& order_id, const std::string& tid, const double fee_amount, const std::string& fee_currency, const std::string& time)
{
    poco_assert(nullptr != session.get());

    Poco::Data::Statement sql_delete(*session);
    Poco::DateTime now_time;
	// std::cout << "[DB - updateTradeHistory_Kraken]: " << uid << ' ' << asset << ' ' << amount << ' ' << price << ' ' << time << std::endl;
	
	std::time_t t = std::stof(time);
	std::stringstream sstime;
	sstime << std::put_time(std::gmtime(&t), "%F") << '\n';

    Poco::Data::Statement sql_insert(*session);
    sql_insert << "INSERT into tradehistory_kraken (uid, asset, amount, price, type, order_id, tid, fee_amount, fee_currency, time) \
                   VALUES (?,?,?,?,?,?,?,?,?,?)", 
				   Poco::Data::Keywords::bind(uid),
                   Poco::Data::Keywords::bind(asset),
                   Poco::Data::Keywords::bind(amount),
				   Poco::Data::Keywords::bind(price),
				   Poco::Data::Keywords::bind(type),
				   Poco::Data::Keywords::bind(order_id),
				   Poco::Data::Keywords::bind(tid),
				   Poco::Data::Keywords::bind(fee_amount),
				   Poco::Data::Keywords::bind(fee_currency),
				   Poco::Data::Keywords::bind(sstime.str());
                   
    sql_insert.execute();
	poco_assert (sql_insert.done());
}

void MySqlStore::updateTradeHistory_Coinbase(const int uid, const std::string& asset, double amount, double price, const std::string& type, const std::string& order_id, const std::string& tid, const double fee_amount, const std::string& fee_currency, const std::string& time)
{
    poco_assert(nullptr != session.get());

    Poco::Data::Statement sql_delete(*session);
    Poco::DateTime now_time;
    // std::cout << "[DB - updateTradeHistory_Coinbase]: " << uid << ' ' << asset << ' ' << amount << ' ' << price << ' ' << time << std::endl;

    std::time_t t = std::stof(time);
    std::stringstream sstime;
    sstime << std::put_time(std::gmtime(&t), "%F") << '\n';

    Poco::Data::Statement sql_insert(*session);
    sql_insert << "INSERT into tradehistory_coinbase (uid, asset, amount, price, type, order_id, tid, fee_amount, fee_currency, time) \
                   VALUES (?,?,?,?,?,?,?,?,?,?)",
                   Poco::Data::Keywords::bind(uid),
                   Poco::Data::Keywords::bind(asset),
                   Poco::Data::Keywords::bind(amount),
                   Poco::Data::Keywords::bind(price),
                   Poco::Data::Keywords::bind(type),
                   Poco::Data::Keywords::bind(order_id),
                   Poco::Data::Keywords::bind(tid),
                   Poco::Data::Keywords::bind(fee_amount),
                   Poco::Data::Keywords::bind(fee_currency),
                   Poco::Data::Keywords::bind(sstime.str());

    sql_insert.execute();
    poco_assert (sql_insert.done());
}


void MySqlStore::updateTradeHistory_Bittrex(const int uid, const std::string& asset, double amount, double price, const std::string& type, const std::string& order_id, const std::string& tid, const double fee_amount, const std::string& fee_currency, const std::string& time)
{
    poco_assert(nullptr != session.get());

    Poco::Data::Statement sql_delete(*session);
    Poco::DateTime now_time;
    // std::cout << "[DB - updateTradeHistory_Bittrex]: " << uid << ' ' << asset << ' ' << amount << ' ' << price << ' ' << time << std::endl;

    std::time_t t = std::stof(time);
    std::stringstream sstime;
    sstime << std::put_time(std::gmtime(&t), "%F") << '\n';
	
	// std::cout << "[DB REQ: updateTradeHistory_Bittrex ]" << uid << asset << amount << price << type << order_id << tid << fee_amount << fee_currency << time << std::endl;

    Poco::Data::Statement sql_insert(*session);
    sql_insert << "INSERT into tradehistory_bittrex (uid, asset, amount, price, type, order_id, tid, fee_amount, fee_currency, time) \
                   VALUES (?,?,?,?,?,?,?,?,?,?)",
                   Poco::Data::Keywords::bind(uid),
                   Poco::Data::Keywords::bind(asset),
                   Poco::Data::Keywords::bind(amount),
                   Poco::Data::Keywords::bind(price),
                   Poco::Data::Keywords::bind(type),
                   Poco::Data::Keywords::bind(order_id),
                   Poco::Data::Keywords::bind(tid),
                   Poco::Data::Keywords::bind(fee_amount),
                   Poco::Data::Keywords::bind(fee_currency),
                   Poco::Data::Keywords::bind(sstime.str());

    sql_insert.execute();
    poco_assert (sql_insert.done());
}


void MySqlStore::updateTradeHistory_Bitstamp(const int uid, const std::string& asset, double amount, double price, const std::string& type, const std::string& order_id, const std::string& tid, const double fee_amount, const std::string& fee_currency, const std::string& time)
{
    poco_assert(nullptr != session.get());

    Poco::Data::Statement sql_delete(*session);
    Poco::DateTime now_time;
    std::cout << "[DB - updateTradeHistory_Bitstamp]: " << uid << ' ' << asset << ' ' << amount << ' ' << price << ' ' << time << std::endl;

    std::time_t t = std::stof(time);
    std::stringstream sstime;
    sstime << std::put_time(std::gmtime(&t), "%F") << '\n';
	
	// std::cout << "[DB REQ: updateTradeHistory_Bittrex ]" << uid << asset << amount << price << type << order_id << tid << fee_amount << fee_currency << time << std::endl;

    Poco::Data::Statement sql_insert(*session);
    sql_insert << "INSERT into tradehistory_bittrex (uid, asset, amount, price, type, order_id, tid, fee_amount, fee_currency, time) \
                   VALUES (?,?,?,?,?,?,?,?,?,?)",
                   Poco::Data::Keywords::bind(uid),
                   Poco::Data::Keywords::bind(asset),
                   Poco::Data::Keywords::bind(amount),
                   Poco::Data::Keywords::bind(price),
                   Poco::Data::Keywords::bind(type),
                   Poco::Data::Keywords::bind(order_id),
                   Poco::Data::Keywords::bind(tid),
                   Poco::Data::Keywords::bind(fee_amount),
                   Poco::Data::Keywords::bind(fee_currency),
                   Poco::Data::Keywords::bind(sstime.str());

    sql_insert.execute();
    poco_assert (sql_insert.done());
}
// ***********************************************************
// Portfolio
Json::Value MySqlStore::calcPortfolioCostBitfinex ( const int uid, const std::string& exchange)
{
	typedef Poco::Tuple<std::string, double, double> Cost;
    typedef std::vector<Cost> AssetCost;
    AssetCost portfolio;

    Json::Value element;
    int i(0);
	double total_cost(0.0), total_amount(0.0);
	std::string message;

    poco_assert(nullptr != session.get());
    Poco::Data::Statement sql_select(*session);
	
	std::cout << "[DB REQ - calcPortfolioCostBitfinex]: " << uid << ' ' << exchange << std::endl;
    sql_select  << "SELECT asset, sum(amount*price) as total_cost, sum(amount) as total_amount FROM tradehistory_bitfinex where uid = ? and amount > 0  group by asset",
                     Poco::Data::Keywords::into(portfolio),
                     Poco::Data::Keywords::bind(uid);

    sql_select.execute();
	poco_assert (sql_select.done());

    for ( AssetCost::const_iterator it = portfolio.begin(); it != portfolio.end(); ++it ) {
        std::cout << "asset " << it->get<0>() << " total_cost: "<< it->get<1>() << " total_amount: " << it->get<2>() << std::endl;
		total_cost = it->get<1>();
		total_amount = it->get<2>();
		
		if (total_amount <= 0){
			std::cout << "Net selling, no coin assets, all cash." << std::endl;
			message = "Net selling, all cash no coins.";
			element[i]["message"] = message;
			element[i]["asset"] 		= it->get<0>();
			element[i]["total_amount"]	= total_amount;
			element[i]["total_cost"]	= total_cost;
		} else {
			element[i]["asset"]  		= it->get<0>();
        	element[i]["average_cost"]	= total_cost / total_amount;
			element[i]["total_amount"]  = total_amount;
			element[i]["total_cost"]	= total_cost;
        	i++;	
		}
    }

    return element;
}

Json::Value MySqlStore::calcPortfolioCostKraken ( const int uid )
{
	typedef Poco::Tuple<std::string, double, double> Cost;
    typedef std::vector<Cost> AssetCost;
    AssetCost portfolio;

    Json::Value element;
    int i(0);
	double total_cost(0.0), total_amount(0.0);
	std::string message;

    poco_assert(nullptr != session.get());
    Poco::Data::Statement sql_select(*session);
	
	std::cout << "[DB REQ - calcPortfolioCostKraken]: " << uid << ' ' <<  std::endl;
    sql_select  << "SELECT asset, sum(price) as total_cost, sum(amount) as total_amount FROM tradehistory_kraken where uid = ? and amount > 0 group by asset limit 10",
                     Poco::Data::Keywords::into(portfolio),
                     Poco::Data::Keywords::bind(uid);


    sql_select.execute();
	poco_assert (sql_select.done());

    for ( AssetCost::const_iterator it = portfolio.begin(); it != portfolio.end(); ++it ) {
        std::cout << "asset " << it->get<0>() << " total_cost: "<< it->get<1>() << " total_amount: " << it->get<2>() << std::endl;
		total_cost = it->get<1>();
		total_amount = it->get<2>();
		
		if (total_amount <= 0){
			message = "Net selling";
			element[i]["message"] = message;
			element[i]["asset"] 		= it->get<0>();
			element[i]["total_amount"]	= total_amount;
			element[i]["total_cost"]	= total_cost;
		} else {
			element[i]["asset"]  		= it->get<0>();
        	element[i]["average_cost"]	= total_cost / total_amount;
			element[i]["total_amount"]  = total_amount;
			element[i]["total_cost"]	= total_cost;
        	i++;	
		}
    }

    return element;
}


Json::Value MySqlStore::calcPortfolioCostCoinbase ( const int uid )
{
	typedef Poco::Tuple<std::string, double, double> Cost;
    typedef std::vector<Cost> AssetCost;
    AssetCost portfolio;

    Json::Value element;
    int i(0);
	double total_cost(0.0), total_amount(0.0);
	std::string message;

    poco_assert(nullptr != session.get());
    Poco::Data::Statement sql_select(*session);
	
	std::cout << "[DB REQ - calcPortfolioCostCoinbase]: " << uid << ' ' <<  std::endl;
	
    sql_select  << "SELECT asset, sum(price*amount) as total_cost, sum(amount) as total_amount FROM tradehistory_coinbase where uid = ? group by asset limit 10",
                     Poco::Data::Keywords::into(portfolio),
                     Poco::Data::Keywords::bind(uid);
    sql_select.execute();
    poco_assert (sql_select.done());
    

    for ( AssetCost::const_iterator it = portfolio.begin(); it != portfolio.end(); ++it ) {
        std::cout << "asset " << it->get<0>() << " total_cost: "<< it->get<1>() << " total_amount: " << it->get<2>() << std::endl;
		total_cost = it->get<1>();
		total_amount = it->get<2>();
		
		if (total_amount <= 0){
			message = "Net selling";
			element[i]["message"] = message;
			element[i]["asset"] 		= it->get<0>();
			element[i]["total_amount"]	= total_amount;
			element[i]["total_cost"]	= total_cost;
			element[i]["average_cost"]  = total_cost / total_amount;
		} else {
			element[i]["asset"]  		= it->get<0>();
        	element[i]["average_cost"]	= total_cost / total_amount;
			element[i]["total_amount"]  = total_amount;
			element[i]["total_cost"]	= total_cost;
        	i++;	
		}
    }

	// std::cout << "calcPortfolioCostCoinbase: " << element << std::endl;

    return element;
}


Json::Value MySqlStore::calcPortfolioCostBittrex ( const int uid )
{
	typedef Poco::Tuple<std::string, double, double> Cost;
    typedef std::vector<Cost> AssetCost;
    AssetCost portfolio;

    Json::Value element;
    int i(0);
	double total_cost(0.0), total_amount(0.0);
	std::string message;

    poco_assert(nullptr != session.get());
    Poco::Data::Statement sql_select(*session);
	
	std::cout << "[DB REQ - calcPortfolioCostBittrex]: " << uid << ' ' <<  std::endl;
	
    sql_select  << "SELECT asset, sum(price*amount) as total_cost, sum(amount) as total_amount FROM tradehistory_bittrex where uid = ? group by asset limit 10",
                     Poco::Data::Keywords::into(portfolio),
                     Poco::Data::Keywords::bind(uid);
    sql_select.execute();
    poco_assert (sql_select.done());
    

    for ( AssetCost::const_iterator it = portfolio.begin(); it != portfolio.end(); ++it ) {
        // std::cout << "asset " << it->get<0>() << " total_cost: "<< it->get<1>() << " total_amount: " << it->get<2>() << std::endl;
		total_cost = it->get<1>();
		total_amount = it->get<2>();
		
		if (total_amount <= 0){
			message = "Net selling";
			element[i]["message"] = message;
			element[i]["asset"] 		= it->get<0>();
			element[i]["total_amount"]	= total_amount;
			element[i]["total_cost"]	= total_cost;
			element[i]["average_cost"]  = total_cost / total_amount;
		} else {
			element[i]["asset"]  		= it->get<0>();
        	element[i]["average_cost"]	= total_cost / total_amount;
			element[i]["total_amount"]  = total_amount;
			element[i]["total_cost"]	= total_cost;
        	i++;	
		}
    }

	// std::cout << "calcPortfolioCostBittrex: " << element << std::endl;

    return element;
}

Json::Value MySqlStore::calcPortfolioCostBitstamp ( const int uid )
{
	typedef Poco::Tuple<std::string, double, double> Cost;
    typedef std::vector<Cost> AssetCost;
    AssetCost portfolio;

    Json::Value element;
    int i(0);
	double total_cost(0.0), total_amount(0.0);
	std::string message;

    poco_assert(nullptr != session.get());
    Poco::Data::Statement sql_select(*session);
	
	std::cout << "[DB REQ - calcPortfolioCostBitstamp]: " << uid << ' ' <<  std::endl;
	
    sql_select  << "SELECT asset, sum(price*amount) as total_cost, sum(amount) as total_amount FROM tradehistory_bitstamp where uid = ? and amount > 0 group by asset limit 10",
                     Poco::Data::Keywords::into(portfolio),
                     Poco::Data::Keywords::bind(uid);
    sql_select.execute();
    poco_assert (sql_select.done());
    

    for ( AssetCost::const_iterator it = portfolio.begin(); it != portfolio.end(); ++it ) {
        // std::cout << "asset " << it->get<0>() << " total_cost: "<< it->get<1>() << " total_amount: " << it->get<2>() << std::endl;
		total_cost = it->get<1>();
		total_amount = it->get<2>();
		
		if (total_amount <= 0){
			message = "Net selling";
			element[i]["message"] = message;
			element[i]["asset"] 		= it->get<0>();
			element[i]["total_amount"]	= total_amount;
			element[i]["total_cost"]	= total_cost;
			element[i]["average_cost"]  = total_cost / total_amount;
		} else {
			element[i]["asset"]  		= it->get<0>();
        	element[i]["average_cost"]	= total_cost / total_amount;
			element[i]["total_amount"]  = total_amount;
			element[i]["total_cost"]	= total_cost;
        	i++;	
		}
    }

	// std::cout << "calcPortfolioCostBitstamp: " << element << std::endl;

    return element;
}
