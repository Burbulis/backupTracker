
#include "layersHelper.hpp"

#ifndef _BLOCKS_AND_LAYERS_
#define _BLOCKS_AND_LAYERS_

#include "layersIO.hpp"

#define GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON

namespace layersAndBlocks
{
	using blockSerialiseOn = std::pair<std::string , bool >;
    using hashAndBlocks = std::map< SQLCMD::types::ul_long, std::vector <blockSerialiseOn> >;
	
    template
	<
	typename bufferType,
	typename sqlCache
	>
	static
	std::optional<hashAndBlocks>	getBlocksWithLayersById(const bufferType & faBuffer,sqlCache& sql);
    static std::optional<SQLCMD::types::ul_long> getFileSizeByGuid(std::string fileGuid);
	static 	DB::my_recordSet getLayersByFileGuid(std::string fileGuid);
	std::optional<DB::my_recordSet>	_getCheckedBlocks(const std::vector<SQLCMD::valdesc>& collect_for_check,std::string fileGuid);
 	DB::my_recordSet  		  getInfoAboutBlocks(const std::vector<std::string>& block_s);
	static std::optional<std::vector< std::string > >	checkChangedBlocks(const std::vector< blockSerialiseOn >& checkedBlocks_);
	static  DB::my_recordSet  getAllLayers(void);
	static	std::optional<hashAndBlocks>  getBlocksWithLayers(LayersIO::layersHelper &layersHelper, LayersIO::layerBuGuidOptional layerByBlockGuid,std::vector< blockSerialiseOn >  checkedBlocks);
	template  < 
		typename bufferType 
	> std::vector< blockSerialiseOn >	_checkChangedBlocks( const	bufferType& faBuffer);



	static
	std::optional<std::vector< std::string > >
	checkChangedBlocks(const std::vector< blockSerialiseOn >& checkedBlocks_)
	{
	//1)Находим блоки которые изменились, устанавливаем попутно флаг blockSerialiseOn ,
	// этот флаг сообщает о том, что блок должен быть сериализован на диск
		if (checkedBlocks_.empty())
			return (std::nullopt);
		std::vector< std::string > checkedBlocks; 
		std::transform(checkedBlocks_.begin(),checkedBlocks_.end(),std::back_inserter(checkedBlocks),[](const std::pair<std::string,bool>& str){
			return ( str.first );
		});
		if (checkedBlocks.empty())
			return (std::nullopt);
		return (checkedBlocks);
	}

