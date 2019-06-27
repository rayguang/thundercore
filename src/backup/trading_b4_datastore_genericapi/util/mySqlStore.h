#ifndef __MYSQLSTORE_H__
#define __MYSQLSTORE_H__

#include "store.h"

namespace util
{
    class MySqlStore : public Store
    {
    public:
        static void initDB(const char* connectString, const int minSessions, const int maxSessions);

        virtual void fetchInfo(int userID, const char* exchangeName) override;
        virtual ~MySqlStore()  override = default;
    };
}

#endif
