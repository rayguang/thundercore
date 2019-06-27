#ifndef __TRADING_COINBASE_H__
#define __TRADING_COINBASE_H__

#include "exchange.h"

#include <coincpp.h>
#include <json/json.h>

namespace Trading
{
    namespace Coinbase
    {
        /******************************************************************************/
        class Engine : public Exchange
        {
        public:
            Engine();
            virtual ~Engine() override = default;

            virtual bool init(const ParseRequest& parseRequest) override;

            virtual std::string postGetBalanceProcessing(const std::string& ctRequestId, const int uid);
            virtual std::string postGetTickerProcessing(const std::string& ctRequestId, const std::string& symbol);
			
			// post orders processing
			virtual std::string postSendOrderProcessing(const std::string& ctRequestId);
			virtual std::string postGetOpenOrdersProcessing(const std::string& ctRequestId);
			virtual std::string postGetOrderStatusProcessing(const std::string& ctRequestId);
			virtual std::string postGetAllOrdersProcessing(const std::string& ctRequestId);
			virtual std::string postCancelOneOrderProcessing(const std::string& ctRequestId, const std::string& orderId);
			virtual std::string postCancelAllOrdersProcessing(const std::string& ctRequestId);
			virtual std::string postGetOrderBookProcessing(const std::string& ctRequestId, const std::string& symbol);

            virtual std::string postGetTradeHistoryProcessing(const std::string& ctRequestId, const int uid);
            virtual std::string postGetPortfolioProcessing(const std::string& ctRequestId, const int uid, Json::Value exchangeAnswer_balance);
            
            // post porcessing allforone interface
            virtual std::string postAnyProcessing(const std::string& ctRequestId);

        protected:
            Json::Value exchangeAnswer;
            const long  recvWindow;
            CoinCPP     coinCPP;
        };

        /******************************************************************************/
        // Public endpoints
        namespace generic
        {     
            class GetServerTime : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };    

            class GetProducts : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            class GetTicker : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            class GetProductTrades : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            class GetCurrencies : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };




        }

        /******************************************************************************/
        // Private endpoints
        namespace account
        {
            class GetPortfolio : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

            class GetBalance : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

            class GetAccount : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            }; 

            class GetAccountHistory : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            }; 

            class GetHolds : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            }; 

           

        }

         /******************************************************************************/
        namespace order
        {
           class SendMarketOrder : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

            class SendLimitOrder : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

            class SendStopOrder : public Engine
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

            class GetOrderStatus : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

            class GetAllOrders : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

            class GetAllOpenOrders : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

            class GetTradeHistory : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

			class GetOrderBook : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

        }
        
    }
}

#endif
