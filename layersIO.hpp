#include <optional>
#include "SqClass.h"
#include "sql/sqlcmdType.hpp"
#include "layersHelper.hpp"
#include "layers_API.hpp"
#ifndef __LAYERS_IO__
#define __LAYERS_IO__

namespace LayersIO
{
    
  	using layerByGuid = std::map<std::string,size_t>;
	using queryInfo_seq = std::vector<queryInfo>;
    using layerBuGuidOptional = std::optional<layerByGuid>;
  
	
	static LayersIO::queryInfo_seq  recordSetToQueryInfo(const DB::my_recordSet& selected);
	static layerBuGuidOptional		getLayersByBlockGuid(const queryInfo_seq& queryInfo);





	/// @brief для заданных блоков во входящем параметре , ищет в базе максимальный layerId
	/// @param queryInfo 
	/// @return typedef std::map<std::string,size_t> layerByGuid 
	///для заданных блоков , возвращает соответствующие им, максимальные layerId.
	LayersIO::layerBuGuidOptional 
	getLayersByBlockGuid(const queryInfo_seq& queryInfo)
	{
		layersHelper helper;
	
		layerByGuid lbg;
		if (queryInfo.empty())
			return (std::nullopt);
	
		for (auto qi:queryInfo)   
		lbg[qi.blockGuid] = 0;

		for (auto qi:queryInfo)
		{	
			std::optional<SQLCMD::types::ul_long> layerId = 
				helper.getlayerSomeThingRelationNameByName<SQLCMD::types::ul_long,SQLCMD::types::ul_long>
					("layerId","layerHash",qi.layerHash);
			
			if ((layerId) && (lbg[qi.blockGuid] <= *layerId))
				lbg[qi.blockGuid] = *layerId;
		}
		return (lbg);
	}


	/// @brief данные переданные в качестве аргумента преобразуются в последовательность из объектов типа queryInfo.
	/// @param selected 
	/// @return последовательность queryInfo
	static LayersIO::queryInfo_seq  recordSetToQueryInfo(const DB::my_recordSet& selected)
	{
		std::vector<std::string> blocks 			     = DataSetAPI::getSomeThingByName< std::vector<std::string> >(std::string("blockGuid"),selected);
		std::vector<SQLCMD::types::ul_long> idx_sUll     = DataSetAPI::getSomeThingByName< std::vector<SQLCMD::types::ul_long> >(std::string("idx"),selected);
		std::vector<SQLCMD::types::ul_long> layerHashUll = DataSetAPI::getSomeThingByName< std::vector<SQLCMD::types::ul_long> >(std::string("layerHash") ,selected);       
		if ((blocks.size()!=idx_sUll.size())||(blocks.size()!=layerHashUll.size()))
			throw std::runtime_error("uncknown reason size mismatch!");
		queryInfo_seq qi_s;
		for (size_t i = 0; i < blocks.size();++i)
			qi_s.push_back(queryInfo(layerHashUll[i],idx_sUll[i],blocks[i]));
		return (qi_s);
	}
}

static LayersIO::queryInfo_seq LayersIO::recordSetToQueryInfo(const DB::my_recordSet& selected);
static LayersIO::layerBuGuidOptional LayersIO::getLayersByBlockGuid(const queryInfo_seq& queryInfo);


#endif