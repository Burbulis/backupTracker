#ifndef _DB_QRY
#define _DB_QRY

#include "sql/SqClass.h"
#include "utils.hpp"
//#include "DA_.hpp"
//#include "sql/sqlcmdType.hpp"
#include "sql/insert.hpp"
#include "sql/update.hpp"
#include "sql/create.hpp"
#include "sql/sqlGenerator.hpp"
#include "sql/sqlcopy.hpp"
#include "log/_log.hpp"
#include "layers/layers_API.hpp"
#include "uid_and_chk.hpp"


namespace LAYER_QUERIES
{
	static void								createBaseLayer(void);
	static DB::my_recordSet  				getBaseLayer(void);
	DB::my_recordSet 						getfilesAndPaths(void);



	#define GET_FILES_AND_PATHS
	inline
	DB::my_recordSet
	getfilesAndPaths(void)
	{
		#ifdef GET_FILES_AND_PATHS
			BEGIN()
		#endif
		SQLCMD::Generator< SQLCMD::selector_type_v,
        SQLCMD::Select<> >
        sys_select("system_ids", 
            SQLCMD::get_ordered(
				SQLCMD::selector_type   {
                        {"pathFile"    ,SQLCMD::valdesc()},
                        {"fileName"    ,SQLCMD::valdesc()},
                        {"fileGuid"    ,SQLCMD::valdesc()},
                	}
                )
        );
		std::string s = sys_select();
		#ifdef GET_FILES_AND_PATHS
		{
			std::ostringstream out_s; out_s << "s = " <<  s  << std::endl;
			LOGTOFILE(LOGHTML::messageType::NOTICE,out_s);
		}
		#endif
		DB::SQlite  sqlPerform("db_buckets");	
		sqlPerform.execute(s);
		DB::my_recordSet rs = sqlPerform.getDataSet();
		#ifdef GET_FILES_AND_PATHS
		{
			std::ostringstream out_s; out_s << "rs.size() = " << rs.size()  << std::endl;
			LOGTOFILE(LOGHTML::messageType::NOTICE,out_s);

			for (auto r:rs)
			{
				out_s << "r.first = " <<  r.first  << std::endl;
				LOGTOFILE(LOGHTML::messageType::NOTICE,out_s);
			}
		}
			END()	
		#endif
		return (rs);
	}

	#define CREATE_BASE_LAYER_LOG_ON
    /// @brief если базового слоя ещё нет,создать его!
    /// @param  без параметров
    static
	void
	createBaseLayer(void)
	{
	#ifdef CREATE_BASE_LAYER_LOG_ON
		LOGTOFILE(LOGHTML::messageType::WARNING , "++CREATE BASE_LAYER" );
		//LOG("++CREATE BASE_LAYER");
	#endif
		//using namespace SQLCMD;
        std::string Uid = uid_sAndId_s::Newuid();
		SQLCMD::types::ul_long layerHash = utils::minicache::str_hash( Uid ) / 100000 ;
		
	    SQLCMD::inserter_type m ={
			{ "layerGuid",SQLCMD::valdesc(Uid) }, 
			{ "layerHash",SQLCMD::valdesc(layerHash) }, 
			{ "layer_start",SQLCMD::valdesc(SQLCMD::types::timeDef())}, 
			{ "layerId",SQLCMD::valdesc(SQLCMD::types::ul_long(1),SQLCMD::valdesc::WHERE_ON)}
		};
	    SQLCMD::Generator< SQLCMD::inserter_type, SQLCMD::InsertIfNotExist> layers(std::string("layers"),m);
	
		
			std::string layers_ = layers();	
	#ifdef CREATE_BASE_LAYER_LOG_ON	
			std::ostringstream out_s; out_s << "layers = " << layers_ << std::endl;
			LOGTOFILE(LOGHTML::messageType::NOTICE,out_s);
	#endif
      	DB::SQlite  sqlPerform("db_buckets");	
		sqlPerform.prepare(layers_);
		sqlPerform.execute();
	
	#ifdef CREATE_BASE_LAYER_LOG_ON
	        LOGTOFILE(LOGHTML::messageType::WARNING,"--CREATE BASE_LAYER");
	#endif
	}

	#define GET_BASE_LAYER_LOG_ON
   /// @brief Возвращает атрибуты базового слоя. (layerGuid,layerId,layerHash)
   /// @param  -//-
   /// @return  DB::my_recordSet с одним единственным элементом. У которого минимальны layerId , 
   //соответственно он и есть описатель базового слоя. 
   static
   DB::my_recordSet
   getBaseLayer(void)
   {
		#ifdef  GET_BASE_LAYER_LOG_ON
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"++getBaseLayer()");
		#endif
		DB::my_recordSet ret;
		SQLCMD::selector_type m = { 
						{ "layerGuid", SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,SQLCMD::selector_cmd_type::NONE,true)},
						{ "layerId"  , SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,SQLCMD::selector_cmd_type::NONE,true)},
						{ "layerHash"   , SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,SQLCMD::selector_cmd_type::NONE,true)}
						};
	
		m["layerId"].setAsPredicate(SQLCMD::valdesc::MIN);
		SQLCMD::Generator<
			SQLCMD::selector_type_v,
			SQLCMD::Select<>
		>selectObjChecks( "layers", SQLCMD::get_ordered(m));
		
		DB::SQlite  sq("db_buckets");
		try
		{
			sq.execute(selectObjChecks());
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			return (sq.getDataSet());
		}
		SQLCMD::valdesc::clearInstanceCounter();
		ret = sq.getDataSet(); 
		#ifdef  GET_BASE_LAYER_LOG_ON
			std::ostringstream out_s; out_s << "ret.second.size() = " << ret.size() << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,out_s);
		#endif
		return (ret);
   }

}//LAYER_QUERIES

#endif