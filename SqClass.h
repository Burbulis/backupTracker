#ifndef _HEADER_SQL_CLASS_
#define _HEADER_SQL_CLASS_
#include <map>
#include <vector>
#include <list>
#include <memory>
#include <string>
#include <sqlite3.h>
#include "throw_.hpp"
#include <stdio.h>
#include <string> 
#include <stdio.h>
#include <typeinfo>
#include <iostream>
#include <cassert>
#include <set>
#include <list>
#include <algorithm>
#include <typeinfo>
#include <cstring>

//#define _DEBUG_SQL_I
//#define _DEBUG_SQL_II
#define val_check(x) (x == SQLITE_OK)?true:false

namespace DB
{
    // map в который неминуемо упадёт результат запроса DB. 
    // Если конечно запрос корректен:)
    const std::string _QUERY_RESULT_IS_EMPTY = "QUERY_RESULT_IS_EMPTY";
    typedef std::map<std::string , std::vector<std::string>> my_recordSet;
    

    my_recordSet EMPTY(void);
    bool IsEmpty(my_recordSet t);


    typedef my_recordSet* my_recordSetPtr;
   

    class sqlQueryCache
    {
    private:
        size_t count;
        static std::list<std::string> cacheSeq;
        /* data */
    public:
        sqlQueryCache();
        sqlQueryCache(size_t count_to_post):count(count_to_post){};
        static void flush();
        void add(std::string query);
        bool isExist(std::string query);
        void set_count(size_t count_to_post);
        size_t get_count(void);
        std::list<std::string> getQueries(void); 
        void exec(void);
        size_t size(void);
        ~sqlQueryCache();
   
    };
      
    class SQlite
    {
#ifdef _DEBUG_SQL_I         
        static  bool debugModeOn;
#endif
        std::unique_ptr<my_recordSet> ptr;
        sqlite3 *db;
        sqlite3_stmt * stmt;
        int handle ;
        std::string currentFile;
        
        static  int _call_back(void* ptr, int column_count, char** columnValues, char** columnNames)
        {
            my_recordSetPtr p_ = reinterpret_cast<my_recordSetPtr>(ptr);
    #ifdef _DEBUG_SQL_I  
            std::cout << " debugModeOn = " << debugModeOn << std::endl;           
    #endif                 
            

            for (int i = 0; i < column_count; ++i)
            {
                if ((NULL == columnValues[i]) && (NULL != columnNames[i]))
                {
                    columnValues[i] = reinterpret_cast<char *>(malloc(10));
                    memset(columnValues[i],0,10);
                    memcpy(columnValues[i],"132231",6);
        #ifdef _DEBUG_SQL_I  
            std::cout << " debugModeOn = " << debugModeOn << std::endl;        
            if (debugModeOn)
            {                    
                    std::cout << "Error column_count columnNames[i] = " << columnNames[i] << std::endl;               
                    std::getc(stdin);
            } 
        #endif                 
                //  continue;
                //  (*p_)[_QUERY_RESULT_IS_EMPTY].push_back(_QUERY_RESULT_IS_EMPTY);
                }
                std::string columnName  = columnNames[i];
                std::string columnVal  = columnValues[i];       
        
        #ifdef _DEBUG_SQL_I
            if (debugModeOn)
            {         
                std::cout << " i = " << i << std::endl;
                std::cout << "columnName[" << i << "] = " << columnName << std::endl;
                std::cout << "columnVal[" << i << "] = "<< columnVal << std::endl;
            }  
        #endif                 
                (*p_)[columnName].push_back(columnVal);
            }
            return (0);
        }
        public:
        SQlite(std::string fileName);
        void prepare(std::string sqlstatement);
        void execute(std::string sql);
        void execute();
#ifdef _DEBUG_SQL_I            
       static void setDebug(void);
       static bool getDebug(void);
#endif        
        my_recordSet   getDataSet(void);
        std::vector<std::string> getFieldNames(void);
        std::vector<std::string> getValuesByFieldName(std::string field_name);
        bool queryResultIsEmpty(void);
        ~SQlite();
    };
   
}

#endif