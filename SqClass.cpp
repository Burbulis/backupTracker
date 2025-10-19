#include <string>
#include <map>
#include <vector>
#include "SqClass.h"
#include "_log.hpp"
#include <iostream>


namespace DB
{
std::list<std::string> sqlQueryCache::cacheSeq;

 #ifdef _DEBUG_SQL_I
    bool SQlite::debugModeOn;
#endif


   

    my_recordSet EMPTY(void)
    {
        DB::my_recordSet empty;
        empty[DB::_QUERY_RESULT_IS_EMPTY].push_back(DB::_QUERY_RESULT_IS_EMPTY);
        return (empty);
    }

    bool IsEmpty(my_recordSet t)
    {
      return (! t.begin()->first.compare(DB::_QUERY_RESULT_IS_EMPTY));
    }

    sqlQueryCache::sqlQueryCache(){};

    void 
    sqlQueryCache::add(std::string query)
    {
        cacheSeq.push_back(query);
    }
  
    size_t 
    sqlQueryCache::size(void)
    {
        return (cacheSeq.size());        
    }

    void 
    sqlQueryCache::set_count(size_t count_to_post)
    {
        count = count_to_post;
    }

    size_t
    sqlQueryCache::get_count(void)
    {
        return (count);
    }
  
    void 
    sqlQueryCache::exec(void)
    {
        if (cacheSeq.size()>=count)
        {
            flush();
        }
    }

    std::list<std::string> 
    sqlQueryCache::getQueries(void)
    {
        return (cacheSeq);
    }    

    bool 
    sqlQueryCache::isExist(std::string query)
    {
       return (std::find_if(cacheSeq.begin(),cacheSeq.end(),[query](std::string str){
            return( !query.compare(str) );
        }) != cacheSeq.end() );
    }

    void
    sqlQueryCache::flush()
    {
        DB::SQlite  sqlPerform("db_buckets");	
        for (std::list< std::string >::iterator It =  cacheSeq.begin();
                It  != cacheSeq.end();++It
                        )
        {
            std::string sql = *It;
            sqlPerform.execute( sql );
            It->clear(); 
        }
        cacheSeq.erase(
            std::remove_if(cacheSeq.begin(),cacheSeq.end(),[](std::string str){
                    return (str.empty());
            }),cacheSeq.end());

    }

    sqlQueryCache::~sqlQueryCache()
    {
    }



    void 
    SQlite::prepare(std::string sqlstatement)
    {
       if (!val_check(sqlite3_prepare(db, sqlstatement.c_str(), -1, &stmt, NULL)))
       {
           THROW(sqlstatement);
       }
    }

    void 
    SQlite::execute()
    {
        sqlite3_step( stmt );
        sqlite3_finalize(stmt); 
    }

    void 
    SQlite::execute(std::string sql)
    {
       char *errMsg = 0; 
       void *_ptr = reinterpret_cast<void*>(ptr.get());
       if(!val_check(sqlite3_exec(db, sql.c_str(),reinterpret_cast< int (*) (void *,int, char **, char **) >(&DB::SQlite::_call_back),_ptr, &errMsg)))
       {
             std::ostringstream ostr;
             ostr << "SQL_Error sql = " << sql  << " ErrorMsg ='" << errMsg << "'" << std::endl; 
             std::cout << ostr.str() << std::endl;
             sqlite3_free(errMsg);
             THROW(ostr.str());
       }
       return ;
    }

    std::vector<std::string>
    SQlite::
    getFieldNames(void)
    {
        std::vector<std::string> v;
        for (auto _valpair:(*ptr))
            v.push_back(_valpair.first);
        return v;
    }

    std::vector<std::string>
    SQlite::
    getValuesByFieldName(std::string field_name)
    {
        return ((*ptr)[field_name]);
    }

    bool
    SQlite::
    queryResultIsEmpty(void)
    {
      if (ptr->empty())
        return (true);
      else  
        return (( 0 == ptr->begin()->first.compare(_QUERY_RESULT_IS_EMPTY) ));
    }

    my_recordSet
    SQlite::
    getDataSet()
    {
        return ((*ptr));
    }
#ifdef _DEBUG_SQL_I   
    void 
    SQlite::  
    setDebug(void)
    {
        debugModeOn = true;
    }

    bool
    SQlite::  
    getDebug(void)
    {
        return (debugModeOn);
    }   


#endif
    SQlite::SQlite(std::string fileName):ptr(std::make_unique<my_recordSet>()),currentFile(fileName),
    db(NULL),stmt(NULL)
    {
#ifdef _DEBUG_SQL_I
        debugModeOn = false;
#endif
        if (!val_check(sqlite3_open(fileName.c_str(),&db)))
        {
             _LOG::out_s <<"fileName = " << fileName << "sqlite error code = " << sqlite3_errcode(db) << " sqlite error : " << sqlite3_errmsg(db) << std::endl;
             THROW(_LOG::out_s.str());
        }
    }

    SQlite::~SQlite()
    {
             
       if (!val_check(sqlite3_close(db)))
       {
           std::cout << "Sqlite Destructor Error!!!! fileName = " << currentFile << "sqlite error code = " << sqlite3_errcode(db) << " sqlite error : " << sqlite3_errmsg(db) << std::endl;
       }
    }
}
