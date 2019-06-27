#include "tradingFactory.h"

#include "exchangeRequest.h"
#include "mySqlStore.h"

#include <Poco/Net/TCPServer.h>
#include <Poco/Net/TCPServerParams.h>
#include <Poco/Net/TCPServerConnection.h>
#include <Poco/Net/TCPServerConnectionFactory.h>
#include <Poco/Net/Socket.h>
#include <Poco/SharedPtr.h>

#include <vector>
#include <utility>
#include <algorithm>
#include <thread>

#define TIME_OUT_SECONDS    5   
#define CONCURRENT_WORKER   20

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
Factory::Factory(const unsigned short serverPort_, const char* dbConnectString_)
    :   serverPort(serverPort_),
        dbConnectString(dbConnectString_),
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
    // Configuration
    #pragma message "CONFIG: concurrentWork"
    // unsigned int num_cores = std::thread::hardware_concurrency();
	// std::cout << "System env CPU cores #: " << num_cores << std::endl;
	const int concurrentWork(CONCURRENT_WORKER);

    // Configure database
    util::MySqlStore::initDB(dbConnectString.c_str(), concurrentWork / 3, concurrentWork);

    // Configure server
    Poco::Net::ServerSocket serverSocket(serverPort);            
    Poco::Net::TCPServerParams* params=new Poco::Net::TCPServerParams();
    params->setMaxThreads(concurrentWork);

    #pragma message "CONFIG: setMaxQueued"
    params->setMaxQueued(concurrentWork * 100);
    timeOutSeconds = TIME_OUT_SECONDS;

    // ******** Start trading server ********    
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

void Factory::showActiveAPI()
{
	std::vector<std::string> v;
	for (auto it = factoryExchanges.begin(); it != factoryExchanges.end(); it++)
	{
		v.push_back(it->first);	
		std::cout << it->first << std::endl;
	}	
}
