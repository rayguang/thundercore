#ifndef __TRADING_BITFINEX_H__
#define __TRADING_BITFINEX_H__

#include "exchange.h"

namespace Trading
{ 
    namespace Bitfinex
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