	template
	<
	typename bufferType,
	typename sqlCache
	>
	static
	std::optional<hashAndBlocks>
	getBlocksWithLayersById(const bufferType &faBuffer,sqlCache& sql/*,ul_long fillerToken*/)
	{
	#ifdef GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
		BEGIN()
	#endif 
		LayersIO::layersHelper  layersHelper;

		std::vector< blockSerialiseOn >  checkedBlocks_ = _checkChangedBlocks<bufferType>(faBuffer);
		if (checkedBlocks_.empty())
			return (std::nullopt);
		std::optional<std::vector< std::string > > checkedBlocks =	checkChangedBlocks(checkedBlocks_);
			if (!checkedBlocks)
				return std::nullopt;

		std::string    layerGuid;
		std::vector<SQLCMD::types::ul_long> v_layers = layersHelper.getlayerSomeThingByName< 
				std::vector<SQLCMD::types::ul_long> 
			>("layerId");	
		DB::my_recordSet selected  = getInfoAboutBlocks(*checkedBlocks);
		LayersIO::queryInfo_seq qi_s = LayersIO::recordSetToQueryInfo(selected);
	
	#ifdef GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
		for (auto q:qi_s)
		{
			_LOG::out_s << "q.blockGuid = " << q.blockGuid<< " q.layerHash = " << q.layerHash << " q.idx = " << q.idx << std::endl;
			LOGTOFILE(LOGHTML::messageType::NOTICE,_LOG::out_s);
		}
	#endif
		
		LayersIO::layerBuGuidOptional    layerByBlockGuid  = LayersIO::getLayersByBlockGuid(qi_s); // layersIO::getLayersByBlockGuid(qi_s);

		if (!layerByBlockGuid)
			return (std::nullopt);
		SQLCMD::types::ul_long max_layerId = layersHelper.getLastLayer<SQLCMD::types::ul_long>("layerId");
	#ifdef GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
		_LOG::out_s  << "max_layerId =" << max_layerId << std::endl;
			_LOG::out_s << "v_layers.size() = " << v_layers.size() << std::endl;
		for (auto i:v_layers)
			_LOG::out_s << " i = " << i << std::endl;	
		LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
	#endif

	#ifdef GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
		_LOG::out_s <<   " layers.size  = " << v_layers.size() <<  std::endl;
		for (auto l:v_layers)
			_LOG::out_s <<   "layer = " << l <<  std::endl;
		LOGTOFILE(LOGHTML::messageType::NOTICE,_LOG::out_s);
		LOGTOFILE(LOGHTML::messageType::NOTICE,"for( dbg:lbg )");
		auto lbg = *layerByBlockGuid;
		for (auto dbg:lbg)
			_LOG::out_s << dbg.first << ":" << dbg.second << " " << std::string((dbg.second == max_layerId)?"max_layerId - DETECTED":"max_layerId - NOT_DETECTED") << std::endl;
		LOGTOFILE(LOGHTML::messageType::NOTICE,_LOG::out_s);
	#endif

		std::optional<hashAndBlocks> ret =  getBlocksWithLayers(layersHelper,layerByBlockGuid,checkedBlocks_);
	#ifdef GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
		END()
	#endif
		return (ret);
	}


