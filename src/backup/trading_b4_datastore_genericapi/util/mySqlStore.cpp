#include "mySqlStore.h"

#include <Poco/Mutex.h>
#include <Poco/Data/Session.h>
#include <Poco/Data/MySQL/Connector.h>
#include "Poco/Data/Session.h"
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionPool.h>
#include <Poco/Thread.h>

#include <queue>


#include <iostream>

using namespace util;

/******************************************************************************/
class RegisterConnector
{
public:
    RegisterConnector()
    {
        Poco::Data::MySQL::Connector::registerConnector();    
    }

    virtual ~RegisterConnector()
    {
        if (nullptr != sessions.get())
            sessions->shutdown();

        Poco::Data::MySQL::Connector::unregisterConnector();    
    }

    public:
        Poco::Mutex                                 mutex;
        Poco::SharedPtr<Poco::Data::SessionPool>    sessions;    
};
static RegisterConnector registerConnector;


/******************************************************************************/
void MySqlStore::fetchInfo(int userID, const char* exchangeName)
{
    Poco::SharedPtr<Poco::Data::Session> session;    
    {
        poco_assert(nullptr != registerConnector.sessions.get());        
        
        while (nullptr == session.get())
        {
            try
            {
                Poco::Mutex::ScopedLock lock(registerConnector.mutex);
                session.assign(new Poco::Data::Session(registerConnector.sessions->get()));
            }
            catch(Poco::Data::SessionPoolExhaustedException& e)
            {   
                Poco::Thread::yield();                
            }            
        }
    }
    poco_assert(nullptr != session.get());  

    // Get ID
    Poco::Data::Statement select(*session);

    select  << "SELECT apikey, apisecret FROM exchange_API WHERE user_id=? AND exchange_name=?",
                    Poco::Data::Keywords::into(this->key),
                    Poco::Data::Keywords::into(this->secret),
                    Poco::Data::Keywords::use(userID),
                    Poco::Data::Keywords::bind(exchangeName);

    select.execute();
}
 
void MySqlStore::initDB(const char* connectString, const int minSessions, const int maxSessions)
{
    registerConnector.sessions.assign(new Poco::Data::SessionPool("MySql", connectString, minSessions, maxSessions));    
}
