#include "minicache.h"
/*------------------------------------------------------------------------
*
-------------------------------------------------------------------------*/
#define CACHED_RECORDS_DEBUG

    IO_CRUD::_fileCache::cacheType _cache;

    IO_CRUD::
    cachedRecords::
    cachedRecords(std::string fileGuid)
    {

        #ifdef  CACHED_RECORDS_DEBUG		
            BEGIN()
            _LOG::out_s << ".ctor(0) fileGuid = " << fileGuid << std:: endl;
            LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
        #endif

            try
            {
                actuallyRecordSet(fileGuid);
            }
            catch(const std::exception& e)
            {
        #ifdef  CACHED_RECORDS_DEBUG		
            _LOG::out_s << "EXCEPTION DETECTED = " << e.what() << std:: endl;
            LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
        #endif

            }
        #ifdef  CACHED_RECORDS_DEBUG		
            END();
        #endif       
    }
/*------------------------------------------------------------------------
*
-------------------------------------------------------------------------*/
    IO_CRUD::    
    cachedRecords::
    cachedRecords(std::string fileGuid,const SQLCMD::types::ul_long layerHash)
    {
        #ifdef  CACHED_RECORDS_DEBUG		
            BEGIN()
            _LOG::out_s << "cachedRecords.ctor(1) fileGuid = " << fileGuid << " layerHash = " << layerHash <<  std:: endl;
            LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
        #endif

            try
            {
                actuallyRecordSet(fileGuid);
            // selectMainDataSqlCmd(fileGuid,layerHash);
            }
            catch(const std::exception& e)
            {
        #ifdef  CACHED_RECORDS_DEBUG		
            _LOG::out_s << "EXCEPTION DETECTED = " << e.what() << std:: endl;
            LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
        #endif
            }
        #ifdef  CACHED_RECORDS_DEBUG		
            END();
        #endif        

    }
