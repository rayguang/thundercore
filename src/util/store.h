#ifndef __STORE_H__
#define __STORE_H__

#include <string>

namespace util
{    
    class Store
    {
    public:        
        // SELECT: Fetch user API key
        virtual void fetchInfo(int userID, const char* exchangeName) = 0;
	    virtual int fetchUserID(int userID) = 0;

        // Insert: save request
        virtual void saveOrderRequest(const std::string& ctOrderID, const std::string& requestType, const std::string& subType, const int userID, const std::string& exchange, const std::string& symbol, const std::string& orderSide, const double quantity, const double price, const std::string& timeInForce) = 0;

        // Insert: save reply
        virtual void saveOrderAnswer(const std::string& ctOrderID, const std::string& status, const std::string& orderId, const std::string& clientOrderId) = 0;

        // Insert: save reply with error
        virtual void saveOrderAnswerError(const std::string& ctOrderID, const int code, const std::string& msg) = 0;

        virtual ~Store() = default;

    public:        
        inline std::string& getAPIkey() { return key; }
        inline std::string& getAPIsecret() { return secret; }

    protected:
        std::string key;
        std::string secret;
    };    
}

#endif
