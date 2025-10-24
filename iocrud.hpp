#include "base.h"
#include "BlocksAndLayers.hpp"
#include "tempTable.hpp"
#define DEBUG_SELECT_TRAKT
//#define _DEBUG_UPDATE_RECORDS_ON_
#define _DEBUG_CREATE_RECORDS_ON_
#define __SERIALISER_ON__
namespace IO_CRUD
{


  class sqlInsert
  {  
    private:
    	utils::minicache::Minicache mini_cache;
        DB::sqlQueryCache sqlInternalCache;
    public:
        sqlInsert(size_t _cache_size):sqlInternalCache(_cache_size)
        {}

        //EXEC_0
        #define EXEC_1
        template
        <
        typename bufferType
        >
        bool exec(TempObjects::autoTempCreator& tempDataStoreObject, bufferType faBuffer,const std::vector< std::string >& approwed,types::ul_long layerHash)
        {
    #ifdef EXEC_1
            BEGIN()
    #endif
           // const size_t counter = utils::getCountOf<bufferType>(faBuffer);
           //TODO: Захуярить FinalUpdate !!!!
           //
           //
            bool isFinalUpdate = false;//utils::isUpdateFinal<bufferType>(faBuffer);
            const size_t counter = faBuffer.blocks.size();
    #ifdef EXEC_1
                _LOG::out_s << "counter = " << counter << std::endl;
                LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
    #endif
            for (size_t i = 0 ; i < counter ; ++ i)
            {
                std::string blockGuid(faBuffer.blocks[i].blockGuid);
                isFinalUpdate  = (!faBuffer.blocks[i].end.compare("_FINA"));
                types::ul_long check = faBuffer.blocks[i].chk;
                types::ul_long blockHash = utils::minicache::str_hash( std::string(faBuffer.blocks[i].blockGuid) ) / 100000;
                SQLCMD::types::ul_long status = faBuffer.blocks[i].status;
                SQLCMD::types::ul_long size = faBuffer.blocks[i].buffer.size();    
                if  ((std::find(approwed.begin(),approwed.end(),blockGuid)==approwed.end()) || ((!check) & (!blockHash) & (!status)))
                {
            #ifdef EXEC_1 
                        _LOG::out_s << "* UNAPPROWED* blockGuid =" << blockGuid <<  " layerHash = " << layerHash << " check = " << check << std::endl;
                        LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
            #endif                
                    continue;
                }
            #ifdef EXEC_1
                        _LOG::out_s << " *APPROWED* blockGuid =" << blockGuid << " blockHash = " << blockHash << " layerHash = " << layerHash << " check = " << check << "status = "  << status << std::endl;
                        LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
            #endif

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
                    std::cout << " UPDATE TEST_TABLE  sql_prompt = "<< _sql_prompt <<  std::endl;;
                    sqlInternalCache.add(_sql_prompt);
                    SQLCMD::valdesc::clearInstanceCounter();
                }
            #ifdef EXEC_1
                _LOG::out_s << " UPDATE TEST_TABLE  sql_prompt = "<< _sql_prompt <<  std::endl;	
                LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
            #endif
            }// for (size_t i = 0 ; i < counter ; ++ i)
            if (isFinalUpdate)
            {
                sqlInternalCache.add(tempDataStoreObject.tableCopy());
            #ifdef EXEC_1
                LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"************************************************************");           
                std::list<std::string> qryDebug = sqlInternalCache.getQueries();
                for (auto str:qryDebug)
                {
                _LOG::out_s << "str ["<<str <<  "]" <<  std::endl;	
                    LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);  
                }            
                LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"************************************************************");           
                _LOG::out_s << "(before flush)sqlInternalCache.size = " << sqlInternalCache.size() << std::endl;
            #endif
                sqlInternalCache.flush();
            }
            #ifdef EXEC_1
            _LOG::out_s <<  "(after flush)sqlInternalCache.size = " << sqlInternalCache.size() << std::endl;
                LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
                END();
            #endif
            return (true);
            
        }//    bool exec(bufferType *faBuffer,const std::vector< std::string >& approwed,types::ul_long layerHash)
        
