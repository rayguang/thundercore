#ifndef __SIGNAL_HANDLER_H__
#define __SIGNAL_HANDLER_H__

#include <vector>
#include <signal.h>

#include "service.h"

namespace util
{
    // to properlly handle the stop of service
    class SignalHandler
    {
    public:
        SignalHandler(Service& serviceToStop, const std::vector<int>& signals = {SIGTERM, SIGQUIT, SIGINT});        
        virtual ~SignalHandler();

    private:
        static void handleSignal(int sig);

    private:
        Service&                serviceToStop;
        std::vector<int>        signals;    
        static SignalHandler    *signalHandler;
    };
}

#endif
