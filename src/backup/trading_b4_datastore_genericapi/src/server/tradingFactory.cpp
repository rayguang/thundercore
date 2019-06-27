#include "tradingFactory.h"

#include "exchangeRequest.h"

#include <Poco/Net/TCPServer.h>
#include <Poco/Net/TCPServerParams.h>
#include <Poco/Net/TCPServerConnection.h>
#include <Poco/Net/TCPServerConnectionFactory.h>
#include <Poco/Net/Socket.h>
#include <Poco/SharedPtr.h>

#include <vector>
#include <utility>
#include <algorithm>

#define TIME_OUT_SECONDS    5   

using namespace Trading;

/******************************************************************************/
namespace Poco
{
namespace Net
{
    template <class BASE>
    class TradingFactoryImpl: public Poco::Net::TCPServerConnectionFactory
    {
    public:
        TradingFactoryImpl(TypeFactoryExchanges& factoryExchanges_, const long timeOutSeconds_)
            : factoryExchanges(factoryExchanges_),
              timeOutSeconds(timeOutSeconds_)
        {
        }
        
        ~TradingFactoryImpl() override
        {
        }
        
        virtual TCPServerConnection* createConnection(const StreamSocket& socket) override        
        {
            return new BASE(socket, factoryExchanges, timeOutSeconds);
        }

    private:
        TypeFactoryExchanges& factoryExchanges;
        const long            timeOutSeconds;
    };
}
}

/*********************************************FactoryExchange*********************************/
Factory::Factory(const unsigned short port_)
    :   port(port_),
        timeOutSeconds(TIME_OUT_SECONDS),
        signalTerminate(true)
{    
}

Factory::~Factory()
{
    stop();
}

void Factory::start()
{
    //server
    Poco::Net::ServerSocket serverSocket(port);    
        
    //configure server
    #pragma message "need proper configure"
    Poco::Net::TCPServerParams* params=new Poco::Net::TCPServerParams();    
    params->setMaxThreads(5);
    params->setMaxQueued(100);		
    timeOutSeconds = TIME_OUT_SECONDS;
    
        
    Poco::Net::TCPServer communicationServer(new Poco::Net::TradingFactoryImpl<ExchangeRequest>(factoryExchanges, timeOutSeconds), serverSocket, params);
    communicationServer.start();
                    
    signalTerminate.wait();
    
    communicationServer.stop();        
}

void Factory::stop()
{
    signalTerminate.set();
}

void Factory::push(const char* exchangePlusAction_, Poco::SharedPtr<FactoryExchange> factoryExchange)
{
    poco_assert(nullptr != exchangePlusAction_ && ::strlen(exchangePlusAction_) > 0 && nullptr != factoryExchange.get());    

    std::string exchangePlusAction(exchangePlusAction_);
    std::transform(exchangePlusAction.begin(), exchangePlusAction.end(), exchangePlusAction.begin(), ::tolower);

    poco_assert(0 == factoryExchanges.count(exchangePlusAction));
    factoryExchanges.insert(std::make_pair(exchangePlusAction, factoryExchange));
}

