#include "base.h"
//#include "BlocksAndLayers.hpp"
#include "layers/layersHelper.h"
#include "tempTable.hpp"
#include <future>
#include <thread>
#include <chrono>
#define DEBUG_SELECT_TRAKT
//#define _DEBUG_UPDATE_RECORDS_ON_
#define _DEBUG_CREATE_RECORDS_ON_
#define __SERIALISER_ON__

#ifndef _IO_CRUD_
#define _IO_CRUD_

namespace IO_CRUD
{
#define GET_FILE_GUID_BY_NAME
        std::string
        getFileGuidByName(std::string fileName , std::string pathToFile)
        {
#ifdef  GET_FILE_GUID_BY_NAME		
    BEGIN()
    _LOG::out_s <<	"fileName = " <<	fileName << std::endl;
    _LOG::out_s << "pathToFile = " << pathToFile << std:: endl;
    LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
#endif
        SQLCMD::Generator< SQLCMD::selector_type_v, SQLCMD::Select<> > fsObjects("system_ids", SQLCMD::get_ordered
        (
        SQLCMD::selector_type {
            {"fileGuid"     ,SQLCMD::valdesc()},
            {"fileName"     ,SQLCMD::valdesc(fileName, SQLCMD::valdesc::WHERE_ON,false)},
            {"pathFile"     ,SQLCMD::valdesc(pathToFile , SQLCMD::valdesc::WHERE_AND,false)}
            }));
        auto s = fsObjects();
        DB::SQlite  sq("db_buckets");
#ifdef  GET_FILE_GUID_BY_NAME		
    _LOG::out_s << "s = " << s << std:: endl;
    LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
#endif

        sq.execute(s);
        auto retQ = sq.getDataSet();
#ifdef  GET_FILE_GUID_BY_NAME		
    LOGTOFILE(LOGHTML::messageType::MESSAGE,"sql query executed");
    _LOG::out_s << "retQ.size() = " << retQ.size() << std:: endl;
    LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);       
    for (auto f:retQ)
        _LOG::out_s << "f = " << f.first << std:: endl;
    LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);      
#endif
        std::string ret = (!retQ.empty())?(!retQ["fileGuid"].empty())?retQ["fileGuid"][0]:"_EMPTY_FileGuid_":"_EMPTY_DATASET_";
#ifdef  DEBUG_SELECT_TRAKT		
    _LOG::out_s << "ret = " << ret << std:: endl;
    LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
    END();
