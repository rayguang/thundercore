#include <map>
#include <vector>
#include <string>
#include <iostream>

// Third party lib
#include "json/json.h"
#include "json/jsoncpp.cpp"

// Binance lib
#include "binacpp.h"
#include "binacpp_websocket.h"

#include "trading.h"

#define API_KEY 	"CjmkwSP57BYJoVqvSHf4pzDGCAiHiW0TFglZlVLJtz5XQXpV03vpa8oQbR5fMsKV"
#define API_SECRECT "M9NcEE96OOdqAFIvznl06huGmr10GUxd32IK6kN4zvx6TmxPXiawNyKWRkgWi6NR"

// Some code to make terminal to have colors
#define KGRN "\033[0;32;32m"
#define KCYN "\033[0;36m"
#define KRED "\033[0;32;31m"
#define KYEL "\033[1;33m"
#define KBLU "\033[0;32;34m"
#define KCYN_L "\033[1;36m"
#define KBRN "\033[0;33m"
#define RESET "\033[0m"

using namespace std;

map<string, map<double, double> > depthCache;
map<long, map<string, double> > klinesCache;
map<long, map<string, double> > aggTradeCache;
map<long, map<string, double> > userTradeCache;
map<string, map<string, double> > userBalance;

int lastUpdateId;

void print_userBalance()
{
    map<string, map<string, double> >::iterator it_i;

    cout << "==================================" << endl;

    for (it_i = userBalance.begin(); it_i != userBalance.end(); it_i++)
    {
        string symbol = (*it_i).first;
        map <string, double> balance = (*it_i).second;

	    cout << "Symbol :" << symbol << ", ";
		printf("Free   : %.08f, ", balance["f"] );
		printf("Locked : %.08f " , balance["l"] );
		cout << " " << endl;
    }
}

int ws_userStream_OnData( Json::Value &json_result)
{
    int i;
    string action = json_result["e"].asString();

    if ( action == "exectionReport" ) {
        string executionType = json_result["x"].asString();
		string orderStatus   = json_result["X"].asString();
		string reason        = json_result["r"].asString();
		string symbol 	     = json_result["s"].asString(); 	
		string side 	     = json_result["S"].asString();	
		string orderType     = json_result["o"].asString();	
		string orderId 	     = json_result["i"].asString();	
		string price 	     = json_result["p"].asString();
		string qty 	         = json_result["q"].asString();

        if (executionType =="NEW") {
            if (orderStatus == "REJECTED") {
                printf(KRED"Order Failed! Reason %s\n"RESET, reason.c_str());
            }
            printf(KGRN"\n\n%s %s %s %s(%s) %s %s\n\n"RESET, symbol.c_str() , side.c_str() , orderType.c_str() , orderId.c_str() , orderStatus.c_str(), price.c_str(), qty.c_str() );
			return 0;
        }
        printf(KBLU"\n\n%s %s %s %s %s\n\n"RESET, symbol.c_str() , side.c_str() , executionType.c_str() , orderType.c_str() , orderId.c_str() );

    } else if ( action == "outboundAccountInfo" ) {
        // Update user balance
        for ( i = 0 ; i < json_result["B"].size(); i++) {
            string symbol = json_result["B"][i]["a"].asString();
            userBalance[symbol]["f"] = atof(json_result["B"][i]["f"].asString().c_str());
            userBalance[symbol]["l"] = atof(json_result["B"][i]["l"].asString().c_str()); 
        }
        print_userBalance();
    }
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Define all order related functions
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int place_limit_order (const char* exchange_name, Json::Value) 
{
    if ( exchange_name == "bina" || exchange_name == "BINA" ){
        //Place order on Binance exchange
        //BinaCPP::send_order( "BNBETH", "BUY", "LIMIT", "GTC", 20 , 0.00380000, "",0,0, recvWindow, result );
	//cout << result << endl;
    } else {
        cout << "INVALID exchange name" << endl;
    }

}

int place_market_order () 
{

}



int main()
{
    // Init API key and secret
    std::string api_key = API_KEY;
    std::string api_secret = API_SECRECT;

    BinaCPP::init(api_key, api_secret);

    Json::Value result;

    // interator index
    int i = 0;

    // recvWindow to specify the number of milliseconds after timestamp the request is valid for.
    // By default, recvWindow = 5000, i.e., 5000ms
    // This is due to network instability and cause requset may take longer to reach the server
    // The request must be processed within certain number of recvWindow or be rejected by server.
    long recvWindow = 5000;

    // Get exchange server time
    BinaCPP::get_serverTime(result);
    std::cout << "Server time is: " << result["serverTime"] << std::endl;

    // Get user account balance
    BinaCPP::get_account(recvWindow, result);
    for (i = 0; i < result["balances"].size(); i++)
    {
        string symbol = result["balances"][i]["asset"].asString();
        userBalance[symbol]["f"] = atof(result["balances"][i]["free"].asString().c_str());
        userBalance[symbol]["l"] = atof(result["balances"][i]["locked"].asString().c_str());
    }
    print_userBalance();

    // User data stream
    //BinaCPP::start_userDataStream(result);
    //cout << result << endl;

    //string ws_path = string ("/ws/");
    //ws_path.append(result["listenKey"].asString() );

    //BinaCPP_websocket::init();
 	//BinaCPP_websocket::connect_endpoint( ws_userStream_OnData , ws_path.c_str() ); 
    //BinaCPP_websocket::enter_event_loop(); 

    // LIMIT order
    //BinaCPP::send_order( "BNBBTC", "BUY", "LIMIT", "GTC", 2 , 0.0020000, "",0,0, recvWindow, result );
	//cout << result << endl;

    // MARKET order
	//BinaCPP::send_order_market( "BNBBTC", "BUY", "MARKET", 1 ,  "",0,0, recvWindow, result );
	//cout << result << endl;

    // ICEBERG order
    // BinaCPP::send_order( "BNBBTC", "BUY", "MARKET", "GTC", 1 , 0,   "",0,5, recvWindow , result );
	// cout << result << endl;

    //Get open order
    //BinaCPP::get_openOrders( "BNBBTC", recvWindow, result ) ;
	//cout << result << endl;

	BinaCPP::get_order("BNBBTC",126058306,"2jRxk6GfywZE5YpEkTkrLU", recvWindow, result);
	cout << result << endl;
	
	Json::Value openorder_result;
	BinaCPP::get_openOrders("BNBBTC", recvWindow, openorder_result);
	cout << openorder_result << endl;
	
	Json::Value allorder_result;
	BinaCPP::get_allOrders("BNBBTC",0,0,recvWindow, allorder_result);
	cout << allorder_result << endl;

	Json::Value trade_result;
	BinaCPP::get_myTrades("BNBBTC",0,0,recvWindow,trade_result);
	cout << trade_result << endl;
	
	Json::Value hr24_result;
	BinaCPP::get_24hr("BNBBTC", hr24_result);
	cout << hr24_result << endl;






}
