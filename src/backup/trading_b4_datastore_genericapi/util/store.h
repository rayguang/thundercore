#ifndef __STORE_H__
#define __STORE_H__

#include <string>

namespace util
{    
    class Store
    {
    public:        
        virtual void fetchInfo(int userID, const char* exchangeName) = 0;
        virtual ~Store() = default;

    public:        
        inline const std::string& getAPIkey() const { return key; }
        inline const std::string& getAPIsecret() const { return secret; }

    protected:
        std::string key;
        std::string secret;
    };    
}

#endif
