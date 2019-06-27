#ifndef __TRADING_EXCHANGE_H__
#define __TRADING_EXCHANGE_H__

#include <Poco/SharedPtr.h>

#include <cstring>
#include <map>

namespace Trading
{
    /******************************************************************************/
    class Exchange
    {
    public:	
        virtual std::string process(const std::string& request) = 0;

        virtual ~Exchange() = default;
    };


    /******************************************************************************/
    class FactoryExchange
    {
    public:
        virtual Poco::SharedPtr<Exchange> createExchange() = 0;        

        virtual ~FactoryExchange() = default;
    };    
    
    /******************************************************************************/
    template <class T>
    class FactoryExchangeImpl : public FactoryExchange
    {
    public:                
        virtual Poco::SharedPtr<Exchange> createExchange() override
        {
            return Poco::SharedPtr<T>(new T);
        }

        virtual ~FactoryExchangeImpl() = default;
    };    

    /******************************************************************************/
    typedef std::map<std::string, Poco::SharedPtr<FactoryExchange>> TypeFactoryExchanges;    
}

#endif
