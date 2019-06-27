#ifndef __TRADING_BITFINEX_H__
#define __TRADING_BITFINEX_H__

#include "exchange.h"
#include <json/json.h>

namespace Trading
{ 
    namespace Bitfinex
    {
        class Engine : public Exchange
        {
        public:	
            Engine();   
            virtual ~Engine() override = default;

            virtual bool init(const ParseRequest& parseRequest) override;
			long long string_to_ul(const char* str);
			
            virtual std::string postAnyProcessing(const std::string& ctRequestId);
			virtual std::string postGetBalanceProcessing(const std::string& ctRequestId, const int uid);
			virtual std::string postGetTickerProcessing(const std::string& ctRequestId, const std::string& symbol);
			virtual std::string postGetPortfolioProcessing(const std::string& ctRequestId, const int uid, Json::Value exchangeAnswer_balance );
            virtual std::string postGetOrderBookProcessing(const std::string& ctRequestId, const std::string& symbol);
            virtual std::string postSendOrderProcessing(const std::string& ctRequestId);

            // Orders related API
            virtual std::string postGetTradeHistory(const std::string& ctRequestId, const std::string& symbol, const int uid);

            //virtual std::string postGetOrderStatusProcessing(const std::string& ctRequestId);
            //virtual std::string postGetAllOrdersProcessing(const std::string& ctRequestId);
            //virtual std::string postGetOpenOrdersProcessing(const std::string& ctRequestId);

        protected:
            const long recvWindow;
			Json::Value exchangeAnswer;
        };

        /******************************************************************************/
        namespace order
        {
            class GetOrderBook : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;
            };
				
			class SendNewOrder : public Engine
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

            class SendMultiNewOrders : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            class ReplaceOrder : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            class GetOrderStatus : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            class GetAllOpenOrders : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            class GetAllOrders : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;
            };
       

            class CancelOneOrder : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            class CancelMultiOrders : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            class CancelAllOrders : public Engine
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

        /******************************************************************************/
        namespace positions
        {
            class GetActivePositions : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };

            class ClaimPositions : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            };
        }

        namespace marginfunding
        {
            class NewOffer : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            }; 

            class CancelOffer : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            }; 

            class GetOfferStatus : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            }; 

            class GetActiveCredits : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            }; 

            class GetOffers : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            }; 

            class GetOffersHistory : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            }; 

            class GetPastFundingTrades : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            }; 

            class GetTakenFunds : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            }; 

            class GetUnusedTakenFunds : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            }; 

            class GetTotalTakenFunds : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            }; 

            class CloseLoan : public Engine
            {
            public:	
                virtual std::string process(const ParseRequest& parseRequest) override;
            }; 

            class ClosePosition : public Engine
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
                virtual std::string process(const ParseRequest& parseRequest) override;

            };

			class GetPortfolio : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;

            };

			class GetAccount : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;

            };

        }
        /******************************************************************************/
        namespace generic 
        {
            class GetAccountFees : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;

            };

            class GetSummary : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;

            };

            class Deposit : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;

            };

            class GetKeyPermissions : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;

            };

            class GetMarginInfo : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;

            };


            class Transfer : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;

            };

            class Withdraw : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;

            };

            class GetBalanceHistory : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;

            };

            class GetWithdrawalHistory : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;

            };

            class GetPastTrades : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;

            };

			class GetTicker : public Engine
            {
            public:
                virtual std::string process(const ParseRequest& parseRequest) override;

            };

        }        
    }
}

#endif
