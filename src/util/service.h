#ifndef __UTIL_SERVICE_H__
#define __UTIL_SERVICE_H__

namespace util
{
    class Service
    {
    public:
        virtual void start() = 0;
        virtual void stop() = 0;
        
        virtual ~Service() = default;
    };
}

#endif
