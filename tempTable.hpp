#include <string>
#include <array>
#include "sql/sqlcopy.hpp"
#include "layersHelper.hpp"

namespace TempObjects
{



//template<typename sqlCacheType>	
//void createTempTable(sqlCacheType& _sqlCache,std::string fileGuid ,SQLCMD::types::ul_long prevMaxlayerHash,SQLCMD::types::ul_long newMaxLayerHash);
//std::array<SQLCMD::types::ul_long,2> layerHashes(std::string fileGuid);



struct autoTempCreator
{
	enum{PREV_LAYER_HASH = 0,LAST_LAYER_HASH,LENGHT};
	std::array<SQLCMD::types::ul_long,2> array;
	std::string _fileGuid;
	autoTempCreator(std::string fileGuid):_fileGuid(fileGuid)
	{	
		array = layerHashes(fileGuid);
	}

	template
	<
		typename sqlType
	>
	void operator()(sqlType& sqlCache)
	{
		createTempTable(sqlCache,_fileGuid,array[PREV_LAYER_HASH],array[LAST_LAYER_HASH]);
	}


	#define LAYERS_HASHES
	std::array<SQLCMD::types::ul_long,2>
	layerHashes(std::string fileGuid)
	{
	#ifdef LAYERS_HASHES
	 	BEGIN()
	#endif
		std::array<SQLCMD::types::ul_long,2> ret;
		LayersIO::layersHelper _detectMaximalIdTotal;
		LayersIO::layersHelper _detectMaximalIdByFileGuid(fileGuid);
		_detectMaximalIdByFileGuid.setLayers();

		SQLCMD::types::ul_long lastLayerIdByFileGuid = _detectMaximalIdByFileGuid.getLastLayer< SQLCMD::types::ul_long >("layerId");
		SQLCMD::types::ul_long lastLayerIdTotal = _detectMaximalIdTotal.getLastLayer< SQLCMD::types::ul_long >("layerId");
	#ifdef LAYERS_HASHES
			_LOG::out_s << "fileGuid = " << fileGuid << " lastLayerIdByFileGuid = " <<  lastLayerIdByFileGuid  << " lastLayerIdTotal = " << lastLayerIdTotal << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
	#endif
	
		if (lastLayerIdByFileGuid < lastLayerIdTotal)
		{

			std::optional<SQLCMD::types::ul_long> prevLayerLayerHash = 
				_detectMaximalIdByFileGuid.getlayerSomeThingRelationNameByName<SQLCMD::types::ul_long,SQLCMD::types::ul_long>("layerHash","layerId",lastLayerIdByFileGuid);

			std::optional<SQLCMD::types::ul_long> nextLayerLayerHash = 
				_detectMaximalIdTotal.getlayerSomeThingRelationNameByName<SQLCMD::types::ul_long,SQLCMD::types::ul_long>("layerHash","layerId",++lastLayerIdByFileGuid);
		

	
		#ifdef LAYERS_HASHES
			_LOG::out_s << "Layer Not Created = " << " prevLayerLayerHash =" << ((prevLayerLayerHash)? *prevLayerLayerHash : 101)  << " nextLayerLayerHash = " <<  ((nextLayerLayerHash)? *nextLayerLayerHash : 101)
			  << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
		#endif

			ret[PREV_LAYER_HASH] = *prevLayerLayerHash;
			ret[LAST_LAYER_HASH] = *nextLayerLayerHash;
		}
		else
		{
			std::optional<SQLCMD::types::ul_long> prevLayerLayerHash = 
				_detectMaximalIdByFileGuid.getlayerSomeThingRelationNameByName<SQLCMD::types::ul_long,SQLCMD::types::ul_long>("layerHash","layerId",lastLayerIdByFileGuid);
			SQLCMD::types::ul_long new_layer_hash = utils::minicache::str_hash( _detectMaximalIdTotal.createNewLayer() ) / 100000;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"NEW_LAYER_CREATED!!!");
	#ifdef LAYERS_HASHES
			_LOG::out_s << "new layerCreated = " << new_layer_hash << " lastLayerIdByFileGuid = " <<  lastLayerIdByFileGuid  << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
	#endif

			ret[PREV_LAYER_HASH] = *prevLayerLayerHash;
			ret[LAST_LAYER_HASH] = new_layer_hash;
		}		
	#ifdef LAYERS_HASHES
	 	END()
	#endif
		return (ret);
	}

std::string update()
{
	std::string mainTable = "_tmp";
	std::string joinedTable = "test_table";

	SQLCMD::selector_type _m = {
								{ "blockHash"	, SQLCMD::valdesc("blockHash",std::vector<std::string>({joinedTable}),SQLCMD::valdesc::__FILLER__,false)},
								{ "status"     	, SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,SQLCMD::selector_cmd_type::NONE,true)},
	};
	SQLCMD::Generator < SQLCMD::selector_type_v, SQLCMD::Select<> > sub_select_0(mainTable, SQLCMD::get_ordered(_m));
	auto _subSelect_0 = sub_select_0();


