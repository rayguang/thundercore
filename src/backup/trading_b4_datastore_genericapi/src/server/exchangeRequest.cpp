#include "exchangeRequest.h"

#include <cstring>
#include <vector>
#include <algorithm>

#include <iostream>

#define MAX_MSG_SIZE        10000
#define EXCHANGEID          "exchangeID"
#define SEPARATOR           "::"

using namespace Trading;

ExchangeRequest::ExchangeRequest(const Poco::Net::StreamSocket& streamSocket, TypeFactoryExchanges &factoryExchanges_, const long timeOutSeconds_)
    :   Poco::Net::TCPServerConnection(streamSocket),
        factoryExchanges(factoryExchanges_),
        timeOutSeconds(timeOutSeconds_)
{   
}

void ExchangeRequest::run()
{
    std::cout << "New Trading Request from: " << socket().peerAddress().host().toString() << " Active Exchanges: " << factoryExchanges.size() << std::endl;
    
    std::vector<char> buffer;
    buffer.resize(MAX_MSG_SIZE);    

    Poco::Timespan timeOut(timeOutSeconds, 0);        
    poco_assert(timeOutSeconds == timeOut.seconds());

    std::string answer;
    if (!socket().poll(timeOut, Poco::Net::Socket::SELECT_READ))
        answer = "Time out";
    else
    {        
        try
        {            
            const int receibedBytes = socket().receiveBytes(&buffer[0], buffer.size());            
            if (receibedBytes>0)
            {
                buffer.resize(receibedBytes);
                const std::string jsonRequest(buffer.begin(), buffer.end());                
                
                #pragma message "Get from JSON exchange Plus Action"
                std::string exchangePlusAction("Binance" SEPARATOR "BUY");
                std::transform(exchangePlusAction.begin(), exchangePlusAction.end(), exchangePlusAction.begin(), ::tolower);                                
                std::cout << "exchangePlusAction is: " << exchangePlusAction << std::endl;                

                auto exchangePair = factoryExchanges.find(exchangePlusAction);
                if (factoryExchanges.end() == exchangePair)
                    answer = "Not found: " + exchangePlusAction;
                else                
                {
                    auto exchange = exchangePair->second->createExchange();
                    poco_assert(nullptr != exchange.get());

                    answer = exchange->process(jsonRequest);
                }                
            }            
        }
        catch (Poco::Exception& ex)
        {            
            answer = "Error: " + ex.displayText();
        }        
    }

    poco_assert(answer.length() > 0);    
    socket().sendBytes(&answer[0], answer.length()); 
}
