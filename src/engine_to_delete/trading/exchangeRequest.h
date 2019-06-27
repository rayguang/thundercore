#ifndef __COMMUNICATION_SERVER_H__
#define __COMMUNICATION_SERVER_H__

#include <Poco/Net/TCPServerConnectionFactory.h>
#include <Poco/Net/StreamSocket.h>

class ExchangeRequest : public Poco::Net::TCPServerConnection 
{
public:
	ExchangeRequest(const Poco::Net::StreamSocket& streamSocket);
	virtual ~ExchangeRequest() = default;

	void run() override;	

	void virtual RequestProcessor(const char* request);


};

#endif


