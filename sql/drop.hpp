#ifndef _DROPTABLE_SQL_
#define _DROPTABLE_SQL_
#include <string>
#include <iostream>
#include <map>
#include <vector> 
#include <algorithm>
#include "sqlcmdType.hpp"
namespace SQLCMD
{

    struct DropTable
    {
        enum { SELECTABLE_SUPPORT = false };
        DropTable(std::string& res, std::string tablename, size_t end) :tableName(tablename)
        {}

        bool operator()()
        {
            res_ = "DROP TABLE IF EXISTS " + tableName;
            return (true);
        }
        std::string get(void)
        {
            return res_;
        }

        std::string get(size_t limit)
        {
            return res_;
        }

        void setEnd(size_t szend)
        {}

        std::string res_;
        std::string tableName;
    };

};
#endif