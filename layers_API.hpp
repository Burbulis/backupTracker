#ifndef __LAYERS_API__
#define __LAYERS_API__

#include <type_traits>
#include <string>
#include <optional>
#include "SqClass.h"
#include "uid_and_chk.hpp"
#include "datasetAPI.hpp"
namespace layersAPI
{


	static  DB::my_recordSet  getAllLayers(void);
	static	std::optional<DB::my_recordSet>	getLayersByFileGuid(std::string fileGuid);
	static std::string	createNewLayer(std::string Uid = uid_sAndId_s::Newuid());

	static bool compare(std::string lval,std::string rval);
	template<typename T> static	bool compare(const T& lt,const T& rt);
	static	DB::my_recordSet checkedHashbyLayers(const std::vector<SQLCMD::valdesc>& layerHashesForCheck);


	#define GET_ALL_LAYERS_LOG_ON	
	static  DB::my_recordSet  getAllLayers(void)
	{
		#ifdef  GET_ALL_LAYERS_LOG_ON
			BEGIN()
		#endif
		DB::my_recordSet ret;
		SQLCMD::selector_type m = { 
						{"layerGuid", SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,SQLCMD::selector_cmd_type::NONE,true)},
						{"layerId", SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,SQLCMD::selector_cmd_type::NONE,true)},
						{"layerHash", SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,SQLCMD::selector_cmd_type::NONE,true)}
						};

		SQLCMD::Generator<
			SQLCMD::selector_type_v,
			SQLCMD::Select<>
		>selectObjChecks( "layers", SQLCMD::get_ordered(m));
		DB::SQlite  sq("db_buckets");
		try
		{
			std::string s = selectObjChecks();
		#ifdef _DEBUG_UPDATE_RECORDS_ON_
			_LOG::out_s << " SQL:: " << s << std::endl;
			LOGTOFILE(LOGHTML::messageType::NOTICE,_LOG::out_s);
		#endif

			sq.execute(s);
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			return (ret);
		}
		SQLCMD::valdesc::clearInstanceCounter();
		ret = sq.getDataSet(); 
		#ifdef  GET_ALL_LAYERS_LOG_ON
			_LOG::out_s << "ret.second.size() = " << ret.size() << std::endl;
			LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
			END();
		#endif
		return (ret);
	}



	#define CREATE_NEW_LAYER_LOG_ON
	static	std::string	createNewLayer(std::string Uid)
	{
		#ifdef  CREATE_NEW_LAYER_LOG_ON
			BEGIN()
		#endif
        DB::SQlite  sqlPerform("db_buckets");
        
		SQLCMD::types::ul_long layerHash = utils::minicache::str_hash( Uid ) / 100000 ;

	    SQLCMD::Generator< SQLCMD::inserter_type, SQLCMD::InsertIfNotExist> layers(std::string("layers"),
        {
        	{ "layerGuid",SQLCMD::valdesc(Uid) }, { "layerHash",SQLCMD::valdesc(layerHash) },{ "layer_start",SQLCMD::valdesc(SQLCMD::types::timeDef())} ,{ "##layerGuid",SQLCMD::valdesc(std::string(Uid),SQLCMD::valdesc::WHERE_ON,false)}
        });
		std::string s = layers();

		#ifdef  CREATE_NEW_LAYER_LOG_ON
			_LOG::out_s << "s = " << s << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
		#endif

		sqlPerform.execute(s);

		#ifdef  CREATE_NEW_LAYER_LOG_ON
			END();
		#endif	

		return (Uid);
	}

	#define GET_LAYERS_BY_FILE_GUID_LOG_ON
	static 
	std::optional<DB::my_recordSet>				
	getLayersByFileGuid(std::string fileGuid)
	{
		#ifdef GET_LAYERS_BY_FILE_GUID_LOG_ON
			BEGIN()
		#endif

		std::string selectQueryStr="";
   		DB::SQlite  sqlPerform("db_buckets");	
		{
			SQLCMD::selector_type m{
				{ "fileGuid", SQLCMD::valdesc(fileGuid, std::vector<std::string>({ "buckets_and_blocks" }), SQLCMD::valdesc::WHERE_ON, false) },
				{ "layerHash"   ,SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,std::vector<std::string>({"blocks"}),SQLCMD::selector_cmd_type::NONE,true) },
				{ "blockHash"   ,SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,std::vector<std::string>({"blocks"}),SQLCMD::selector_cmd_type::NONE,false) },
			};
			m["layerHash"].setAsPredicate(SQLCMD::valdesc::DISTINCT);
			SQLCMD::Generator< SQLCMD::selector_type_v,
				SQLCMD::Select< SQLCMD::InnerJoinId0> >
				_select("buckets_and_blocks", { "blocks" }, SQLCMD::get_ordered(m));


			SQLCMD::wrapper<SQLCMD::selector_type>::add(
				{
					{"blockHash", SQLCMD::valdesc() },
					{"blockHash", SQLCMD::valdesc()}
				}
			);

			_select.getCmdObject().linkJoin({ "blockHash" }, { "blockHash" });
			selectQueryStr = _select();
		}
		try
		{
			//sqlPerform.setDebug();	
			#ifdef GET_LAYERS_BY_FILE_GUID_LOG_ON
				_LOG::out_s <<  "selectQueryStr = " << selectQueryStr  << std::endl;
				LOGTOFILE(LOGHTML::messageType::NOTICE,_LOG::out_s);
			#endif		
			sqlPerform.execute(selectQueryStr);
	
		}
		catch(const std::exception& e)
		{
		#ifdef GET_LAYERS_BY_FILE_GUID_LOG_ON
				_LOG::out_s << "Exception e.what() = " <<  e.what()  << std::endl;
				LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
		#endif
			return (std::nullopt);
		}
		if ( sqlPerform.queryResultIsEmpty() )
		{
			#ifdef GET_LAYERS_BY_FILE_GUID_LOG_ON
				LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"QUERY IS EMPTY!!!!");
			#endif			
			return (std::nullopt);
		}
		auto recordSet = sqlPerform.getDataSet();	
		#ifdef GET_LAYERS_BY_FILE_GUID_LOG_ON
				_LOG::out_s << "recordSet.size() = " <<  recordSet.size()  << std::endl;
				LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
		#endif


		std::vector<SQLCMD::types::ul_long> layerHashes =DataSetAPI::getSomeThingByName< std::vector<SQLCMD::types::ul_long> >("layerHash",recordSet);
		std::vector<SQLCMD::valdesc> inCollection;
		std::transform(layerHashes.begin(),layerHashes.end(),std::back_inserter(inCollection),[](SQLCMD::types::ul_long arg){
			return (SQLCMD::valdesc(arg));
		});
		#ifdef GET_LAYERS_BY_FILE_GUID_LOG_ON
			END()
		#endif
		return (checkedHashbyLayers(inCollection));
	}



	/// @brief 	//scope subblock for check actual layers selected from buckets in layer table.	
	/// @param layerHashesForCheck //layerHash's for checking 
	/// @return dataset .
	static
	DB::my_recordSet
	checkedHashbyLayers(const std::vector<SQLCMD::valdesc>& layerHashesForCheck)
	{
		SQLCMD::selector_type m_layers = 
		{
			{"layerId",   SQLCMD::valdesc()},
			{ "layerGuid", SQLCMD::valdesc()},
			{ "layerHash", SQLCMD::valdesc(0,SQLCMD::valdesc::WHERE_IN)}
		};
		SQLCMD::Generator < SQLCMD::selector_type_v, SQLCMD::Select<> > selectFromLayers("layers", SQLCMD::get_ordered(m_layers));
		selectFromLayers.getCmdObject().addInParam(layerHashesForCheck);
		std::string sq_layer = selectFromLayers();	
		DB::SQlite  _sqlPerform("db_buckets");	
		try
		{
		#ifdef CHECKED_HASH_BY_LAYERS_LOG_ON
			_LOG::out_s << "sq_layer = " << sq_layer << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);	
		#endif	
			_sqlPerform.execute(sq_layer);
		}
		catch(const std::exception& e)
		{
			_LOG::out_s << "Exception e.what() = " <<  e.what()  << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
			return (DB::EMPTY());
		}	
		auto _fields = _sqlPerform.getFieldNames();
		DB::my_recordSet _recordSet = _sqlPerform.getDataSet();
		#ifdef CHECKED_HASH_BY_LAYERS_LOG_ON
			END();
		#endif
		return (_recordSet);
	}
}
#endif