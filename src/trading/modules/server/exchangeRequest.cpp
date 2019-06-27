#include "exchangeRequest.h"

#include <curl/curl.h>
#include <cstring>
#include <vector>
#include <iostream>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

/******************************************************************************/
//  init global curl for all apis
class CurlGlobalInit
{
public:
    CurlGlobalInit()
    {
        ::curl_global_init(CURL_GLOBAL_DEFAULT);
		spdlog::info("[exchangeRequest]: Called curl_global_init");
    }

    virtual ~CurlGlobalInit()
    {
        ::curl_global_cleanup();
        std::cout << "curl_global_cleanup" << std::endl;
    }
};

static CurlGlobalInit curlGlobalInit;
/******************************************************************************/

#define MAX_MSG_SIZE        10000
#define EXCHANGEID          "exchangeID"

using namespace Trading;

ExchangeRequest::ExchangeRequest(const Poco::Net::StreamSocket& streamSocket, TypeFactoryExchanges &factoryExchanges_, const long timeOutSeconds_)
    :   Poco::Net::TCPServerConnection(streamSocket),
        factoryExchanges(factoryExchanges_),
        timeOutSeconds(timeOutSeconds_)
{   
}

void ExchangeRequest::run()
{
    std::cout << "New Trading Request from: " << socket().peerAddress().host().toString() << std::endl;
    
    std::vector<char> buffer;
    buffer.resize(MAX_MSG_SIZE);    

    Poco::Timespan timeOut(timeOutSeconds, 0);        
    poco_assert(timeOutSeconds == timeOut.seconds());

    std::string answer;
    if (!socket().poll(timeOut, Poco::Net::Socket::SELECT_READ))
        answer = "TradingEngine connection time out.";
    else
    {        
        try
        {            
            const int receibedBytes = socket().receiveBytes(&buffer[0], buffer.size());            
            if (receibedBytes>0)
            {
                buffer.resize(receibedBytes);
                const std::string jsonRequest(buffer.begin(), buffer.end());                
                
                ParseRequest parseRequest(jsonRequest);
                const std::string className = parseRequest.classFromRequest();
                                
                auto exchangePair = factoryExchanges.find(className);
                if (factoryExchanges.end() == exchangePair)
                    answer = "UNKNOWN request class: " + className;
                else                
                {
                    auto exchange = exchangePair->second->createExchange();
                    poco_assert(nullptr != exchange.get());

                    if (exchange->init(parseRequest)){
                        answer = exchange->process(parseRequest);
                    }
                    else
                    {
                        answer = "[ERROR: exchangeRequest]: FAILED to init exchange: " + className;
                        
                    }                  
                }
            }
        }
        catch (Poco::Exception& ex)
        {            
            answer = "[ERROR: exchangeRequest]: " + ex.displayText();
        }    
        catch(...)    
        {
            answer = "[ERROR: exchangeRequest generic]: ";
        }
    }

    poco_assert(answer.length() > 0);    
    socket().sendBytes(&answer[0], answer.length()); 
}
