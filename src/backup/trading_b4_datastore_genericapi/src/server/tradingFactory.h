#ifndef __TRADING_FACTORY_H__
#define __TRADING_FACTORY_H__

#include <Poco/Event.h>
#include <Poco/SharedPtr.h>

#include "service.h"
#include "exchange.h"


namespace Trading
{    
    class Factory : public util::Service
    {
    public:
        Factory(const unsigned short port);
        virtual ~Factory();
        
        virtual void start() override;
        virtual void stop() override;        

        // use this macro in push
        // Example: tradingFactoryObj.push(PUSH_EXCHANGE_PLUS_ACTION(Binance::BUY));
        #define PUSH_EXCHANGE_PLUS_ACTION(EXCHANGE_PLUS_ACTION) #EXCHANGE_PLUS_ACTION, new Trading::FactoryExchangeImpl<Trading::EXCHANGE_PLUS_ACTION>        
        void push(const char* exchangePlusAction, Poco::SharedPtr<FactoryExchange> factoryExchange);

    private:
        const unsigned short    port;
        long                    timeOutSeconds;
        Poco::Event             signalTerminate;
        TypeFactoryExchanges    factoryExchanges;
    };
}

#endif
