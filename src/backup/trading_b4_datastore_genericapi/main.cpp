#include "signalHandler.h"
#include "tradingFactory.h"
#include "binance.h"
#include "bitfinex.h"

#include <iostream>
#include <Poco/Data/MySQL/Connector.h>
#define MIN_ARG 			2

void showUsage(char *programName)
{
	std::cout << "Usage: " << programName << " portNumber"  << std::endl;	
}

int main(int argc, char *argv[])
{		
	if (argc < MIN_ARG)
		showUsage(argv[0]);
	else
	{		
		const unsigned short port=::atoi(argv[1]);
		
		Poco::Data::MySQL::Connector::registerConnector();

		Trading::Factory tradingFactory(port);
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Binance::BUY));
		tradingFactory.push(PUSH_EXCHANGE_PLUS_ACTION(Bitfinex::BUY));		
		
		util::SignalHandler signalHandler(tradingFactory);

		std::cout << "<CTRL+C> to stop" << std::endl;
		
		tradingFactory.start();		

		Poco::Data::MySQL::Connector::unregisterConnector();
	}	

	return 0;
}