/*------------------------------------------------------------------------
*
-------------------------------------------------------------------------*/
    void 
    IO_CRUD::
    cachedRecords::
    actuallyRecordSet(const std::string& fileGuid)
    {
      #ifdef DEBUG_SELECT_TRAKT 
            BEGIN();
        #endif	
 
        firstPrototype(fileGuid);
 
        #ifdef DEBUG_SELECT_TRAKT 
            END();
        #endif	
    }    

    #define FIRST_PROTOTYPE 
    void 
    IO_CRUD::
    cachedRecords::
    firstPrototype(std::string fileGuid)
    {
        #ifdef  FIRST_PROTOTYPE
          BEGIN();
        #endif	
        Layers::layersHelper helper_one(fileGuid);
        helper_one.setLayers();   
        std::vector<SQLCMD::types::ul_long> hash_s = helper_one.getHashes();//getlayerSomeThingByName< std::vector<SQLCMD::types::ul_long> >("layerHash");
        for (auto h:hash_s)
        {
        #ifdef FIRST_PROTOTYPE 
               _LOG::out_s <<  "fileGuid = " << fileGuid << "  /  h = " << h << std::endl;
              LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
        #endif
            selectMainDataSqlCmd(fileGuid,h);
        }
        #ifdef FIRST_PROTOTYPE 
            END();
        #endif	  
    }

    #define _GET_DEBUG 
    DB::my_recordSet
    IO_CRUD::
    cachedRecords::
    get(std::string fileGuid,const SQLCMD::types::ul_long layerHash)
    {
         SQLCMD::types::ul_long _LAYER_HASH_ID  = layerHash; 
        #ifdef _GET_DEBUG
            BEGIN();
            _LOG::out_s << "fileGuid = " << fileGuid << " _LAYER_HASH_ID =" << _LAYER_HASH_ID  << std::endl;
            LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
        #endif	

        DB::my_recordSet _ret  = _cache[_LAYER_HASH_ID][fileGuid];
        
        #ifdef _GET_DEBUG
          
        	for (auto s:_ret)
			{
				std::ostringstream out_s; out_s << "fieldName => " << s.first << std::endl;
				for (auto i:s.second)
					out_s << "data = " << i << std::endl;
				LOGTOFILE(LOGHTML::messageType::NOTICE,out_s);
			}
        
           // _LOG::out_s <<  "_LAYER_HASH_ID = " << _LAYER_HASH_ID << " _ret.size() = " << _ret.size() << std::endl;
           // LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
            END();           
        #endif
        return  (_ret);
    }


    #define GET_CURRENT_RECORD
    DB::my_recordSet
    IO_CRUD::
    cachedRecords::
    get_current_record(std::string fileGuid)
    {
        #ifdef GET_CURRENT_RECORD
        LOGTOFILE( LOGHTML::messageType::STRONG_WARNING,"++get_current_hash("+fileGuid+")" );
        #endif
        Layers::layersHelper _helper(fileGuid);  
        _helper.setLayers();
        SQLCMD::types::ul_long FirstId  = _helper.getFirstLayerId();
        SQLCMD::types::ul_long lastId   = _helper.getLastLayerId();
        #ifdef GET_CURRENT_RECORD
        _LOG::out_s <<  "FirstId = " << FirstId << " lastId = " << lastId << std::endl;
        LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
        #endif
        auto layerHash = _helper.getHashById(lastId);  
        #ifdef GET_CURRENT_RECORD
          _LOG::out_s <<  " layerHash = " << *layerHash << std::endl;
          LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);       
        #endif      
       
        DB::my_recordSet _sq_last  = selectMainDataSqlCmd(_helper.getFileGuid(),*layerHash);   
        std::vector<SQLCMD::types::ul_long> chk_s_last = DataSetAPI::getSomeThingByName< std::vector<SQLCMD::types::ul_long> >("chk",_sq_last);
        SQLCMD::types::ul_long nLayerId = 0 ;
        #ifdef GET_CURRENT_RECORD
          _LOG::out_s <<  "chk_s_last.size() = " << chk_s_last.size() << std::endl;
          LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
          _LOG::out_s <<  "_sq_last.size() = " << _sq_last.size() << std::endl;
          LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
        #endif
        std::vector<SQLCMD::types::ul_long> chk_s_current;
  
        for (SQLCMD::types::ul_long i = lastId ;i > FirstId ; --i)
        {

            layerHash = _helper.getHashById(i);
            #ifdef GET_CURRENT_RECORD
            _LOG::out_s <<  "layerHash = " << *layerHash << " i = " << i << std::endl;
            LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
            #endif

            DB::my_recordSet _sq  = selectMainDataSqlCmd(_helper.getFileGuid(),*layerHash);   

            chk_s_current = DataSetAPI::getSomeThingByName< std::vector<SQLCMD::types::ul_long> >("chk",_sq);
                nLayerId = i;
                
                std::vector< std::string > chk_s;
                std::transform(chk_s_last.begin(),chk_s_last.end(),chk_s_current.begin(),std::back_inserter(chk_s) ,[](SQLCMD::types::ul_long larg,SQLCMD::types::ul_long rarg)
                {
                        auto result = ( ( larg == DA::FILLERS::FILESIZE_INCREASE )||( larg == DA::FILLERS::FILESIZE_REDUCTION ) || (larg == DA::FILLERS::FILLER_FILESIZE_NO_CHANGED) )? rarg : larg;
                        return ( std::to_string(result) );
                }
            );   

        #ifdef GET_CURRENT_RECORD
            for (auto s:chk_s)
            {
                _LOG::out_s <<  "s = " << s << std::endl;
                LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
            }         
        #endif
                _sq["chk"].clear();
                _sq["chk"] = chk_s;
                return (_sq);          
        }      
        #ifdef GET_CURRENT_RECORD
        END()
        #endif
        return ( _sq_last );
    }


        #define EXIST_IN_CACHE
    bool
    IO_CRUD::
    cachedRecords::
    exist(std::string fileGuid,const SQLCMD::types::ul_long layerHash)
    {
        #ifdef EXIST_IN_CACHE
            BEGIN()
            _LOG::out_s << " layerHash = " << layerHash << " fileGuid= " << fileGuid << std::endl;
            LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
        #endif	

      
        SQLCMD::types::ul_long _LAYER_HASH_ID  = layerHash; 
        if (!layerHash)
        {
            Layers::layersHelper max_helper(fileGuid);    
            max_helper.setLayers();
            _LAYER_HASH_ID =max_helper.getLastLayerId();//getLastLayer<SQLCMD::types::ul_long>(std::string("layerHash"));
    	  //  auto layers = LAYER_QUERIES::getBaseLayer();
            #ifdef EXIST_IN_CACHE
                _LOG::out_s << "_LAYER_HASH_ID = " << _LAYER_HASH_ID <<  std::endl;
                LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
            #endif	
        }
        bool ret = (_cache.count(_LAYER_HASH_ID) > 0) ? (_cache[_LAYER_HASH_ID].count(fileGuid)>0):false;
        #ifdef EXIST_IN_CACHE
            _LOG::out_s << "EXIST fileGuid = " << fileGuid << " _LAYER_HASH_ID =" << _LAYER_HASH_ID  << "ret = " <<  ret << std::endl;
            LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
           END()
        #endif	

        return ( ret );
    }


     #define SELECT_MAIN_DATA_SQLCMD

    DB::my_recordSet
    IO_CRUD::
    cachedRecords::
    selectMainDataSqlCmd(std::string fileGuid,const SQLCMD::types::ul_long layerHash)
    {
		#ifdef SELECT_MAIN_DATA_SQLCMD
			BEGIN()
		#endif	

        const std::string CACHE_BY_LAYER_ERROR = std::string("can't retrieve information about records LayerHash Not Found!");
        const std::string CACHE_ERRROR = std::string("can't retrieve information about records!");
        const std::string QUERY_RESULT_IS_EMPTY = std::string("Query result is empty!");

    	#ifdef SELECT_MAIN_DATA_SQLCMD
            _LOG::out_s << "fileGuid = " << fileGuid << " layerHash = " << layerHash <<  std::endl;
			LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
        #endif	
    
        if (exist(fileGuid,layerHash))
        {
            #ifdef SELECT_MAIN_DATA_SQLCMD
                BEGIN();
                _LOG::out_s << "++EXIST(fileGuid = " << fileGuid << " layerHash = " << layerHash << ")" << std::endl;
                LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
                END()
            #endif	
            return (get(fileGuid,layerHash));
        }
        SQLCMD::types::ul_long _LAYER_HASH_ID  = layerHash;
    
        if (!layerHash)
        {
		    auto layers = LAYER_QUERIES::getBaseLayer(); 
            _LAYER_HASH_ID = std::stoull(layers["layerHash"][0]);
        }
    
		#ifdef SELECT_MAIN_DATA_SQLCMD
            _LOG::out_s << "_LAYER_HASH_ID = " << _LAYER_HASH_ID <<  std::endl;
			LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
        #endif
//******************************************************************/
//   Ёбаный пиздец с этим запросом!Нужен нормальный inner join/    *             
//******************************************************************/
        SQLCMD::Generator< SQLCMD::selector_type_v,
        SQLCMD::Select< SQLCMD::InnerJoinId0, SQLCMD::InnerJoinId1> >
    	selectObjChecks("blocks", { "buckets_and_blocks","layers" },
	    	SQLCMD::get_ordered(
		    	SQLCMD::selector_type{
							 {"fileGuid"     ,SQLCMD::valdesc(fileGuid, std::vector<std::string>({"buckets_and_blocks"}) , SQLCMD::valdesc::WHERE_ON)},
							 {"layerHash"       ,SQLCMD::valdesc(_LAYER_HASH_ID,std::vector<std::string>({"blocks"}), SQLCMD::valdesc::WHERE_AND,false)},
							 {"layerHash"   ,SQLCMD::valdesc(_LAYER_HASH_ID,std::vector<std::string>({"blocks"}),SQLCMD::valdesc::WHERE_AND,false) },
							 {"idx"         ,SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,std::vector<std::string>({"buckets_and_blocks"}),SQLCMD::selector_cmd_type::NONE,true) },
							 {"blockGuid"   ,SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,std::vector<std::string>({"buckets_and_blocks"}),SQLCMD::selector_cmd_type::NONE,true) },
							 {"layerGuid"   ,SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,std::vector<std::string>({"layers"}),SQLCMD::selector_cmd_type::NONE,true) },
							 {"blockHash"   ,SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,std::vector<std::string>({"blocks"}),SQLCMD::selector_cmd_type::NONE,false)},
							 {"status"      ,SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,std::vector<std::string>({"blocks"}),SQLCMD::selector_cmd_type::NONE,true) },
							 {"chk"         ,SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,std::vector<std::string>({"blocks"}),SQLCMD::selector_cmd_type::NONE,true) },
							 {"size"        ,SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,std::vector<std::string>({"blocks"}),SQLCMD::selector_cmd_type::NONE,true) }
			                }
		                )
    	);

        SQLCMD::wrapper<SQLCMD::selector_type>::add(
            {
                {"blockHash", SQLCMD::valdesc()},
                {"layerHash", SQLCMD::valdesc()}
            }
        );
        selectObjChecks.getCmdObject().linkJoin({ "blockHash","layerHash" }, { "blockHash","layerHash" });
        std::string _s = selectObjChecks();
		#ifdef SELECT_MAIN_DATA_SQLCMD
            _LOG::out_s << "_S_CMD = " << _s << std::endl;
            LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
        #endif	
        DB::SQlite  sq("db_buckets");
        sq.execute(_s);
        bool existed = exist(fileGuid , _LAYER_HASH_ID); 
        if (!existed)
        {
            if ( sq.queryResultIsEmpty() )
                throw std::runtime_error(QUERY_RESULT_IS_EMPTY);
            auto _sq = sq.getDataSet();
            
 	 #ifdef SELECT_MAIN_DATA_SQLCMD 
       		_LOG::out_s << "existed = " << existed << std::endl; 
            LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
            for (auto x:_sq)
            {
                _LOG::out_s << "x = " << x.first << std::endl; 
                LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
            }
	#endif
                _LOG::out_s << "NOT EXIST fileGuid = " << fileGuid << " _LAYER_HASH_ID =" << _LAYER_HASH_ID << " _sq.size() = " << _sq.size() << std::endl;
                LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
       //  #endif	
                _cache[_LAYER_HASH_ID][fileGuid] = _sq;
        }
        else
        {
		#ifdef SELECT_MAIN_DATA_SQLCMD
            _LOG::out_s << "EXIST fileGuid = " << fileGuid << " _LAYER_HASH_ID =" << _LAYER_HASH_ID << std::endl;
            LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
            END();  
        #endif	
            return (_cache[_LAYER_HASH_ID][fileGuid] );
        }
        
        if (_cache[_LAYER_HASH_ID][fileGuid].empty())
                throw std::runtime_error(CACHE_ERRROR);

		#ifdef SELECT_MAIN_DATA_SQLCMD
				END();
		#endif	
        return ( _cache[_LAYER_HASH_ID][fileGuid] );
    }

    IO_CRUD::cachedRecords::~cachedRecords()
    {};
    IO_CRUD::cachedRecords::cachedRecords()
    {};