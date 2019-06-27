#include <Poco/Net/TCPServer.h>
#include <Poco/Net/TCPServerParams.h>
#include <Poco/Net/Socket.h>

#include "exchangeRequest.h"

//test
#include <cstdlib>
#include <iostream>
#include <signal.h>
static bool stop=false;

void handle_signal(int sig)
{
    stop=true;
} 

void showUsage(char *programName)
{
	std::cout << "Usage: " << programName << " portNumber"  << std::endl;	
}

int main(int argc, char *argv[])
{		
	signal(SIGQUIT, handle_signal);

	if (argc<2)
		showUsage(argv[0]);
	else
	{		
		try
        {            
			//server
			Poco::Net::ServerSocket serverSocket(::atoi(argv[1]));
			//Poco::Net::SecureServerSocket serverSocket(::atoi(argv[1]));
			
			//configure server
			Poco::Net::TCPServerParams* params=new Poco::Net::TCPServerParams();
			params->setMaxThreads(4);
			params->setMaxQueued(4);		

			Poco::Net::TCPServer communicationServer(new Poco::Net::TCPServerConnectionFactoryImpl<ExchangeRequest>(), serverSocket, params);		
			communicationServer.start();
							
			std::cout << std::endl << "<CTRL+C> to stop" << std::endl << std::endl;
			while (!stop)			
				Poco::Thread::sleep(5000);

			communicationServer.stop();			
        }
        catch (Poco::Exception& ex)
        {            
            std::cerr << "Error: " << ex.displayText() << std::endl;            
        }		
	}	

	return 0;
}

