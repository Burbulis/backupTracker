#ifndef _FILE_CACHE_
#define _FILE_CACHE_
#include <map>
#include <unordered_map>
#include <string>
#include "../layers/layersHelper.h"
#include "../base.h"


namespace IO_CRUD
{

struct _fileCache
    {
        typedef std::unordered_map<std::string,DB::my_recordSet> cacheType;
        _fileCache(){} 
#define GET_FILES_AND_GUIDS_BY_PATH
        DB::my_recordSet
        getFilesAndGuidsByPath(std::string pathToFile)
        {
    #ifdef GET_FILES_AND_GUIDS_BY_PATH
          BEGIN()
    #endif
            if (_cache.count(pathToFile) > 0)
               return (_cache[pathToFile]);

            SQLCMD::Generator< SQLCMD::selector_type_v, SQLCMD::Select<> > fsObjects("system_ids", SQLCMD::get_ordered
            (
              SQLCMD::selector_type  {
                {"fileGuid"     ,SQLCMD::valdesc()},
                {"fileName"     ,SQLCMD::valdesc()},
                {"pathFile"     ,SQLCMD::valdesc(pathToFile , SQLCMD::valdesc::WHERE_ON)}
                }));
            auto s = fsObjects();
            DB::SQlite  sq("db_buckets");
    #ifdef GET_FILES_AND_GUIDS_BY_PATH
                _LOG::out_s << "s =" << s << std::endl;
                LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
    #endif
            sq.execute(s);          
            _cache[pathToFile] = sq.getDataSet();
            const std::string CACHE_ERRROR = std::string("can't retrieve information about files in path = " + pathToFile);
            if (_cache[pathToFile].empty())
                throw std::runtime_error(CACHE_ERRROR);
    #ifdef GET_FILES_AND_GUIDS_BY_PATH
           END()
    #endif
           return (_cache[pathToFile]);
        }
    private:
     static cacheType  _cache;
   };
}
#endif