	_m = {
										{ "blockHash"	, SQLCMD::valdesc("blockHash",std::vector<std::string>({joinedTable}),SQLCMD::valdesc::__FILLER__,false)},
										{ "status"     	, SQLCMD::valdesc(1,std::vector<std::string>({joinedTable}),SQLCMD::valdesc::__AS__,true)},
	};
	SQLCMD::Generator < SQLCMD::selector_type_v, SQLCMD::Select<> > sub_select_1(mainTable, SQLCMD::get_ordered(_m));
	auto _subSelect_1 = sub_select_1();


	SQLCMD::multi_selector_type m_update =
	{
			{ "status"  , SQLCMD::valdesc(_subSelect_0,SQLCMD::valdesc::__FILLER__,true)},
			{ "status"  , SQLCMD::valdesc(_subSelect_1,SQLCMD::valdesc::WHERE_EXISTS,true)}
	};
	SQLCMD::Generator <  SQLCMD::selector_type_v, SQLCMD::Update > update("test_table", SQLCMD::get_ordered(m_update));
	return update();
}

std::string dropIfExists(std::string tableName)
{
	SQLCMD::Generator<std::string, SQLCMD::DropTable > drop(tableName, "");
	
	return drop();
}

#define CREATE_SUBTEMP_TABLE_LOG_ON
std::string createSubTemp(SQLCMD::types::ul_long LAYER_HASH_ID,SQLCMD::types::ul_long status_main,SQLCMD::types::ul_long status_join)
{
	#ifdef CREATE_SUBTEMP_TABLE_LOG_ON
		BEGIN()
	#endif

	std::string mainTable = "blocks";
	std::string joinedTable = "test_table";
	SQLCMD::multi_selector_type m = {
									{ "chk" 	, SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,SQLCMD::selector_cmd_type::NONE,true)},
									{ "blockHash"	, SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,std::vector<std::string>({joinedTable}),SQLCMD::selector_cmd_type::NONE,true)},
									{ "status"     	, SQLCMD::valdesc(status_main,std::vector<std::string>({mainTable}),SQLCMD::valdesc::WHERE_AND,true)},
									{ "status"     	, SQLCMD::valdesc(status_join,std::vector<std::string>({joinedTable}),SQLCMD::valdesc::WHERE_AND,false)},
									{ "layerHash"   , SQLCMD::valdesc(LAYER_HASH_ID ,std::vector<std::string>({mainTable}) 	,SQLCMD::valdesc::WHERE_ON,false)}///,
								//	{ "layerHash"   , SQLCMD::valdesc(22222 ,std::vector<std::string>({joinedTable}) 	,SQLCMD::valdesc::__AS__,false)},
	};
	SQLCMD::Generator < SQLCMD::selector_type_v, SQLCMD::Select< SQLCMD::InnerJoinId0 > > main_select(mainTable, { joinedTable }, SQLCMD::get_ordered(m));
	SQLCMD::wrapper<SQLCMD::selector_type>::add(
		{
			{"blockHash", SQLCMD::valdesc()}
		}
	);
	main_select.getCmdObject().linkJoin({ "blockHash" }, { "blockHash" });
	std::string ss = main_select();
	SQLCMD::selector_type m_create =
	{
				{ "tmp", SQLCMD::valdesc(std::string(ss),SQLCMD::valdesc::AS_TABLE)},
				{ "blockHash", SQLCMD::valdesc(1112,SQLCMD::valdesc::WHERE_ON)}
	};

	SQLCMD::Generator <  SQLCMD::selector_type_v, SQLCMD::Create<false, true> > create("_tmp", SQLCMD::type_v(m_create));
	std::string _s = create();
	#ifdef CREATE_SUBTEMP_TABLE_LOG_ON
		_LOG::out_s << "table already created _s = "<< _s <<  std::endl;	
		LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
		END()	
	#endif

	return (_s);
}	


std::string tableCopy(void)
{
	SQLCMD::selector_type m_copy =
	{
				{ "status"     , SQLCMD::valdesc()    },
				{ "chk"        , SQLCMD::valdesc()    },
				{ "blockHash"  , SQLCMD::valdesc()    },
				{ "layerHash"  , SQLCMD::valdesc()    },
				{ "size"       , SQLCMD::valdesc()    }

	};
	return (SQLCMD::SqlCopy("test_table", "blocks", m_copy));
}