// real insert record
    #define EXEC_2
   
        bool exec(DA::mainDescNorm faBuffer,types::ul_long layerHash, bool isCreateInserter = true,bool isCreateSystem = true)
        {
        #ifdef EXEC_2
            BEGIN()
                _LOG::out_s << "exec__layerHash = " << layerHash << std::endl;
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
            #ifdef EXEC_2
                _LOG::out_s << "query = " << query << std::endl;
                LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
            #endif
                    sqlInternalCache.add(query);
                }
            }
            const size_t blocksCount =  faBuffer.blocks.size();
            bool isFinal = false;
            for (size_t i = 0 ; i < blocksCount ; ++i)
            {
            
                if (faBuffer.blocks[i].status == FILLERS::FILESIZE_INCREASE || faBuffer.blocks[i].status == FILLERS::FILLER_FILESIZE_CREATED)
                {
                    types::ul_long blockHash = utils::minicache::str_hash( faBuffer.blocks[i].blockGuid ) / 100000;  
                    if (isCreateInserter)
                    {
                        SQLCMD::Generator< SQLCMD::inserter_type , SQLCMD::Insert> insertBlocksAndBuckets("buckets_and_blocks",
                        {
                            {"fileGuid"	 ,SQLCMD::valdesc (faBuffer.fileGuid)},
                            {"blockGuid" ,SQLCMD::valdesc (faBuffer.blocks[i].blockGuid)},
                            {"blockHash" ,SQLCMD::valdesc (blockHash)},
                            {"idx"		 ,SQLCMD::valdesc (faBuffer.blocks[i].index)},
                            {"layerHash" ,SQLCMD::valdesc (layerHash)}
                        });	

                        query = insertBlocksAndBuckets();
            #ifdef EXEC_2
                _LOG::out_s << "query = " << query << std::endl;
                LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
            #endif
                
                        sqlInternalCache.add(query);
                    }
                    SQLCMD::valdesc::clearInstanceCounter();
                    SQLCMD::types::ul_long size = faBuffer.blocks[i].buffer.size();
                    SQLCMD::Generator< SQLCMD::inserter_type , SQLCMD::Insert	> InsertIntoBlocks("blocks",
                    {
                        {"chk",SQLCMD::valdesc      (faBuffer.blocks[i].chk)},
                        {"status",SQLCMD::valdesc   (
                        (faBuffer.blocks[i].status == FILLERS::FILESIZE_INCREASE)
                            ?FILLERS::FILLER_FILESIZE_INCREASE : faBuffer.blocks[i].status)},
                        {"blockHash",SQLCMD::valdesc(blockHash)},
                        {"layerHash",SQLCMD::valdesc(layerHash)},
                        {"size",SQLCMD::valdesc     (size)}
                    });	
                    query = InsertIntoBlocks();
        #ifdef EXEC_2
            _LOG::out_s << "query = " << query << std::endl;
            LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
        #endif
                    sqlInternalCache.add(query);
                }
                isFinal = (0 == faBuffer.blocks[i].end.compare("_FINA"));
                SQLCMD::valdesc::clearInstanceCounter();
                if (isFinal)
                    break;
            }
            if (isFinal)
            {
        #ifdef EXEC_2
            LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"FINALISED!!!");
        #endif
        #ifdef EXEC_2
        std::list<std::string> query_s = sqlInternalCache.getQueries();
        for (auto q:query_s)
        {    
            _LOG::out_s << "[query = " << q << "]" << std::endl;
            LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
        }
        
        #endif


            sqlInternalCache.flush();
                ret = (0 == sqlInternalCache.size()) ;
        #ifdef EXEC_2
            END()
        #endif
                return (ret);
            }
        #ifdef EXEC_2
            END()
        #endif
            return (ret);
        } // bool exec(DA::mainDescNorm faBuffer,types::ul_long layerHash)
    
    bool
    cacheIsEmpty(void)
    {
        TRACE();
        return (sqlInternalCache.size() == 0);     
    }

    void flushAnyWay(void)
    {
            TRACE();
        sqlInternalCache.flush();
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
    bool update(
        bufferType& faBuffer
    )
    {

        #ifdef DEBUG_UPDATE
            BEGIN()
        #endif
        LayersIO::layersHelper helper;
        enum{ FIRST_ELEMENT = 0 };
        std::optional< layersAndBlocks::hashAndBlocks > res;
        TempObjects::autoTempCreator tempStorage(faBuffer.fileGuid);        
        try
        {          
    #ifdef DEBUG_UPDATE
            LOGTOFILE(LOGHTML::messageType::MESSAGE,"*******************************************************");
                _LOG::out_s << "faBuffer.fileGuid = "<<  faBuffer.fileGuid <<   std::endl;	
                LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);

    #endif
            LayersIO::layersHelper lh(faBuffer.fileGuid);
            lh.setLayers();
            SQLCMD::types::ul_long  firstId = lh.getFirstLayer< SQLCMD::types::ul_long >("layerId");
            SQLCMD::types::ul_long layerHash = 
                *lh.getlayerSomeThingRelationNameByName< SQLCMD::types::ul_long , SQLCMD::types::ul_long >("layerHash","layerId",firstId);
            exec(faBuffer,layerHash,true,false);    
            SQLCMD::types::ul_long  lastId = lh.getLastLayer< SQLCMD::types::ul_long >("layerId");
            layerHash = 
                *lh.getlayerSomeThingRelationNameByName< SQLCMD::types::ul_long , SQLCMD::types::ul_long >("layerHash","layerId",lastId);
            exec(faBuffer,layerHash,false,false);    
  #ifdef DEBUG_UPDATE
            LOGTOFILE(LOGHTML::messageType::MESSAGE,"*******************************************************");
  #endif
  
            res =  layersAndBlocks::getBlocksWithLayersById<bufferType,DB::sqlQueryCache>(faBuffer,sqlInternalCache);
            if (!res)
            {
    #ifdef DEBUG_UPDATE
                LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"layerGuid not found by layerHash!");
    #endif
                return (false);
            }
            // TODO :LayerHash увеличивающих блоков должен быть как у остальных.
            //
            //
            //   std::cout << "Prepare to create temp table" << std::endl;
            // getc(stdin);  
              tempStorage(sqlInternalCache);
              //table.tableCopy();
    #ifdef _DEBUG_UPDATE_RECORDS_ON_
            for (auto r: *res )
            {
                _LOG::out_s << "hash =  " << r.first << std::endl;
                LOGTOFILE( LOGHTML::messageType::STRONG_WARNING,_LOG::out_s );
                for (auto x:r.second) 
                {
                    _LOG::out_s << "x.first : " << x.first << "  x.second = " << x.second << std::endl;
                    LOGTOFILE( LOGHTML::messageType::STRONG_WARNING,_LOG::out_s );
                }
            }
            _LOG::out_s << "sqlInternalCache.size = " << sqlInternalCache.size() << std::endl;
            LOGTOFILE( LOGHTML::messageType::STRONG_WARNING,_LOG::out_s );
    #endif
            
        }
        catch(const std::bad_alloc& ba)
        {
  #ifdef DEBUG_UPDATE
            _LOG::out_s << "exception detected : " << ba.what();
            LOGTOFILE( LOGHTML::messageType::STRONG_WARNING,_LOG::out_s );
  #endif
            return (false);
        }
        catch(const std::exception& e)
        {
  #ifdef DEBUG_UPDATE
            _LOG::out_s << "exception detected : " << e.what();
            LOGTOFILE( LOGHTML::messageType::STRONG_WARNING,_LOG::out_s );
  #endif
            return (false);
        }
        
        SQLCMD::types::ul_long  layerHash = res->begin()->first;
        std::vector< std::pair< std::string , bool> > blocksToNextLayer = res->begin()->second;
        std::optional<std::string> layerGuid;
        try
        {
   #ifdef DEBUG_UPDATE
            _LOG::out_s << "layerHash = "<<  layerHash << " blocksToNextLayer.size() = " << blocksToNextLayer.size() <<  std::endl;	
            LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
    #endif
            helper.refresh(); 
            helper.DBG_VIEW();
            layerGuid =  helper.getlayerSomeThingRelationNameByName<SQLCMD::types::ul_long , std::string >("layerGuid","layerHash",layerHash);
        }
        catch(const std::bad_alloc& ba)
        {
  #ifdef DEBUG_UPDATE
            _LOG::out_s << "exception detected : " << ba.what();
            LOGTOFILE( LOGHTML::messageType::STRONG_WARNING,_LOG::out_s );
  #endif
            return (false);
        }           
        catch(const std::exception& e)
        {
  #ifdef DEBUG_UPDATE
            _LOG::out_s << "exception detected : " << e.what();
            LOGTOFILE( LOGHTML::messageType::STRONG_WARNING,_LOG::out_s );
  #endif
            return (false);
        }         
        if (!layerGuid)
        {
  #ifdef DEBUG_UPDATE

            _LOG::out_s << "layerGuid not found by layerHash = " << layerHash <<  std::endl;
            LOGTOFILE( LOGHTML::messageType::STRONG_WARNING,_LOG::out_s );                    
  #endif
            return (false);
        }   
  #ifdef DEBUG_UPDATE
            _LOG::out_s << "layerGuid = "<<  *layerGuid << " res.size() = " << res->size() <<  std::endl;	
            LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
            for (auto z:blocksToNextLayer)
            {
                _LOG::out_s << "layerHash = " << layerHash << " block = " << z.first << " filler = " << z.second << "<br>" << std::endl;
                    LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
            }
    #endif
        if (!res)
        {
#ifdef DEBUG_UPDATE
            LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"nothing to update!");
