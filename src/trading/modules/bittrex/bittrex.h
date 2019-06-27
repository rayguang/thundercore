#ifndef __TRADING_BITTREX_H__
#define __TRADING_BITTREX_H__

#include "exchange.h"

#include <bitxcpp.h>
#include <json/json.h>

namespace Trading
{
    namespace Bittrex
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

            // post processing related to orders routines
            virtual std::string postSendOrderProcessing(const std::string& ctRequestId);
            virtual std::string postGetOrderStatusProcessing(const std::string& ctRequestId);
            virtual std::string postCancelOneOrderProcessing(const std::string& ctRequestId, const std::string& orderId);
            virtual std::string postGetAllOpenOrdersProcessing(const std::string& ctRequestId);
            virtual std::string postGetAllOrdersProcessing(const std::string& ctRequestId);
			virtual std::string postGetOrderBookProcessing(const std::string& ctRequestId, const std::string& symbol);
			virtual std::string postGetTradeHistoryProcessing(const std::string& ctRequestId, const int uid);		

            // post porcessing allforone interface
            virtual std::string postAnyProcessing(const std::string& ctRequestId);
            virtual std::string postGetPortfolioProcessing(const std::string& ctRequestId, const int uid, Json::Value exchangeAnswer_balance);
            
        protected:
            Json::Value exchangeAnswer;
            const long  recvWindow;
            BitxCPP     bitxCPP;
        };

        /******************************************************************************/
        namespace generic
        {     
            class GetMarkets : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };    

            class GetCurrencies : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };   

            class GetTicker : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            class GetMarketSummaries : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            class GetMarketSummary : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            class GetMarketHistory : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };


        }

        /******************************************************************************/
        namespace account
        {
            class GetAssetBalance : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

            class GetBalance : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

            class GetDepositAddress : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

            class Withdraw : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

            class GetWithdrawHistory : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

            class GetDepositHistory : public Engine
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

         /******************************************************************************/
         // Implement authenticated API: market
        namespace order
        {
            class SendLimitOrder : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            class BuyLimit : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

            class SellLimit : public Engine
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

            class GetAllOrders : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

			class GetOrderBook : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

			class GetTradeHistory : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;
            };
        }
    }
}

#endif