/*
	Временная таблица
CREATE TABLE if not exists test_table AS 
	SELECT blocks.chk,blocks.status,blocks.blockHash,blocks.size,blocks.layerHash 
	FROM buckets_and_blocks inner join blocks on buckets_and_blocks.blockHash = blocks.blockHash 
where buckets_and_blocks.fileGuid like '7145a6f1-8203-4016-86ea-3fa00944534f' AND blocks.layerHash = 55799843279091
*/
	template
	<
		typename sqlCacheType
	>
	void createTempTable(sqlCacheType& _sqlCache,std::string fileGuid ,SQLCMD::types::ul_long prevMaxlayerHash,SQLCMD::types::ul_long newMaxLayerHash)
	{

		#ifdef CREATE_TEMP_TABLE_LOG_ON
			_LOG::out_s << " ++createTempTable  fileGuid = "<< fileGuid << " prevMaxlayerHash = " 
			<< prevMaxlayerHash << " newMaxLayerHash = " << newMaxLayerHash << std::endl;	
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
		#endif
	#define CREATE_TABLE_FUNCTION_DEBUG 
	#ifdef CREATE_TABLE_FUNCTION_DEBUG


		const std::string joinedTable 	= "blocks";
		const std::string mainTable 	= "buckets_and_blocks";
		#define SET_VALDESC_DEFAULT(x) SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE, std::vector<std::string>({ x }), SQLCMD::selector_cmd_type::NONE, true)
		SQLCMD::multi_selector_type m_select = {
			{ "chk", 		SET_VALDESC_DEFAULT(joinedTable)},
			{ "layerHash", SQLCMD::valdesc(prevMaxlayerHash, std::vector<std::string>({joinedTable}),SQLCMD::valdesc::WHERE_AND,false)},
			{ "layerHash", SQLCMD::valdesc(newMaxLayerHash,  std::vector<std::string>({joinedTable}),SQLCMD::valdesc::__AS__,true)},
			/*SET_VALDESC_DEFAULT("blocks")*/
			{ "status"	 , SQLCMD::valdesc("2004",			 std::vector<std::string>({joinedTable}),SQLCMD::valdesc::__AS__,true)	},
			{ "blockHash", SET_VALDESC_DEFAULT(joinedTable)},
			{ "fileGuid",  SQLCMD::valdesc(fileGuid,SQLCMD::valdesc::WHERE_ON,false)},
			{ "size", 	   SET_VALDESC_DEFAULT(joinedTable)}

		};
		SQLCMD::Generator < SQLCMD::selector_type_v, SQLCMD::Select< SQLCMD::InnerJoinId0>  > _select(mainTable,
			{ joinedTable }, SQLCMD::get_ordered(m_select));
		SQLCMD::wrapper<SQLCMD::selector_type>::add(
			{
				{"blockHash", SQLCMD::valdesc()}
			}
		);
		_select.getCmdObject().linkJoin({ "blockHash" }, { "blockHash" });
	//	_select.getCmdObject().setDebug();
		std::string select = _select();	
		#ifdef CREATE_TEMP_TABLE_LOG_ON
			_LOG::out_s << " select = "<< select << std::endl;	
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
		#endif
		
		std::string select_result = std::string(" ")+select;

		SQLCMD::selector_type m_create = 
			{
						{ "tmp", SQLCMD::valdesc(std::string(select_result),SQLCMD::valdesc::AS_TABLE)},
						{ "fileGuid", SQLCMD::valdesc(fileGuid,SQLCMD::valdesc::WHERE_ON)}                           
			};


		SQLCMD::Generator <  SQLCMD::selector_type_v, SQLCMD::Create<false,false> > create("test_table", SQLCMD::type_v(m_create));
		std::string _s = create();

		#ifdef CREATE_TEMP_TABLE_LOG_ON
			_LOG::out_s << " _s= "<<  _s << std::endl;	
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
		#endif
		if (_sqlCache.isExist(_s))
		{

		#ifdef CREATE_TEMP_TABLE_LOG_ON
			_LOG::out_s << "table already created _s = "<< _s <<  std::endl;	
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
		
		#endif
			return;
		}


		_sqlCache.add(_s);

		#ifdef CREATE_TEMP_TABLE_LOG_ON
			_LOG::out_s << " _s(createTable) = "<< _s <<  std::endl;	
			std::cout << _LOG::out_s.str() << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
		#endif
		_s = dropIfExists("_tmp");
		#ifdef CREATE_TEMP_TABLE_LOG_ON
			_LOG::out_s << " _s(dropIfExists) = "<< _s <<  std::endl;	
			std::cout << _LOG::out_s.str() << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
		#endif
		_sqlCache.add(_s);

		_s = createSubTemp(prevMaxlayerHash,2002,2004);

		#ifdef CREATE_TEMP_TABLE_LOG_ON
			_LOG::out_s << " _s(createSubTable) = "<< _s <<  std::endl;	
			std::cout << _LOG::out_s.str() << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
		#endif
		_sqlCache.add(_s);
		_s = update();
		#ifdef CREATE_TEMP_TABLE_LOG_ON
			_LOG::out_s << " _s(update) = "<< _s <<  std::endl;	
			std::cout << "press any key for:" << _LOG::out_s.str() << std::endl;
			getc(stdin);
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
		#endif
		_sqlCache.add(_s);
		//_s = tableCopy();		
		#ifdef CREATE_TEMP_TABLE_LOG_ON
			_LOG::out_s << " _s(tableCopy) = "<< _s <<  std::endl;	
			std::cout << _LOG::out_s.str() << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
		#endif
		//_sqlCache.add(_s);
		#endif
	}
};
   /* template
    <
     typename   sqlCache
    >
    tryToCreateTemp(sqlCache& sql,std::string fileGuid)
    {
        layersHelper  layersHelper;
				 
        LayersIO::layerBuGuidOptional lg = LayersIO::getLayersByBlockGuid(LayersIO::recordSetToQueryInfo())
        createTempTable(sql,fileGuid,)
    }
*/

}