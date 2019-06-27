#include "iostream"

#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/Session.h>
#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/SessionFactory.h>
#include <tuple>

using namespace Poco::Data;
using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;
using Poco::format;

// Init Environment
const std::string host="35.225.200.25";
const std::string user="root";
const std::string password="Bitcoin2019!!";
const std::string db="trading";

//Poco::SharedPtr<Poco::Data::Session> _pSession = 0;

std::string _dbConnString = "host=" + host + ";user=" + user + ";password=" + password + ";db="+db +";compress=true;auto-reconnect=true";


std::pair<std::string, std::string> getAPIkey(int userID, const char*  exchangeName)
{
//    Poco::Data::MySQL::Connector::registerConnector();

    std::string api_key;
	std::string api_secret;

    try
    {
        // 0. connect & create table
        Poco::Data::Session session(Poco::Data::MySQL::Connector::KEY, _dbConnString);

        //_pSession = new Session(SessionFactory::instance().create(MySQL::Connector::KEY,_dbConnString));

        // std::vector<int> outn1;
        Statement selectn(session);
        selectn << "SELECT apikey, apisecret FROM exchange_API WHERE user_id=? and exchange_name=?", into(api_key), into(api_secret),  use(userID), use(exchangeName), now;

    }
    catch (const Poco::Data::ConnectionFailedException &ce)
    {
        std::cout << ce.displayText() << std::endl;
    }
    catch (const Poco::Data::MySQL::StatementException &se)
    {
        std::cout << se.displayText() << std::endl;
    }

 //   Poco::Data::MySQL::Connector::unregisterConnector();
	
    return std::make_pair(api_key, api_secret);


}



std::string getAPIsecret(int userID, int exchangeID)
{
  //  Poco::Data::MySQL::Connector::registerConnector();

    std::string api_secret;
	Poco::SharedPtr<Poco::Data::Session> _pSession;

    try
    {
        // 0. connect & create table
		Poco::Data::Session session(Poco::Data::MySQL::Connector::KEY, _dbConnString);

        //_pSession = new Session(SessionFactory::instance().create(MySQL::Connector::KEY,_dbConnString));


        // std::vector<int> outn1;
        Statement selectn(session);
        selectn << "SELECT apikey FROM exchange_API WHERE user_id=? and exchange_id=?", into(api_secret), use(userID), use(exchangeID), now;

    }
    catch (const Poco::Data::ConnectionFailedException &ce)
    {
        std::cout << ce.displayText() << std::endl;
    }
    catch (const Poco::Data::MySQL::StatementException &se)
    {
        std::cout << se.displayText() << std::endl;
    }

    //if ( session.isConnected())
    //    std::cout << "*** Connected to " << '(' << _dbConnString <<')' <<std::endl;

   // Poco::Data::MySQL::Connector::unregisterConnector();

    return api_secret;


}

// std::string getAPIsecret(int userID, int exchangeID)
// {
//     Poco::Data::MySQL::Connector::registerConnector();

//     std::string api_secrect;

//     try
//     {
//         // 0. connect & create table
//         Session session(Poco::Data::MySQL::Connector::KEY,
//             "host=" + host + ";user=" + user + ";password=" + password +";db=" + db);

//         std::vector<int> outn1;
//         Statement selectn(session);
//         selectn << "SELECT apisecrect FROM exchange_API WHERE user_id=? and exchang_id=? ", into(api_secret), use(userID), use(exchangeID), now;

//     }
//     catch (const Poco::Data::ConnectionFailedException &ce)
//     {
//         std::cout << ce.displayText() << std::endl;
//     }
//     catch (const Poco::Data::MySQL::StatementException &se)
//     {
//         std::cout << se.displayText() << std::endl;
//     }

//     Poco::Data::MySQL::Connector::unregisterConnector();

//     return api_secrect;


// }
