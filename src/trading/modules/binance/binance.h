#ifndef __TRADING_BINANCE_H__
#define __TRADING_BINANCE_H__

#include "exchange.h"

#include <binacpp.h>
#include <json/json.h>

namespace Trading
{
    namespace Binance
    {
        /******************************************************************************/
        class Engine : public Exchange
        {
        public:
            Engine();
            virtual ~Engine() override = default;

            virtual bool init(const ParseRequest& parseRequest) override;

            // post processing related to send_order
            virtual std::string postProcessing(const std::string& ctRequestId);
            
            // post processing related to get_order
            virtual std::string postSendOrderProcessing(const std::string& ctRequestId);
            virtual std::string postGetOrderStatusProcessing(const std::string& ctRequestId);
            virtual std::string postGetAllOrdersProcessing(const std::string& ctRequestId);
            virtual std::string postGetOpenOrdersProcessing(const std::string& ctRequestId);
            virtual std::string postGetTradeHistoryProcessing(const std::string& ctRequestId);
			virtual std::string postCancelOneOrderProcessing(const std::string& ctRequestId);
            virtual std::string postCancelAllOrdersProcessing(const std::string& ctRequestId);

            virtual std::string postGetMyTradesProcessing();
 			
 			virtual std::string postGetTickerProcessing(const std::string& ctRequestId);
			virtual std::string postGetOrderBookProcessing(const std::string& ctRequestId, const std::string& symbol);
            virtual std::string postGetRecentTradesProcessing(const std::string& ctRequestId, const std::string& symbol);

            // post processing related to generic
            virtual std::string postGetBalanceProcessing(const std::string& ctRequestId, const int uid);
			virtual std::string postGetMarketProcessing(const std::string& ctRequestId);

            // post porcessing allforone interface
            virtual std::string postAnyProcessing(const std::string& ctRequestId);
			virtual std::string postRequestProcessing(const std::string& ctRequestId);

        protected:
            Json::Value exchangeAnswer;
            const long  recvWindow;
            BinaCPP     binaCPP;
        };

        /******************************************************************************/
        namespace order
        {
            /**************************************************************************/        
            class SendLimitOrder : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            /**************************************************************************/        
            class SendMarketOrder : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            /**************************************************************************/        
            class SendIcebergOrder : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            /**************************************************************************/        
            class SendStopLossOrder : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            class SendStopLossLimitOrder : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            class SendTakeProfitOrder : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            class SendTakeProfitLimitOrder : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            class SendLimitMakerOrder : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

             /**************************************************************************/ 
            // API: Cancel orders  
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

            class CancelAllLimitOrders : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            class CancelAllStopLimitOrders : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;
            };
          
            // API: Check an order's status  
            class GetOrderStatus : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };       

            /**************************************************************************/
            // Get all account orders; active, canceled, or filled.
            class GetAllOpenOrders : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            /**************************************************************************/
            // Get all account orders; active, canceled, or filled.
            class GetAllOrders : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            /**************************************************************************/
            // Get all trades history
            class GetTradeHistory : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

			// Get order book
            class GetOrderBook : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

			class GetRecentTrades : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;
            };
            
        }

        /******************************************************************************/
        namespace generic
        {     
            class ServerTime : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };     

            class GetTicker : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            class GetMarket : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

			class GetTradingPairs : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            


        }

        /******************************************************************************/
        namespace account
        {
            class GetBalance : public Engine
            {
                public:
                    virtual std::string process(const ParseRequest& parseREquest) override;
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
    }
}

#endif