#endif
        return ( ret );
    }//  getFileGuidByName

  class sqlInsert
  {  
    private:
        uint64_t updateExecCallCount;
        uint64_t insertExecCallCount;
    	utils::minicache::Minicache mini_cache;
        TempObjects::autoTempCreator tempStorage;
        DB::sqlQueryCache sqlInternalCacheInserters;
        DB::sqlQueryCache sqlInternalCacheUpdaters;
        DB::sqlQueryCache sqlInternalCacheTableCreators;
    public:
        sqlInsert(size_t _cache_size):updateExecCallCount(0),insertExecCallCount(0)//:sqlInternalCache(_cache_size)
        {}
    //    bool execUpdate(TempObjects::autoTempCreator& tempDataStoreObject,DA::mainDescNorm faBuffer,
    //          const std::vector< std::string >& approwed,types::ul_long layerHash);
    //  bool execInsert(DA::mainDescNorm faBuffer,types::ul_long layerHash, bool isCreateInserter = true,bool isCreateSystem = true);
 
        //EXEC_0
 
        template
        <
        typename bufferType
        >
        bool isFinalUpdate(bufferType faBuffer)
        {
            bool isFinal = false;
            const size_t counter = faBuffer.blocks.size();
            for (size_t i = 0; ((i < counter) && (!isFinal))  ; ++i)
            {
                isFinal = (!faBuffer.blocks[i].end.compare("_FINA"));
            }
            return isFinal;
        }



        #define EXEC_UPDATE_CMD
        template
        <
        typename bufferType
        >
        bool execUpdate(bufferType faBuffer,bool FinalUpdate)
        {
    #ifdef EXEC_UPDATE_CMD
            BEGIN()
    #endif
           // bool FinalUpdate = isFinalUpdate(faBuffer);

            ++updateExecCallCount;
             const size_t counter = faBuffer.blocks.size();
            #ifdef EXEC_UPDATE_CMD
                _LOG::out_s << "updateExecCallCount = " << updateExecCallCount << " counter = "<< counter << " FinalUpdate = " << FinalUpdate << std::endl;	
                LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
            #endif
            for (size_t i = 0 ; i < counter ; ++ i)
            {
                std::string blockGuid(faBuffer.blocks[i].blockGuid);
            #ifdef EXEC_UPDATE_CMD
                _LOG::out_s << " FinalUpdate = "<< FinalUpdate << " faBuffer.blocks[i].end = " << faBuffer.blocks[i].end << std::endl;	
                LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
            #endif
                types::ul_long check = faBuffer.blocks[i].chk;
                types::ul_long blockHash = utils::minicache::str_hash( std::string(faBuffer.blocks[i].blockGuid) ) / 100000;
                SQLCMD::types::ul_long status = faBuffer.blocks[i].status;
                SQLCMD::types::ul_long size = faBuffer.blocks[i].buffer.size();    
                
                bool isFiller = utils::isFiller(faBuffer.blocks[i]);

                SQLCMD::selector_type m_update =
                {
                            { "status"  , SQLCMD::valdesc(status)},
                            { "chk"  , SQLCMD::valdesc(check)},
                            { "size" , SQLCMD::valdesc(size)},
                            { "blockHash"  , SQLCMD::valdesc(blockHash,SQLCMD::valdesc::WHERE_ON,false)}

                };
 
                SQLCMD::Generator <  SQLCMD::selector_type_v, SQLCMD::Update > update("test_table", SQLCMD::type_v(m_update));
                std::string _sql_prompt = update();
                {
                    sqlInternalCacheUpdaters.add(_sql_prompt);
                    SQLCMD::valdesc::clearInstanceCounter();
                }
            #ifdef EXEC_UPDATE_CMD
                _LOG::out_s << " UPDATE_sql_prompt = "<< _sql_prompt << "  sqlInternalCacheUpdaters.size() = " << sqlInternalCacheUpdaters.size() << std::endl;	
                LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
            #endif
            }

            if (FinalUpdate)
            {
                //sqlInternalCacheUpdaters.size()
               // std::cout << "====UPDATE====" << std::endl;
               // getc(stdin);          
            
                std::list<std::string> qryUpdate = sqlInternalCacheUpdaters.getQueries();
            #ifdef EXEC_UPDATE_CMD
                LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"**FINAL__UPDATE**");           
                LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"***Updaters***");           
                LOGTOFILE(LOGHTML::messageType::MESSAGE,"try to ...");         
                _LOG::out_s << "(before flush) qryUpdate.size = " << qryUpdate.size() << std::endl;
                LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);  
             //   _LOG::out_s <<  " sqlInternalCacheTableCreators = " << sqlInternalCacheTableCreators.size() << std::endl;
             //   LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
                END();
            #endif
                return (true);
            }
            #ifdef EXEC_UPDATE_CMD
                END();
           #endif
           return (false);
            
        }//    bool exec(bufferType *faBuffer,const std::vector< std::string >& approwed,types::ul_long layerHash)
        
