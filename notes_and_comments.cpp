			//DB::my_recordSet info =	DB_QUERIES::getInfoAboutBlocks(v);
			//createNewLayer(layerGuid);
/*			DB::my_recordSet selected = DB_QUERIES::getInfoAboutBlocks(v);
			DB_QUERIES::queryInfo_seq  qi_s = DB_QUERIES::recordSetToQueryInfo(selected);
			DB_QUERIES::layerByGuid lbg = DB_QUERIES::getLayersByBlockGuid(qi_s);
			std::vector<std::string> blocksToNextLayer;
		//	std::vector<std::string> blockAnotherLayer;			
			size_t nextLayerId = 0;

			if (
				(!blocksToNextLayer.empty()) && (IT_IS_FIRST_UPDATE)		
			)
			{
				layerGuid = DB_QUERIES::createNewLayer();
				layerHash = utils::minicache::str_hash(layerGuid) / 100000;
				nextLayerId = BASE_LAYER_ID;
#ifdef _DEBUG_UPDATE_RECORDS_ON_
				_LOG::out_s << " New layer Created! layerGuid = " << layerGuid << "currentLayerId = " <<  nextLayerId << std::endl;	
				LOGTOFILE(_LOG::out_s);
#endif
			}
			else
			{
#ifdef _DEBUG_UPDATE_RECORDS_ON_
				LOGTOFILE("IT_IS_NOT_FIRST_UPDATE!");
#endif
				auto id_s = layers["layerId"];
				std::vector<SQLCMD::types::ul_long> v_layers;			
				std::transform(id_s.begin(),id_s.end(),std::back_inserter(v_layers),[](const std::string& str)
				{
					return (std::stoull(str.c_str()));	
				});

#ifdef _DEBUG_UPDATE_RECORDS_ON_
			
			_LOG::out_s << " UPDATE RECORD[12]!!!!! layerHash =" << layerHash << std::endl;
			_LOG::out_s << "lbg.size = " << lbg.size() << std::endl;

			for (auto l: lbg)
				_LOG::out_s << "block = " << l.first << " layerId = " << l.second << std::endl;
			
			for (std::string i:blocksToNextLayer)
				_LOG::out_s << "** i = " << i <<  std::endl;

			_LOG::out_s << "** nextLayerId = " << nextLayerId <<  std::endl;
			LOGTOFILE(_LOG::out_s);

	#ifdef _DEBUG_ANY_KEY_WAITING_ON_
			LOG("press any key...");
			getc(stdin);
	#endif
#endif
*/