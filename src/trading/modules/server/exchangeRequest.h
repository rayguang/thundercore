#ifndef __TRADING_COMMUNICATION_SERVER_H__
#define __TRADING_COMMUNICATION_SERVER_H__

#include <Poco/Net/TCPServerConnectionFactory.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/SharedPtr.h>

#include <vector>

#include "exchange.h"

namespace Trading
{    
	class ExchangeRequest : public Poco::Net::TCPServerConnection 
	{
	public:
		ExchangeRequest(const Poco::Net::StreamSocket& streamSocket, TypeFactoryExchanges &factoryExchanges, const long timeOutSeconds);
		virtual ~ExchangeRequest() override = default;

		void run() override;

	private:
		TypeFactoryExchanges &factoryExchanges;
		const long           timeOutSeconds;
	};
}

#endif
