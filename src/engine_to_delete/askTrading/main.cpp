#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketStream.h>
#include <Poco/StreamCopier.h>

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
		Poco::Net::SocketAddress sa("localhost", ::atoi(argv[1]));
		Poco::Net::StreamSocket socket(sa);
		Poco::Net::SocketStream stream(socket);

		stream << "{'REQ': 'EX', 'Pair': 'EOS/BTC', 'VAL': '1.23456789'}";					
		stream.flush();

		std::string str; 
		stream >> str;// this is can block, and do not work fine with \r and \n
		std::cout << "Answer is: " << str << std::endl << std::flush;		
	}	

	return 0;
}

