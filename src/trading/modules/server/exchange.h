#ifndef __TRADING_EXCHANGE_H__
#define __TRADING_EXCHANGE_H__

#include <Poco/SharedPtr.h>

#include <cstring>
#include <map>

namespace Trading
{
    /******************************************************************************/
    class ParseRequest
    {
    public:        
        ParseRequest(const std::string& request);

        virtual std::string classFromRequest() const;
        virtual ~ParseRequest() = default;
            
    public:
        std::string     ctRequestId;
        std::string     requestType;
        std::string     subType;
        int             userID;
        std::string     exchange;
        std::string     symbol;        
        std::string     orderSide;
        double          quantity;
        double          price;
        double          stopPrice;
        double          limitPrice;
        double          icebergQty;
        std::string     timeInForce;
		std::string     clientOrderId;
		std::string     orderId;
		std::string     fromId;
        int             limit;
        std::string	    type;
        long            positionId;
        std::string     currency;
        std::string     walletaddr;
		std::string	    twoFactor;
        std::string     startTime;
        std::string     expireTime;
        std::string     leverage;
        std::string     accountId;
        std::string     orderStatus;
		std::string		bookLevel;
        std::string     fromtm;
        std::string     totm;

    };

    /******************************************************************************/
    class Exchange
    {
    public:        
        // server calls automatically init then process
        virtual bool init(const ParseRequest& parseRequest);
        virtual std::string process(const ParseRequest& parseRequest) = 0;

        virtual ~Exchange() = default;
    protected:
        std::string key;
        std::string secret;            
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
