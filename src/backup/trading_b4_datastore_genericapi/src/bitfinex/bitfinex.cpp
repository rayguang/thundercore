#include "bitfinex.h"

#include <iostream>

using namespace Trading::Bitfinex;

Engine::Engine()
{
}

/******************************************************************************/
std::string BUY::process(const std::string& request)
{
    std::cout << "Bitfinex Request is: " << request << std::endl;
    return std::string("{\"RequestID\":\"987654321\",\"Answer\":\"OK\"}");    
}
