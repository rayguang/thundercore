#ifndef __TRADING_KRAKEN_H__
#define __TRADING_KRAKEN_H__

#include "exchange.h"

#include <krakcpp.h>
#include <json/json.h>

namespace Trading
{
    namespace Kraken
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
			virtual std::string postGetOrderBookProcessing(const std::string& ctRequestId, const std::string& symbol);

            // API related to orders
            virtual std::string postSendOrderProcessing(const std::string& ctRequestId);
            virtual std::string postGetOrderStatusProcessing(const std::string& ctRequestId, const std::string& orderId);
            //virtual std::string postGetAllOrdersProcessing(const std::string& ctRequestId);
            virtual std::string postGetOpenOrdersProcessing(const std::string& ctRequestId);
			virtual std::string postCancelOneOrderProcessing(const std::string& ctRequestId, const std::string& orderId);
            virtual std::string postCancelAllOrdersProcessing(const std::string& ctRequestId);
			
			virtual std::string postGetTradeHistory(const std::string& ctRequestId, const std::string& symbol, const int uid);
			virtual std::string postGetPortfolioProcessing(const std::string& ctRequestId, const int uid, Json::Value exchangeAnswer_balance);

            // post porcessing allforone interface
            virtual std::string postAnyProcessing(const std::string& ctRequestId);

        protected:
            Json::Value exchangeAnswer;
            const long  recvWindow;
            KrakCPP     krakCPP;
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

            class GetAssetInfo : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

            class GetAssetPairs : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

            class GetTicker : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

            class GetOrderbook : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

            class GetTrades : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            }; 

            class GetSpread : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            }; 


        }

        /******************************************************************************/
        // Private endpoints
        namespace account
        {
            class GetBalance : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };  

            class GetTradeBalance : public Engine
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
        namespace order
        {
            class GetTradeHistory : public Engine
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

            class GetOrderStatus : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            class GetClosedOrders : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            }; 

            class QueryTrades : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            }; 

            class GetOpenPositions : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            }; 

            class GetLedgerInfo : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            }; 

            class QueryLedgers : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            }; 

            class GetTradeVolume : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

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

            class CancelOneOrder : public Engine
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
