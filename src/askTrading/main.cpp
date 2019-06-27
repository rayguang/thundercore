#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketStream.h>
#include <Poco/StreamCopier.h>
#include <Poco/JSON/Object.h>

//test
#include <cstdlib>
#include <signal.h>
#include <sstream> 

#include <iostream>
#include <chrono>
#include <uuid/uuid.h>

#define MAX_MSG_SIZE        10000
#define TIME_OUT_SECONDS    5   

//using namespace std::chrono;

void showUsage(char *programName)
{
	std::cout << "Usage: " << programName << " portNumber"  << std::endl;	
}


#define LIMIT_ORDER1
#define MARKET_ORDER1
#define SERVER_TIME
#define MAX_ROUND 		300

int main(int argc, char *argv[])
{		
	if (argc<2)
		showUsage(argv[0]);
	else
	{	
		
		//Timer start
		auto start = std::chrono::high_resolution_clock::now();
			
		// {'REQ': 'EX', 'Pair': 'EOS/BTC', 'VAL': '1.23456789'}";		
		Poco::JSON::Object::Ptr json = new Poco::JSON::Object(true);			
		
		// Generate uuid
		uuid_t uuid;
		uuid_generate_random(uuid);
		char uuid_str[37];
		uuid_unparse_lower(uuid,uuid_str);
		std::cout << uuid_str << std::endl;

		#if defined(SERVER_TIME)
			json->set("RequestType", "generic");			
			json->set("Exchange", "BINANCE");
			json->set("SubType", "ServerTime");			

		#elif defined(MARKET_ORDER)				
			json->set("RequestType", "send_order");
			json->set("UserID", 436);
			json->set("Exchange", "BINANCE");
			json->set("Symbol",   "BNBBTC");
			json->set("Side",     "BUY");
			json->set("OrderType", "MARKET");
			json->set("Quantity",  1.0);
			json->set("Price",     "");		

		#elif defined(LIMIT_ORDER)
			json->set("RequestType", "send_order");
			json->set("UserID", 436);
			json->set("Exchange", "BINANCE");
			json->set("Symbol",   "BNBBTC");
			json->set("Side",     "BUY");
			json- >set("OrderType", "LIMIT");
			json->set("TimeInForce", "GTC"); // GTC, FOK, IOC
			json->set("Quantity",  1.0);
			json->set("Price",     0.0028574);		
		#endif

		std::ostringstream streamJson;
		json->stringify(streamJson);

		std::string request(streamJson.str());		
		std::cout << "buy is:" << request << std::endl;			

		Poco::Net::SocketAddress sa("localhost", ::atoi(argv[1]));
		Poco::Net::StreamSocket socket(sa);				
		socket.sendBytes(&request[0], request.length());

		Poco::Timespan timeOut(TIME_OUT_SECONDS, 0);        
    	poco_assert(TIME_OUT_SECONDS == timeOut.seconds());

		std::string answer="{'no answer'}";
		try
        {            
			std::vector<char> buffer;
    		buffer.resize(MAX_MSG_SIZE);   

            const int receibedBytes = socket.receiveBytes(&buffer[0], buffer.size());            
            if (receibedBytes>0)
            {
                buffer.resize(receibedBytes);                
                answer=std::string(buffer.begin(), buffer.end());                				                
            }            
        }
        catch (Poco::Exception& ex)
        {            
            answer = "Error: " + ex.displayText();
        }     
		std::cout << "Answer is: " << answer << std::endl;			
		
		auto end = std::chrono::high_resolution_clock::now();

		auto duration =std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
		std::cout << "Runtime: " << duration << std::endl;
	}	
	
	return 0;
}