// real insert record
    #define EXEC_INSERT_CMD
  
       bool execInsert(DA::mainDescNorm faBuffer,types::ul_long layerHash,std::function<bool (typename DA::ul_long)> statusChecker, bool isCreateInserter = true,bool isCreateSystem = true)
       {
        ++insertExecCallCount;
        #ifdef EXEC_INSERT_CMD
            BEGIN()
            _LOG::out_s << "insertExecCallCount = " << insertExecCallCount << "exec_layerHash = " << layerHash << " isCreateInserter = " << isCreateInserter << " isCreateSystem = " << isCreateSystem << std::endl;
            LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
        #endif
            
            std::string query=""; //TODO : Запилить признак конца букета, и признак конца сессии.
            bool ret = false;
           
            if (isCreateSystem)
            {
        
                if (!mini_cache.exist(utils::minicache::str_hash(std::string(faBuffer.fileGuid))))
                {
                    SQLCMD::Generator< SQLCMD::inserter_type , SQLCMD::Insert> insertSystem_ids("system_ids",
                        {
                            {"pathFile",SQLCMD::valdesc(faBuffer.pathFile)},
                            {"fileName",SQLCMD::valdesc(faBuffer.fileName)},
                            {"fileGuid",SQLCMD::valdesc(faBuffer.fileGuid)},
                            {"ftCreationTime",SQLCMD::valdesc( utils::time::toTimeString  ( faBuffer.creationTime     ))},
                            {"ftLastAccessTime",SQLCMD::valdesc( utils::time::toTimeString(faBuffer.lastAccessTime  ))},
                            {"ftLastWriteTime",SQLCMD::valdesc( utils::time::toTimeString (faBuffer.lastWriteTime    ))}
                        }
                    );
                    query = insertSystem_ids();
            #ifdef EXEC_INSERT_CMD
                _LOG::out_s << "query = " << query << std::endl;
                LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
            #endif
            sqlInternalCacheInserters.add(query);
                  // sqlInternalCache.add(query);
                }
            }
            const size_t blocksCount =  faBuffer.blocks.size();
            #ifdef EXEC_INSERT_CMD
                _LOG::out_s << ">> blocksCount = " << blocksCount << std::endl;
                LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
            #endif

            for (size_t i = 0 ; i < blocksCount ; ++i)
            {
            

                if (!statusChecker(faBuffer.blocks[i].status))
                {
                    continue; 
                }
                     types::ul_long blockHash = utils::minicache::str_hash( faBuffer.blocks[i].blockGuid ) / 100000;  
                    if (isCreateInserter)
                    {
                        uint64_t index =  faBuffer.blocks[i].index;
                        SQLCMD::Generator< SQLCMD::inserter_type , SQLCMD::Insert> insertBlocksAndBuckets("buckets_and_blocks",
                        {
                            {"fileGuid"  ,SQLCMD::valdesc (faBuffer.fileGuid)},
                            {"blockGuid" ,SQLCMD::valdesc (faBuffer.blocks[i].blockGuid)},
                            {"blockHash" ,SQLCMD::valdesc (blockHash)},
                            {"idx"       ,SQLCMD::valdesc (index)},
                            {"layerHash" ,SQLCMD::valdesc (layerHash)}
                        }); 

                        query = insertBlocksAndBuckets();
            #ifdef EXEC_INSERT_CMD
                _LOG::out_s << "query = " << query << " index  = " << index << std::endl;
                LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
            #endif
                        sqlInternalCacheInserters.add(query);     
                    }
                    SQLCMD::valdesc::clearInstanceCounter();
                    SQLCMD::types::ul_long size = faBuffer.blocks[i].buffer.size();
                    SQLCMD::Generator< SQLCMD::inserter_type , SQLCMD::Insert   > InsertIntoBlocks("blocks",
                    {
                        {"chk",SQLCMD::valdesc      (faBuffer.blocks[i].chk)},
                        {"status",SQLCMD::valdesc   (
                             (faBuffer.blocks[i].status == DA::FILLERS::FILESIZE_INCREASE)
                                   ? DA::FILLERS::FILLER_FILESIZE_INCREASE : faBuffer.blocks[i].status)},
                        {"blockHash",SQLCMD::valdesc(blockHash)},
                        {"layerHash",SQLCMD::valdesc(layerHash)},
                        {"size",SQLCMD::valdesc     (size)}
                    }); 
                    query = InsertIntoBlocks();
            #ifdef EXEC_INSERT_CMD
                _LOG::out_s << "query = " << query << std::endl;
                LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
            #endif                 
                    sqlInternalCacheInserters.add(query);

                ret = (0 == faBuffer.blocks[i].end.compare("_FINA"));
 
                if (ret)
                    break;

                }
               SQLCMD::valdesc::clearInstanceCounter();
            #ifdef EXEC_INSERT_CMD
                _LOG::out_s << "ret = " << ret << " sqlInternalCacheInserters.size() = " <<  sqlInternalCacheInserters.size() << std::endl;
                
                LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
            #endif 

            #ifdef EXEC_INSERT_CMD
                END() 
            #endif 
             sqlInternalCacheInserters.flush();      
            return (ret);
        } // bool execInsert(...)
    bool
    cacheIsEmpty(void)
    {
        return (sqlInternalCacheInserters.size() == 0);     
    }

    void flushAnyWay(void)
    {
       // sqlInternalCacheInserters.flush();
    }

    /// @brief обновление блоков переданных клиентом.
    /// @tparam bufferType тип буффера 
    /// @param faBuffer //буффер содержащий блоки для обновления
    /// @return всё прошло хорошо -true/всё плохо -false.
