#ifndef CACHED_RECORDS
#define CACHED_RECORDS

#include <map>
#include <unordered_map>
#include <string>
#include "fileCache.hpp"




namespace IO_CRUD
{
    class cachedRecords
    {

    public:
        typedef std::unordered_map <std::string,DB::my_recordSet> cachedRecordsType;
        typedef std::unordered_map <  SQLCMD::types::ul_long, cachedRecordsType > cachedRecordsByLayerIdType; 

        cachedRecords(std::string fileGuid);
        cachedRecords(std::string fileGuid,const SQLCMD::types::ul_long layerHash);
        void  actuallyRecordSet(const std::string& fileGuid);
        void  firstPrototype(std::string fileGuid);
        DB::my_recordSet  get(std::string fileGuid,const SQLCMD::types::ul_long layerHash = 0);
        DB::my_recordSet  get_current_record(std::string fileGuid);
        bool  exist(std::string fileGuid,const SQLCMD::types::ul_long layerHash = 0);
        DB::my_recordSet  selectMainDataSqlCmd(std::string fileGuid,const SQLCMD::types::ul_long layerHash = 0);
        cachedRecords();
        ~cachedRecords();

    private:
        cachedRecordsByLayerIdType  _cache;

    };
}

#endif