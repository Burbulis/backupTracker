#include <optional>
#include "../sql/SqClass.h"
#include "../sql/sqlcmdType.hpp"
#include "layersHelper.h"
#include "layers_API.hpp"
#ifndef __LAYERS_IO__
#define __LAYERS_IO__

namespace LayersIO
{
    
  	//using layerByGuid = std::map<std::string,size_t>;
	//using queryInfo_seq = std::vector<queryInfo>;
    //using layerBuGuidOptional = std::optional<layerByGuid>;
  
	
	static std::optional<SQLCMD::types::ul_long> getFileSizeByGuid(std::string fileGuid);


static
	std::optional<SQLCMD::types::ul_long>
	getFileSizeByGuid(std::string fileGuid)
	{
		/*"layerId","layerHash"}*/
		#ifdef GET_FILE_SIZE_BY_GUID_LOG_ON
			BEGIN()
		#endif

		//LayersIO::layersHelper _helper(fileGuid);
		Layers::layersHelper _helper(fileGuid);
		_helper.setLayers();
		if (_helper.Empty())
		{
			_LOG::out_s << "getFileSizeByGuid-error fileGuid = " <<  fileGuid << "not found!" << std::endl;
			throw std::runtime_error( _LOG::out_s.str() );			
		}
		const SQLCMD::types::ul_long LAYER_HASH_ID = _helper.getLastLayerId();//getLastLayer<SQLCMD::types::ul_long>(std::string("layerHash"));
		SQLCMD::selector_type m = {
									{ "fileGuid" 	, SQLCMD::valdesc(fileGuid,SQLCMD::valdesc::WHERE_ON,false)},
									{ "blockHash"	, SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,std::vector<std::string>({"blocks"}),SQLCMD::selector_cmd_type::NONE,false)},
									{ "size"     	, SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,std::vector<std::string>({"blocks"}),SQLCMD::selector_cmd_type::NONE,true)},
									{ "layerHash"   , SQLCMD::valdesc(LAYER_HASH_ID ,std::vector<std::string>({"blocks"}) 	,SQLCMD::valdesc::WHERE_AND,false)},
			};
		m["size"].setAsPredicate(SQLCMD::valdesc::SUM);
		SQLCMD::Generator < SQLCMD::selector_type_v, SQLCMD::Select< SQLCMD::InnerJoinId0 > > _select("buckets_and_blocks", { "blocks" }, SQLCMD::get_ordered(m));
		SQLCMD::wrapper<SQLCMD::selector_type>::add(
			{
				{"blockHash", SQLCMD::valdesc()}
			}
		);
		_select.getCmdObject().linkJoin({ "blockHash" }, { "blockHash" });
		auto s = _select();
		DB::SQlite  sqlPerform("db_buckets");		
		#ifdef GET_FILE_SIZE_BY_GUID_LOG_ON
			_LOG::out_s << "s = " <<  s  << std::endl;
			LOGTOFILE(LOGHTML::messageType::NOTICE,_LOG::out_s);
		#endif
		try{
		sqlPerform.execute(s);
		}
		catch(const std::exception& e)
		{
			_LOG::out_s << "Exception e.what() = " <<  e.what()  << std::endl;
			getc(stdin);
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);	
		
			return (std::nullopt);
		}
		DB::my_recordSet rs = sqlPerform.getDataSet();
		if (DB::IsEmpty(rs))
			throw std::runtime_error("file size calculation error!");

		#ifdef GET_FILE_SIZE_BY_GUID_LOG_ON
			_LOG::out_s << "rs.size() = " <<  rs.size()  << std::endl;
			for (auto i:rs)
			{
				_LOG::out_s << "rs.first = " <<  i.first << "rs.second.size() = " << i.second.size() << std::endl;
				LOGTOFILE(LOGHTML::messageType::NOTICE,_LOG::out_s);
			}
		#endif
		SQLCMD::types::ul_long size = 0;
		try
		{
			size = DataSetAPI::getSomeThingByName< std::vector< SQLCMD::types::ul_long> >("size",rs)[0];
			#ifdef GET_FILE_SIZE_BY_GUID_LOG_ON
				_LOG::out_s << "size = " <<  size << std::endl;
				LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);			
			#endif
		}
		catch(const std::exception& e)
		{
			#ifdef GET_FILE_SIZE_BY_GUID_LOG_ON
				_LOG::out_s << "Exception catched = " << e.what() << std::endl;
				LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
			#endif
		}
		#ifdef GET_FILE_SIZE_BY_GUID_LOG_ON
			END()
		#endif
		return ( size );
	}
}

#endif