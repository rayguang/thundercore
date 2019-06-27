#ifndef __TRADING_CORE_H__
#define __TRADING_CORE_H__

#include "exchange.h"

#include <binacpp.h>
#include <bitscpp.h>
#include <bitxcpp.h>
#include <coincpp.h>
#include <krakcpp.h>
#include <json/json.h>

namespace Trading {
    namespace Raycore {
        class Engine : public Exchange
        {
        public:
            Engine();
            virtual ~Engine() override = default;

            virtual bool init(const ParseRequest& parseRequest) override;

            // post processing related to send_order
            //virtual std::string postProcessing(const std::string& ctRequestId);
            

        protected:
            Json::Value exchangeAnswer;
            const long  recvWindow;
            BinaCPP     binaCPP;
            BitsCPP     bitsCPP;
            BitxCPP     bitxCPP;
            KrakCPP     krakCPP;
            CoinCPP     coinCPP;

        };

        namespace account
        {
            class GetPortfolio : public Engine
            {
                public:
                    virtual std::string process(const ParseRequest& parseREquest) override;
            };  

            class GetBalanceAsset : public Engine
            {
                public:
                    virtual std::string process(const ParseRequest& parseREquest) override;
            }; 

            class GetBalanceExchange : public Engine
            {
                public:
                    virtual std::string process(const ParseRequest& parseREquest) override;
            }; 

        }
    }
}

#endif
