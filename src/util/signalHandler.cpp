#include "signalHandler.h"

#include <Poco/Bugcheck.h>

using namespace util;

SignalHandler *SignalHandler::signalHandler = nullptr;

SignalHandler::SignalHandler(Service& serviceToStop_, const std::vector<int>& signals_)       
    :   serviceToStop(serviceToStop_),
        signals(signals_)
{
    poco_assert(nullptr == SignalHandler::signalHandler);// just one is necessary
    SignalHandler::signalHandler = this;        

    for(auto iSignal: signals)        
        signal(iSignal, SignalHandler::handleSignal);        
}

SignalHandler::~SignalHandler()
{
    SignalHandler::signalHandler = nullptr;
}

void SignalHandler::handleSignal(int sig)
{    
    poco_assert(nullptr != SignalHandler::signalHandler);
    
    size_t iSignal=0;
    while ( SignalHandler::signalHandler->signals[iSignal] != sig &&
            iSignal < SignalHandler::signalHandler->signals.size())
        ++iSignal;

    if (iSignal < SignalHandler::signalHandler->signals.size())        
        SignalHandler::signalHandler->serviceToStop.stop();        
}
