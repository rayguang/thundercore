#include <Poco/Format.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/Session.h>
#include <Poco/Data/MySQL/Connector.h>

using namespace Poco::Data::Keywords;

int main(int argc, char* argv[])
{
    const std::string host="35.225.200.25";
    const std::string user="root";
    const std::string password="Bitcoin2019!!";
    const std::string db="trading";
    const std::string tablename = "db_test";

    Poco::Data::MySQL::Connector::registerConnector();

    try
    {
        // 0. connect & create table
        Poco::Data::Session session(Poco::Data::MySQL::Connector::KEY,
            "host=" + host + ";user=" + user + ";password=" + password +";db=" + db);

        session << Poco::format("CREATE TABLE `%s` ("
            "  `dow` enum('Monday','Tuesday','Wednesday','Thursday','Friday','Saturday','Sunday') default 'Sunday',"
            "  PRIMARY KEY  (`dow`)"
            ") DEFAULT CHARSET=latin1", tablename), now;

        // 1. insert from string
        std::string sday;
        Poco::Data::Statement inserts(session);
        inserts << Poco::format("INSERT INTO %s VALUES (?)", tablename), use(sday);
        sday = "Wednesday";
        inserts.execute();

        // 2. insert from int
        int nday;
        Poco::Data::Statement insertn(session);
        insertn << Poco::format("INSERT INTO %s VALUES (?)", tablename), use(nday);
        nday = 2;
        insertn.execute();

        // 3. select into string
        std::vector<std::string> outs;
        Poco::Data::Statement selects(session);
        selects << Poco::format("SELECT dow FROM %s", tablename), into(outs), now;

        // 4. select into int (problem)
        // This fails - the returned values are all zeroes.
        std::vector<int> outn;
        Poco::Data::Statement selectn(session);
        selectn << Poco::format("SELECT dow FROM %s", tablename), into(outn), now;
        
	// 5. select into int
        // This works.
        std::vector<int> outn1;
        Poco::Data::Statement selectn1(session);
        selectn1 << Poco::format("SELECT dow + 0 AS dow FROM %s", tablename), into(outn1), now;
    }
    catch (const Poco::Data::ConnectionFailedException &ce)
    {
        std::cout << ce.displayText() << std::endl;
    }
    catch (const Poco::Data::MySQL::StatementException &se)
    {
        std::cout << se.displayText() << std::endl;
    }

    Poco::Data::MySQL::Connector::unregisterConnector();

    return 0;
}