	static
	std::optional<hashAndBlocks>
	getBlocksWithLayers(LayersIO::layersHelper &layersHelper, LayersIO::layerBuGuidOptional layerByBlockGuid,std::vector< blockSerialiseOn >  checkedBlocks)
	{
		hashAndBlocks m_out;	
		
//TODO:
//переименовать ничего не значащую переменную i, во чтото более адекватное.		
		for (blockSerialiseOn i:checkedBlocks)
		{
			size_t layerId = (*layerByBlockGuid)[i.first];
			size_t old_layerId = layerId;
#ifdef GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
				_LOG::out_s << "i = " << i.first << " layerByBlockGuid = " << (*layerByBlockGuid)[i.first] <<  " layerId = " << layerId << std::endl;
				LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
#endif

#ifdef GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
			_LOG::out_s << "layerId =" << layerId << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
#endif
		  	std::optional<SQLCMD::types::ul_long> layerHash =
				layersHelper.getlayerSomeThingRelationNameByName< SQLCMD::types::ul_long , SQLCMD::types::ul_long >("layerHash","layerId",++layerId);					
			if (layerHash)
				m_out[ *layerHash ].push_back(i);
			else
			{
#ifdef GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
				LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"++layerId NOT_FOUND!");
#endif
				layerHash =layersHelper.getlayerSomeThingRelationNameByName< SQLCMD::types::ul_long , SQLCMD::types::ul_long >("layerHash","layerId",old_layerId);	
				m_out[ *layerHash ].push_back(i);
			}
#ifdef GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
				_LOG::out_s << "m_out[" << *layerHash << "] <= " <<  i.first  << std::endl;
				LOGTOFILE(LOGHTML::messageType::NOTICE,_LOG::out_s);
#endif
		}
#ifdef GET_BLOCK_WITH_LAYERS_BY_ID_LOG_ON
		END();
#endif
		return (m_out);
	}//	getBlocksWithLayers(


    /// @brief для заданных контрольных сумм блоков, во входящем параметре , ищет в базе соответствующие им :
	/// blockHash,status,layyerHash или пустой вектор если контрольные суммы не ннайдены.
    /// @param const std::vector<SQLCMD::valdesc>& collect_for_check - список контрольных сум блоков подлежащих обнаружению в базе. 
	/// @param fileGuid -соответствующий fileGuid для поиска контрольльтных сумм.
    /// @return для заданных контрольными суммами блоков(если они контрольнные суммы обнаружены в таблице blocks ) , возвращает соответствующие им поля: 
	///status,blockHash,layerHash,chk согласно запросу ниже:
	///SELECT blocks.status,blocks.blockHash,blocks.layerHash,blocks.chk FROM blocks 
	///	inner join buckets_and_blocks on blocks.blockHash = buckets_and_blocks.blockHash 
	//where blocks.chk IN(269733645692229,,,,N) AND buckets_and_blocks.fileGuid like 'XXX' 
	//если ничего найдено небыло, возврашается std::nullopt.
	std::optional<DB::my_recordSet> 
	_getCheckedBlocks(const std::vector<SQLCMD::valdesc>& collect_for_check,std::string fileGuid)
	{
		SQLCMD::selector_type m = {
                            {"chk", SQLCMD::valdesc(0,SQLCMD::valdesc::WHERE_IN)},
                            { "fileGuid", SQLCMD::valdesc(fileGuid,std::vector<std::string>({"buckets_and_blocks"}), SQLCMD::valdesc::WHERE_AND,false)},
                            { "status", SQLCMD::valdesc()},
                            { "blockHash", SQLCMD::valdesc()},
                            { "layerHash", SQLCMD::valdesc()}
    	};

		SQLCMD::Generator< SQLCMD::selector_type_v,
			SQLCMD::Select< SQLCMD::InnerJoinId0> >
			selectObjChecks("blocks", { "buckets_and_blocks" }, SQLCMD::get_ordered(m)
				);

		SQLCMD::wrapper<SQLCMD::selector_type>::add(
			{
				{"blockHash", SQLCMD::valdesc() },
			}
		);
		selectObjChecks.getCmdObject().addInParam(collect_for_check);
		selectObjChecks.getCmdObject().linkJoin({ "blockHash" }, { "blockHash" });
		DB::SQlite  sq("db_buckets");
		try
		{
			std::string s_ = selectObjChecks(); 
			#ifdef CHECK_CHANGED_BLOCKS_LOG_ON	
			_LOG::out_s << "s_ = " << s_ << std::endl; 
			LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
			#endif      
 			sq.execute(s_);
			if (sq.queryResultIsEmpty())
				return (std::nullopt);
		}
		catch(const std::exception& e)
		{
			#ifdef CHECK_CHANGED_BLOCKS_LOG_ON	
				_LOG::out_s << "exception detected" << e.what() << std::endl; 
				LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
			#endif   
			throw(e);
		}
		
		SQLCMD::valdesc::clearInstanceCounter();
		return (sq.getDataSet());
}



	static
	std::optional<SQLCMD::types::ul_long>
	getFileSizeByGuid(std::string fileGuid)
	{
		/*"layerId","layerHash"}*/
		#ifdef GET_FILE_SIZE_BY_GUID_LOG_ON
			BEGIN()
		#endif

		LayersIO::layersHelper _helper(fileGuid);
		_helper.setLayers();
		if (_helper.IsEmpty())
		{
			_LOG::out_s << "getFileSizeByGuid-error fileGuid = " <<  fileGuid << "not found!" << std::endl;
			throw std::runtime_error( _LOG::out_s.str() );			
		}
		const SQLCMD::types::ul_long LAYER_HASH_ID = _helper.getLastLayer<SQLCMD::types::ul_long>(std::string("layerHash"));
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


 template
    <
        typename bufferType
    >
	std::vector< blockSerialiseOn >
	_checkChangedBlocks( const	bufferType& faBuffer)
	{
		#ifdef CHECK_CHANGED_BLOCKS_LOG_ON	
			BEGIN()
		#endif
		std::string fileGuid = std::string(faBuffer.fileGuid); 
	    std::set<SQLCMD::types::ul_long> _hashes;
		enum{FIRST_ELEMENT=0};
		std::vector<SQLCMD::valdesc> collect_for_check;
		std::vector< blockSerialiseOn > ret;
		const size_t blocksCount =  faBuffer.blocks.size();
        bool isFinal = false;
  
		#ifdef CHECK_CHANGED_BLOCKS_LOG_ON	
			_LOG::out_s << "fileGuid = " << fileGuid  << std::endl; 
			LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
		#endif   

		std::transform(faBuffer.blocks.begin(),faBuffer.blocks.end(),std::back_inserter(collect_for_check),[](const DA::tdataAttrib_s& val)
		{
			const std::string& blockGuid = std::string(val.blockGuid);
			const SQLCMD::types::ul_long& CHECK = val.chk;	
			if ((0 != CHECK)||(utils::isFiller<DA::tdataAttrib_s>(val)))
				return (SQLCMD::valdesc(CHECK));
		});

		std::transform(faBuffer.blocks.begin(),faBuffer.blocks.end(),std::back_inserter(ret),[](const DA::tdataAttrib_s& val)
		{
			const std::string& blockGuid = std::string(val.blockGuid);
			bool ret = !utils::isFiller<DA::tdataAttrib_s>(val);
			return ( std::pair<std::string,bool>(blockGuid,ret));
		});

		#ifdef CHECK_CHANGED_BLOCKS_LOG_ON	
			_LOG::out_s << "collect_for_check.size() = " << collect_for_check.size() << "ret.size() = "  << ret.size() << std::endl; 
			LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
			for (size_t i = 0; i < collect_for_check.size(); ++i)
				_LOG::out_s << "collect_for_check[" << i  << "] = " << collect_for_check[i].value << std::endl; 
			LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
		#endif      
		
		std::optional<DB::my_recordSet> selected = _getCheckedBlocks(collect_for_check,fileGuid);
		
		if (!selected)
		{
			#ifdef CHECK_CHANGED_BLOCKS_LOG_ON	
			LOGTOFILE(LOGHTML::messageType::NOTICE,"selected.empty()");
			END();
			#endif      
			return ret;
		}
		ret.clear();
    
		const size_t counter = faBuffer.blocks.size();
		DB::my_recordSet _selected = *selected;
		for (size_t i = 0 ;	i < counter ; ++i)
		{	
			const std::string& blockGuid = std::string(faBuffer.blocks[i].blockGuid);
			SQLCMD::types::ul_long CHECK = faBuffer.blocks[i].chk;		
		  	auto status = DataSetAPI::getSomeThingRelationNameByName<SQLCMD::types::ul_long,SQLCMD::types::ul_long>
				(std::string("status"),std::string("chk"),CHECK, _selected);
	#ifdef CHECK_CHANGED_BLOCKS_LOG_ON	
			if 	(status)
			{
				_LOG::out_s << " status[OK] =" << *status  << std::endl;
				LOGTOFILE( LOGHTML::messageType::WARNING ,_LOG::out_s);
			}
	#endif

			std::optional<SQLCMD::types::ul_long> blockHash = DataSetAPI::getSomeThingRelationNameByName
				<SQLCMD::types::ul_long,SQLCMD::types::ul_long>
				(std::string("blockHash"),std::string("chk"),CHECK, _selected);

			const bool SERIALISER_ON = !utils::isFiller<DA::tdataAttrib_s>(faBuffer.blocks[i]);
			if (
				(blockHash) &&	(status) && (faBuffer.blocks[i].status == FILLERS::FILESIZE_INCREASE) || 
											(faBuffer.blocks[i].status == FILLERS::FILLER_FILESIZE_REPLACEMENT) && !_hashes.count(CHECK)
			)		
			{
				_hashes.insert(CHECK);
				ret.push_back(std::pair<std::string,bool>( blockGuid , SERIALISER_ON));
	#ifdef CHECK_CHANGED_BLOCKS_LOG_ON	
				_LOG::out_s << " chk[" << i << "]=" << faBuffer.blocks[i].chk <<" [status = ]" << *status << " blockHash = " << *blockHash << " SERIALISER_ON = " 
				<< SERIALISER_ON << " ACCEPTED!" << std::endl;
				LOGTOFILE( LOGHTML::messageType::WARNING ,_LOG::out_s);
	#endif
			}
			else
			{
	#ifdef CHECK_CHANGED_BLOCKS_LOG_ON	
				_LOG::out_s << " chk[" << i << "]=" << faBuffer.blocks[i].chk <<" status = " << *status << " blockHash = " << *blockHash << "SERIALISER_ON = " << SERIALISER_ON << " NOT_ACCEPTED!" << std::endl;
				LOGTOFILE( LOGHTML::messageType::WARNING ,_LOG::out_s);
	#endif
			}
		}

	#ifdef CHECK_CHANGED_BLOCKS_LOG_ON		
		_LOG::out_s << "ret.size() = " << ret.size()  << std::endl;  
		LOGTOFILE(LOGHTML::messageType::WARNING , _LOG::out_s);
		if (!_hashes.empty())
		{
			_LOG::out_s << "_hashes.size() = " << _hashes.size()  << std::endl;  
			LOGTOFILE(LOGHTML::messageType::WARNING , _LOG::out_s);
		
		}
		else
		{
			_LOG::out_s << " _hashes is empty!" << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING , _LOG::out_s);
		}
		END()
	#endif
		return ret;
	}//	_checkChangedBlocks(const	bufferType& faBuffer)


	 DB::my_recordSet
    getInfoAboutBlocks(const std::vector<std::string>& block_s)
    {
		#ifdef GET_INFO_ABOUT_BLOCKS_LOG_ON	
				BEGIN()
		#endif  
			SQLCMD::Generator< SQLCMD::selector_type_v,
			SQLCMD::Select< SQLCMD::InnerJoinId0> >
			_select("buckets_and_blocks", { "blocks" },
			SQLCMD::get_ordered
			(
				SQLCMD::selector_type	{
					{"fileGuid"     ,SQLCMD::valdesc()},
					{"layerHash"    ,SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,std::vector<std::string>({"blocks"}), SQLCMD::selector_cmd_type::NONE),},
					{"blockHash"    ,SQLCMD::valdesc(SQLCMD::selector_cmd_type::NONE,std::vector<std::string>({"blocks"}), SQLCMD::selector_cmd_type::NONE)},
					{ "idx"         ,SQLCMD::valdesc() },
					{ "blockGuid"   ,SQLCMD::valdesc(std::string(""),SQLCMD::valdesc::WHERE_IN)},
				}
			)
			);


			SQLCMD::wrapper<SQLCMD::selector_type>::add(
				{
					{"blockHash", SQLCMD::valdesc()}
				}
			);
			_select.getCmdObject().linkJoin({ "blockHash" }, { "blockHash" });
			std::vector<SQLCMD::valdesc> check_s;
			std::transform(block_s.begin(), block_s.end(), std::back_inserter(check_s), [](const std::string& str) {
				return (SQLCMD::valdesc(str));
				});
			_select.getCmdObject().addInParam(check_s);
            DB::SQlite  sq("db_buckets");
            std::string s = _select();
			#ifdef GET_INFO_ABOUT_BLOCKS_LOG_ON	
				_LOG::out_s << "s = " << s << std::endl; 
				LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
			#endif  
			try
            {
                sq.execute(s);
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
            SQLCMD::valdesc::clearInstanceCounter();
            DB::my_recordSet selected = sq.getDataSet(); 
		#ifdef GET_INFO_ABOUT_BLOCKS_LOG_ON	
			END()
		#endif  
            return (selected);
    };//getInfoAboutBlocks


}
#endif