#define DEBUG_UPDATE
    template
    <
        typename bufferType
    >
    bool update(const bufferType& faBuffer  )
    {

        #ifdef DEBUG_UPDATE
            BEGIN()
        #endif
        Layers::layersHelper helper;
        //LayersIO::layersHelper helper;
        
        enum{ FIRST_ELEMENT = 0 };
       
        /// TempObjects::autoTempCreator tempStorage(faBuffer.fileGuid); 
        //std::array<SQLCMD::types::ul_long,2> hashes = tempStorage.getLayers(faBuffer.fileGuid);//layerHashes(faBuffer.fileGuid);   
        //SQLCMD::types::ul_long  firstLayerHash = hashes[TempObjects::autoTempCreator::PREV_LAYER_HASH];
        //SQLCMD::types::ul_long  lastLayerHash  = hashes[TempObjects::autoTempCreator::LAST_LAYER_HASH];   
        

       std::optional< std::array<SQLCMD::types::ul_long,2> > hashes = std::nullopt;
       hashes = tempStorage.getLayers(faBuffer.fileGuid);//layerHashes(faBuffer.fileGuid);   
        
        SQLCMD::types::ul_long  prevLayerHash  = hashes->at(TempObjects::autoTempCreator::PREV_LAYER_HASH);
        SQLCMD::types::ul_long  lastLayerHash  = hashes->at(TempObjects::autoTempCreator::LAST_LAYER_HASH);   
        if (tempStorage.complete())
        {
    #ifdef DEBUG_UPDATE
            LOGTOFILE(LOGHTML::messageType::MESSAGE,"hashes Ok...");
            _LOG::out_s << " prevLayerHash = " << prevLayerHash << " lastLayerHash = " << lastLayerHash <<  std::endl;	
            LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
    #endif
        }
        bool FinalUpdate = isFinalUpdate(faBuffer);

    #ifdef DEBUG_UPDATE
            LOGTOFILE(LOGHTML::messageType::MESSAGE,"*******************************************************");
                _LOG::out_s << "faBuffer.fileGuid = "<<  faBuffer.fileGuid << " faBuffer.blocks.size() = " << faBuffer.blocks.size() <<  std::endl;	
                LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
    #endif
            helper.resetLayers(); 
            std::optional <SQLCMD::types::ul_long>  prevId =  helper.getIdByHash(prevLayerHash); 
            std::optional <SQLCMD::types::ul_long>  lastId = helper.getIdByHash(lastLayerHash); 
    #ifdef DEBUG_UPDATE
                _LOG::out_s <<"prevId = " << *prevId << " lastId = "<<  *lastId <<   std::endl;	
                LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
    #endif
    #ifdef DEBUG_UPDATE
                _LOG::out_s <<"***prevLayerHash = "<<  prevLayerHash << " lastLayerHash = " << lastLayerHash << std::endl;	
                LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
    #endif
               //  if (faBuffer.blocks[i].status == DA::FILLERS::FILESIZE_INCREASE || faBuffer.blocks[i].status == DA::FILLERS::FILLER_FILESIZE_CREATED
              //          || DA::FILLERS::FILLER_IS_ZERO_FILE )
       /* std::future<bool> retInsert = 
        std::async(std::launch::async,[this](bufferType _faBuffer,SQLCMD::types::ul_long  _prevLayerHash){
            #ifdef DEBUG_UPDATE
                _LOG::out_s <<"***_prevlayerHash = "<<  _prevLayerHash <<  std::endl;	
                LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
            #endif
          
            return (this->execInsert(_faBuffer,_prevLayerHash,[](DA::ul_long var){
                    return (var == DA::FILLERS::FILESIZE_INCREASE || var == DA::FILLERS::FILLER_FILESIZE_CREATED
                    || var == DA::FILLERS::FILLER_IS_ZERO_FILE);
                },true,false));      
        },faBuffer,prevLayerHash);
        */

         std::future<bool> retInsert = 
        std::async(std::launch::async,[this](bufferType _faBuffer,SQLCMD::types::ul_long  _prevLayerHash){
           return (this->execInsert(_faBuffer,_prevLayerHash,[](DA::ul_long var){
                    return (var == DA::FILLERS::FILESIZE_INCREASE || var == DA::FILLERS::FILLER_FILESIZE_CREATED);
                },true,false));      
        },faBuffer,prevLayerHash);

        std::future<bool> retUpdate =
        std::async(std::launch::async,[this](bufferType _faBuffer,bool _FinalUpdate){
            return (this->execUpdate(_faBuffer,_FinalUpdate));
        },faBuffer,FinalUpdate);

        retUpdate.wait();

        auto furureSerialise = std::async(std::launch::async,[this](bufferType _faBuffer,SQLCMD::types::ul_long _layerHash)
        {
            this->serialise(_faBuffer,_layerHash);    
        },faBuffer,lastLayerHash);


        if (FinalUpdate)
        {

             auto futureFlush =  std::async(std::launch::async,[this](TempObjects::autoTempCreator& _tempSotrage)
              {
                this->totalFlush(_tempSotrage);
            },std::ref(tempStorage));
            retInsert.wait();
            retUpdate.wait();
 
            futureFlush.wait();
            furureSerialise.wait();
        }    

       // serialise(_faBuffer,layerHash);
   #ifdef DEBUG_UPDATE
            END();
   #endif
        return (true);
    }

    #define TOTAL_FLUSH
    void totalFlush(TempObjects::autoTempCreator& tempStorage)
    {
    #ifdef TOTAL_FLUSH
        BEGIN()
    #endif
        std::list<std::string> query_s = sqlInternalCacheInserters.getQueries();//sqlInternalCache.getQueries();
        this->sqlInternalCacheTableCreators.insertQueries(sqlInternalCacheInserters.getQueries());
        tempStorage(this->sqlInternalCacheTableCreators); 
        this->sqlInternalCacheTableCreators.insertQueries(sqlInternalCacheUpdaters.getQueries());
        this->sqlInternalCacheTableCreators.add(tempStorage.tableCopy());
        this->sqlInternalCacheTableCreators.flush();      
    #ifdef TOTAL_FLUSH
        _LOG::out_s << "  this->sqlInternalCacheTableCreators.size() = " << sqlInternalCacheTableCreators.size() << std::endl;	
        LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
        END()
    #endif
    
    }

    #define __SERIALISER_ON__    
    template<
        typename bufferType
    >
    void
    serialise(bufferType& faBuffer , SQLCMD::types::ul_long layerHash )
    {
        #ifdef __SERIALISER_ON__
                BEGIN();
                _LOG::out_s << " lastLayerHash = " << layerHash << std::endl;	
                LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
        #endif 
        Layers::layersHelper helper;
         //getlayerSomeThingRelationNameByName< SQLCMD::types::ul_long , std::string >
         //  ("layerGuid","layerHash",layerHash);
        std::optional < std::string >  layerGuid = helper.getGuidById( *helper.getIdByHash(layerHash));
        #ifdef __SERIALISER_ON__
                _LOG::out_s << "layerGuid = " << ((layerGuid)? *layerGuid : std::string("LayerGuid - Error detected") ) << std::endl;
                LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
        #endif    
            diskOperations::stubSerialise(faBuffer,
                [](DA::ul_long var){
                    return (
                            var == DA::FILLERS::FILESIZE_INCREASE || 
                            var == DA::FILLERS::FILLER_FILESIZE_CREATED || 
                            var == DA::FILLERS::FILLER_FILESIZE_REPLACEMENT
                            );
                },*layerGuid);   
   //     #endif
    }
    
    };//the end of sqlInsert
} //IOCRUD namespace end
#endif