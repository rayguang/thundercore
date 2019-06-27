#ifndef __MYSQLSTORE_H__
#define __MYSQLSTORE_H__
#include <iomanip>

#include "store.h"
#include <json/json.h>

#include <Poco/SharedPtr.h>
#include <Poco/Data/Session.h>

namespace util
{
    class MySqlStore : public Store
    {
    public:
        static void initDB(const char* connectString, const int minSessions, const int maxSessions);

        MySqlStore();
        virtual ~MySqlStore() override = default;

        // Select: Fetch user API key
        virtual void fetchInfo(int userID, const char* exchangeName) override;
		virtual int fetchUserID(int UserID);
        virtual std::string fetchExchangeUID(int userID, const char* exchangeName);

        // Insert: save request
        virtual void saveOrderRequest(const std::string& ctOrderID, const std::string& requestType, const std::string& subType, const int userID, const std::string& exchange, const std::string& symbol, const std::string& orderSide, const double quantity, const double price, const std::string& timeInForce) override;

        // Insert: save reply
        virtual void saveOrderAnswer(const std::string& ctOrderID, const std::string& status, const std::string& orderId, const std::string& clientOrderId) override;

        // Insert: save reply error    
        virtual void saveOrderAnswerError(const std::string& ctOrderID, const int code, const std::string& msg) override;

        // Balances
        virtual void saveDepositHistory(const std::string& uid, const std::string& exchange, const std::string& asset, double amount, const std::string& date, const std::string& txid);
        virtual void saveWithdrawHistory(const std::string& uid, const std::string& exchange, const std::string& asset, double amount, const std::string& date, const std::string &txid);
        virtual void updateAccountBalance(const int uid, const std::string& exchange, const std::string& asset, double amount);
		virtual void clearAccountBalance(const int uid, const std::string& exchange);
		virtual Json::Value selectBalanceByAsset(const int uid);
        virtual Json::Value selectBalanceByExchange(const int uid);

        // Orders and Trades
        virtual void updateTradeHistory_Bitfinex(const int uid, const std::string& exchange, const std::string& asset, double amount, double price, const std::string& type, const std::string& order_id, const std::string& tid, const  double fee_amount,const std::string& fee_currency, const std::string& time);
        virtual void updateTradeHistory_Kraken(const int uid, const std::string& asset, double amount, double price, const std::string& type, const std::string& order_id, const std::string& tid, const  double fee_amount,const std::string& fee_currency, const std::string& time);
        virtual void updateTradeHistory_Coinbase(const int uid, const std::string& asset, double amount, double price, const std::string& type, const std::string& order_id, const std::string& tid, const  double fee_amount,const std::string& fee_currency, const std::string& time);
        virtual void updateTradeHistory_Bittrex(const int uid, const std::string& asset, double amount, double price, const std::string& type, const std::string& order_id, const std::string& tid, const  double fee_amount,const std::string& fee_currency, const std::string& time);
        virtual void updateTradeHistory_Bitstamp(const int uid, const std::string& asset, double amount, double price, const std::string& type, const std::string& order_id, const std::string& tid, const  double fee_amount,const std::string& fee_currency, const std::string& time);

        virtual void deleteTradeHistory_Bitfinex(const int uid, const std::string& exchange);
        virtual void deleteTradeHistory_Kraken(const int uid);
        virtual void deleteTradeHistory_Binance(const int uid);
        virtual void deleteTradeHistory_Bitstamp(const int uid);
        virtual void deleteTradeHistory_Bittrex(const int uid);
        virtual void deleteTradeHistory_Coinbase(const int uid);

        // Portfolio
        virtual double selectHistoryPrice( const std::string& asset, const std::string& date );
        virtual std::string selectFundingDate( const std::string&uid, const std::string& exchange, const std::string& asset );
        virtual Json::Value selectPortfolio( const int uid, Json::Value exchangeAnswer);
		virtual Json::Value calcPortfolioCostBitfinex ( const int uid, const std::string& exchange );
        virtual Json::Value calcPortfolioCostKraken ( const int uid);
        virtual Json::Value calcPortfolioCostCoinbase ( const int uid);
        virtual Json::Value calcPortfolioCostBitstamp ( const int uid);
        virtual Json::Value calcPortfolioCostBittrex ( const int uid);

    protected:
        Poco::SharedPtr<Poco::Data::Session> session;
    };
}

#endif
