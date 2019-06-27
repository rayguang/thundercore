#ifndef __TRADING_BITSTAMP_H__
#define __TRADING_BITSTAMP_H__

#include "exchange.h"

#include <bitscpp.h>
#include <json/json.h>

namespace Trading
{
    namespace Bitstamp
    {
        /******************************************************************************/
        class Engine : public Exchange
        {
        public:
            std::vector<std::string> order_type {"INSTANT", "LIMIT", "MARKET", "STOP"};

            Engine();
            virtual ~Engine() override = default;

            virtual bool init(const ParseRequest& parseRequest) override;
            
            virtual std::string postSendOrderProcessing(const std::string& ctRequestId);
            virtual std::string postGetBalanceProcessing(const std::string& ctRequestId, const int uid);
            virtual std::string postGetTickerProcessing(const std::string& ctRequestId, const std::string& symbol);

            virtual std::string postGetOrderStatusProcessing(const std::string& ctRequestId, const std::string& orderId);
            virtual std::string postGetAllOrdersProcessing(const std::string& ctRequestId);
            virtual std::string postGetAllOpenOrdersProcessing(const std::string& ctRequestId);
            virtual std::string postCancelOneOrderProcessing(const std::string& ctRequestId);
            virtual std::string postCancelAllOrdersProcessing(const std::string& ctRequestId);
			
			virtual std::string postGetTradeHistoryProcessing(const std::string& ctRequestId, const int uid);
			virtual std::string postGetPortfolioProcessing(const std::string& ctRequestId, const int uid, Json::Value exchangeAnswer_balance);
            virtual std::string postGetOrderBookProcessing(const std::string& ctRequestId, const std::string& symbol);

            // post porcessing allforone interface
            virtual std::string postAnyProcessing(const std::string& ctRequestId);

        protected:
            Json::Value exchangeAnswer;
            const long  recvWindow;
            BitsCPP     bitsCPP;
        };

        /******************************************************************************/
        namespace generic
        {     
            class GetMarkets : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };    

            class GetHourlyMarkets: public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            class GetTransactions: public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            class GetTradingPairs: public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            class GetEURUSD: public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            class GetTicker: public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

        }

        namespace account
        {
            class GetBalance : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

            class GetMyTrades : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

            class GetPortfolio : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            }; 
        } 

        namespace order
        {
            class GetAllOpenOrders : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

            class GetOrderStatus : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

            class CancelOneOrder : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

            class CancelAllOrders : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

            class SendLimitOrder : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

            class SendMarketOrder : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

            class SendInstantOrder : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

            class GetAllOrders: public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

			class GetOrderBook: public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

			class GetTradeHistory: public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;
            };
        }

    }
}

#endif
