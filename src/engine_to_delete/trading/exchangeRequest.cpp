#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/TCPServer.h>
#include <Poco/Thread.h>

#include <vector>
#include <iostream>

#include "exchangeRequest.h"

ExchangeRequest::ExchangeRequest(const Poco::Net::StreamSocket& streamSocket)
    : Poco::Net::TCPServerConnection(streamSocket)
{
    Poco::Net::TCPServer tcpServer();
}

#define MAX_MSG_SIZE 100
#define TIME_OUT     1000000

void ExchangeRequest::RequestProcessor(const char* request)
{
    // Parse request message, e.g., UserID, Exchange Name, OrderType, ActionType, Quantity, Price, TimeInForce, misc

    // Check if API calls for exchange active/exists


    // Retrieve User API keys for exchanges
    retrieveUserAPI(userID, exchangeID)

    // Place API alls
    placeAPICall(API_KEY, API_SECRECT, EXCHANGE_ID, USER_ID, API_TYPE)

}

void ExchangeRequest::run()
{
    std::cout << "New Trading Request from: " << socket().peerAddress().host().toString() << std::endl << std::flush;
    
    std::vector<char> buffer;
    buffer.resize(MAX_MSG_SIZE);

    Poco::Timespan timeOut(TIME_OUT);    
    if (!socket().poll(timeOut, Poco::Net::Socket::SELECT_READ))
        std::cerr << "Time out" << std::endl;
    else
    {
        try
        {            
            const int receibedBytes = socket().receiveBytes(&buffer[0], buffer.size());            
            if (receibedBytes>0)
            {
                buffer.resize(receibedBytes);
                std::string request(buffer.begin(), buffer.end());
                std::cout << "Request is: " << request << std::endl;             

                //Poco::Thread::sleep(5000);//simulate transaction time, if necessary

                char answer[]="{'OK'}";	
                socket().sendBytes(answer, ::strlen(answer));//simulate all ok
            }
        }
        catch (Poco::Exception& ex)
        {            
            std::cerr << "Error: " << ex.displayText() << std::endl;            
        }     
    }
}