#endif		
            throw std::logic_error("nothing to update!");
            return (false);
        }
        
        std::vector< std::string > approwed;
 
        std::transform(blocksToNextLayer.begin(),blocksToNextLayer.end(),std::back_inserter(approwed) ,[](std::pair<std::string,bool> p){
            return (p.first);    
        });
  #ifdef DEBUG_UPDATE
        _LOG::out_s << "before exec... approwed.size() = " << approwed.size() << std::endl;
        LOGTOFILE( LOGHTML::messageType::STRONG_WARNING,_LOG::out_s );
  #endif		
//need to create temp_table here
        if (exec<bufferType>(tempStorage,faBuffer,approwed,layerHash))
        {
        #ifdef __SERIALISER_ON__
        #ifdef DEBUG_UPDATE
                    _LOG::out_s << "after exec... approwed.size() = " << approwed.size() << std::endl;
                    LOGTOFILE( LOGHTML::messageType::STRONG_WARNING,_LOG::out_s );
            
                    LOGTOFILE(LOGHTML::messageType::WARNING,"try to serialise!");
        #endif	
            approwed.clear();	
            std::transform(blocksToNextLayer.begin(),blocksToNextLayer.end(),std::back_inserter(approwed) ,[](std::pair<std::string,bool> p){
                return ((p.second)?p.first:"");    
            });
            approwed.erase(
                std::remove_if(approwed.begin(),approwed.end(),[](std::string str){
                            return (str.empty());
                        }),approwed.end());
            try
            {  
                if (!approwed.empty())
                        diskOperations::stubSerialise(faBuffer,*layerGuid,approwed); 
            }
            catch(const std::bad_alloc& ba)
            {
    #ifdef DEBUG_UPDATE
                _LOG::out_s << "exception detected : " << ba.what();
                LOGTOFILE( LOGHTML::messageType::STRONG_WARNING,_LOG::out_s );
    #endif            
                return (false);
            }
            catch(const std::runtime_error &r)
            {
      #ifdef DEBUG_UPDATE           
                _LOG::out_s << "exception detected : " << r.what();
                LOGTOFILE( LOGHTML::messageType::STRONG_WARNING,_LOG::out_s );
      #endif          
                return (false);
            }
            catch(const std::exception& e)
            {
      #ifdef DEBUG_UPDATE     
                _LOG::out_s << "exception detected : " << e.what();
                LOGTOFILE( LOGHTML::messageType::STRONG_WARNING,_LOG::out_s );
      #endif        
                return (false);
            } 


            #endif	
   #ifdef DEBUG_UPDATE
            END();
    #endif
            return (true);
        }//if exec
    #ifdef DEBUG_UPDATE
          END()
    #endif
        return (false);
    } //Update
    
  };//the end of sqlInsert


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
} //IOCRUD namespace end