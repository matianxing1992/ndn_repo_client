#ifndef NDN_REPO_CLIENT_UTILS_DATA_STORAGE_HPP
#define NDN_REPO_CLIENT_UTILS_DATA_STORAGE_HPP

#include <sqlite3.h>

class DataStorage
{
public:
    sqlite3* GetSqlite3DBInstance();

private:

    static sqlite3* _db;
};

sqlite3* DataStorage::_db=nullptr;




#endif