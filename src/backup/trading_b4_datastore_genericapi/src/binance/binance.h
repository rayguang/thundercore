#ifndef __TRADING_BINANCE_H__
#define __TRADING_BINANCE_H__

#include "exchange.h"

namespace Trading
{
    namespace Binance
    {
        class Engine : public Exchange
        {
        public:	
            Engine();           
            virtual ~Engine() override = default;
        };

        /******************************************************************************/
        class BUY : public Engine
        {
        public:	
            virtual std::string process(const std::string& request) override;        
            virtual ~BUY() override = default;
        };
    }
}

#endif
