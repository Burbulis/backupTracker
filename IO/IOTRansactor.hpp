#include <boost/asio.hpp>
#include "../blocks/utils/utils.hpp"
#include "../shprota/ReadFromBuff.h"
#include "../dataset/datasetAPI.hpp"
#include "../cache/minicache.h"

#ifndef __IOTRANSACTOR__
#define __IOTRANSACTOR__

namespace IoTransactor
{

	struct layerType
	{
		layerType(
			std::string _Guid,
			SQLCMD::types::ul_long _Hash,
			SQLCMD::types::ul_long _Id,
			std::tm _Time
		):
		Guid(_Guid),
		Hash(_Hash),
		Id(_Id),
		Time(_Time)
		{}
		enum{
			GUID = 1,
			ID,
			TIME
		};
		std::vector <uint8_t> toBinary(void)
		{
			shprotaBuff::writeTo bufflayers;
			bufflayers.add("Guid",Guid);
			bufflayers.add("Id",static_cast<uint64_t>(Id));
			bufflayers.add("Time",Time);
			return bufflayers();
		}

		void
		fromBinary(std::vector <uint8_t> buffer)
		{
		    shprotaBuff::ReadFrom rf(buffer);
        	Guid = rf.getString("Guid");
        	Id   = *rf.extract64("Id");
        	Time = *rf.getDateTime("Time");
		}
		std::string Guid;
		SQLCMD::types::ul_long Hash;
		SQLCMD::types::ul_long Id;
		std::tm Time;
	};


	template
	<
		typename Tsocket
	>
	static size_t _writeParams(Tsocket& _socket ,const uint64_t _sizeOf ,std::string fileGuid)
	{
		shprotaBuff::writeTo wt;
		wt.add("fileGuid",fileGuid);
		wt.add("sizeOf",static_cast<uint64_t>(_sizeOf));
		std::vector<uint8_t> _buffer = utils::set_size(wt(),500);
		size_t writed =	boost::asio::write(_socket,boost::asio::buffer(_buffer));
		return (0);
	}


	#define SEND_DATA
	template
	<
        typename Tsocket,
		typename bufferType
	>
	static size_t sendData(Tsocket& _socket,bufferType md)
	{
	#ifdef  SEND_DATA
		BEGIN()
		_LOG::out_s  << ".typeOf =" << md.typeOf << " REAL_QUERY!" << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);    
	#endif

		std::string fileGuid(md.fileGuid);	
		fileGuid = IO_CRUD::getFileGuidByName(std::string(md.fileName) , std::string(md.pathFile));
	#ifdef  SEND_DATA
		_LOG::out_s << "FILE_GUID_STAGE_1 = " << fileGuid << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif

		Layers::layersHelper _helper(fileGuid);  
        _helper.setLayers();
        SQLCMD::types::ul_long layerHash  = *_helper.getHashById(*_helper.getIdByGuid(md.layerGuid));
    	IO_CRUD::cachedRecords cr(fileGuid);
		bool existed = cr.exist(fileGuid,layerHash);
		DB::my_recordSet sq = (existed)?cr.get(fileGuid,layerHash):cr.get_current_record(fileGuid);//(existed)?cr.get_current_record(fileGuid) : DB::EMPTY();
		//cr.
	#ifdef  SEND_DATA
		_LOG::out_s << "existed = " << existed << std::endl; 
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
		for (auto x:sq)
		{
			_LOG::out_s << "x = " << x.first << std::endl; 
			LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
		}
	#endif

	if ( DB::IsEmpty(sq) )
		  throw std::runtime_error(" fileGuid not exist in the selected layer of db");
		

		auto  valChk 	   = DataSetAPI::getSomeThingByName< std::vector<SQLCMD::types::ul_long> >("chk",sq); 
		auto  valFGuid 	   = DataSetAPI::getSomeThingByName< std::vector<std::string> >("fileGuid",sq);  
		auto  valBlockGuid = DataSetAPI::getSomeThingByName< std::vector<std::string> >( "blockGuid" , sq);
		auto  valIdx 	   = DataSetAPI::getSomeThingByName< std::vector< SQLCMD::types::ul_long > >("idx",sq);
		auto  valSize 	   = DataSetAPI::getSomeThingByName< std::vector< SQLCMD::types::ul_long > >("size", sq);
		auto  valStatus	   = DataSetAPI::getSomeThingByName< std::vector< SQLCMD::types::ul_long > >("status" ,sq);	
		auto  valLayer	   = DataSetAPI::getSomeThingByName< std::vector<std::string> >("layerGuid",sq);
	
		
	#ifdef  SEND_DATA
		if ((valChk.size()!=valFGuid.size())||(valFGuid.size()!= valBlockGuid.size())||(valIdx.size()!=valBlockGuid.size())||(valSize.size()!=valBlockGuid.size()))
		{
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"Size of SEQ mismach!");		
		}
		bool dbg_ = cr.exist(fileGuid);
		_LOG::out_s << "fileguid = " << fileGuid << ((dbg_ == true) ? std::string(" - FOUND"):std::string("NOT FOUND"))  << std::endl; 
		LOGTOFILE((dbg_)?LOGHTML::messageType::MESSAGE : LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);		
		_LOG::out_s << "valBlockGuid.size() = " << valBlockGuid.size()  << " / valIdx.size() =" << valIdx.size() << std::endl; 
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
		_LOG::out_s  << "valLayer.size() = " << valLayer.size() <<" layerGuid = " << md.layerGuid << " layer = " <<((!valLayer.empty())?valLayer[0] : std::string("EMPTY!?")) << " ret query empty? = " <<  
		valChk.empty() << "||" << valIdx.empty() << "||" << valBlockGuid.empty() <<  std::endl;
		LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
	#endif	
	
		
		if ( valChk.empty() || valBlockGuid.empty() )
		{
			throw std::runtime_error("can't execute IO_CRUD::selectMainDataSqlCmd!");			
		}
		shprotaBuff::writeTo blocks;
		//enum{BEGIN = 1,LAYER_GUID,BLOCK_GUID,CHK,INDEX,STATUS,SIZE,END};
		for (size_t i = 0 ;i< valBlockGuid.size(); ++i)
		{		
			shprotaBuff::writeTo block;
			block.add("BEGIN",std::string("ABCD_"));
			block.add("Layer",valLayer[i]);
			block.add("blockGuid",valBlockGuid[i]);
			block.add("chk",valChk[i]);
			block.add("index",valIdx[i]);
			block.add("status",valStatus[i]);
			block.add("size",valSize[i]);
			block.add("END",std::string("_DCBA"));
			blocks.add("CURRENT_BLOCK",block());
		}
		size_t count = blocks.getId();

	#ifdef  SEND_DATA
		_LOG::out_s <<"count =" << count << std::endl;	
		LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);					
	#endif

		std::vector<uint8_t> blocksAsBuffer = blocks();
		IoTransactor::_writeParams(_socket,blocksAsBuffer.size(),fileGuid);	
		size_t writed =	boost::asio::write(_socket,boost::asio::buffer(blocksAsBuffer));	
	
	#ifdef  SEND_DATA
		_LOG::out_s <<"writed = " <<  writed << std::endl; 	
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
		END()
	#endif
		return writed;
	}


	#define DBGQUERY_FILES_AND_PATHS
	template
	<
        typename Tsocket
	>
	void queryFilesAndPaths(Tsocket& _socket)
	{
	#ifdef	DBGQUERY_FILES_AND_PATHS
		BEGIN()	
	#endif
			DB::my_recordSet rs = LAYER_QUERIES::getfilesAndPaths();

			if (rs.empty())
				throw std::runtime_error("system records not found or empty!");

	#ifdef	DBGQUERY_FILES_AND_PATHS
			_LOG::out_s << "rs.size() = " << rs.size() <<  std::endl;
			LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif
			auto fileNames = DataSetAPI::getSomeThingByName< std::vector<std::string> >("fileName",rs);
			auto fileGuids = DataSetAPI::getSomeThingByName< std::vector<std::string> >("fileGuid",rs);
			auto pathFiles = DataSetAPI::getSomeThingByName< std::vector<std::string> >("pathFile",rs);

			if (fileGuids.empty() || fileGuids.empty() || pathFiles.empty())
				throw std::runtime_error("something table of system records empty!");


			if ((fileGuids.size() != fileNames.size()) || (fileGuids.size()!= pathFiles.size()))
				throw std::runtime_error("table of system records data mismatch!");

	#ifdef	DBGQUERY_FILES_AND_PATHS
			_LOG::out_s << "fileNames.size() = " << fileNames.size() <<  std::endl;
			LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
			_LOG::out_s << "fileGuids.size() = " << fileNames.size() <<  std::endl;
			LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
			_LOG::out_s << "pathFiles.size() = " << fileNames.size() <<  std::endl;
			LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif
			shprotaBuff::writeTo ret;			
			for (size_t i = 0 ; i < fileGuids.size(); ++i)
			{
				try
				{
					auto fileSize =  LayersIO::getFileSizeByGuid(fileGuids[i]); //::getFileSizeByGuid( fileGuids[i]);
					_LOG::out_s << "fileSize = " << *fileSize << " fileGuid[" << i  << "]" << fileGuids[i]  << std::endl;
					LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
					shprotaBuff::writeTo wt;
					wt.add("fileGuid",fileGuids[i]); //enum{FILE_GUID = 1,PATH_FILE,FILE_NAME,FILE_SIZE}
					wt.add("pathFile",pathFiles[i]);
					wt.add("fileName",fileNames[i]);
					wt.add("fileSize",*fileSize);
					ret.add("filesAndPaths",wt());
				}
				catch(const std::exception& e)
				{
					std::cerr << "ALARM!!!" << std::endl;
					std::cerr << e.what() << '\n';
					getc(stdin);
					break;
				}
			}

	 std::vector <uint8_t> buff  = ret();
	 //utils::set_size()
	 IoTransactor::_writeParams(_socket, buff.size() ,"_NEW_GUID_001_");	
		//	IOTRasactor::_writeParams()	
	 size_t writed =	boost::asio::write(_socket,boost::asio::buffer( buff ));
	#ifdef DBGQUERY_FILES_AND_PATHS	
		_LOG::out_s << "writed = " << writed << std::endl;
		LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
	#endif
	}

#define GET_FILE_BY_GUID_BY_NAME

	std::string 
	getFileGuidByName(
		std::string pathFile,
		std::string fileName)
{
	#ifdef  GET_FILE_BY_GUID_BY_NAME		
		BEGIN()
	#endif
	IO_CRUD::_fileCache fc;
		DB::my_recordSet rc;
		try
		{ 
			rc = fc.getFilesAndGuidsByPath(pathFile);
		}
		catch(std::runtime_error &re)
		{
			throw re;	
		}

		std::vector<std::string> fileNames = rc["fileName"];
		std::vector<std::string>::iterator It =	std::find_if(fileNames.begin(),fileNames.end(),[fileName](std::string str){
				return (!fileName.compare(str));
			});
		if (fileNames.end() != It)
		{ 
	#ifdef  GET_FILE_BY_GUID_BY_NAME		
			_LOG::out_s << "fileNames It = " << *It << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
	#endif		
		}
		else
		{
			types::ul_long _hash =  utils::minicache::str_hash(fileName)/10000;	
			_LOG::out_s << "fileName  = " << fileName << " _hash = " << _hash << " NOT_FOUND" << std::endl;
			throw (std::runtime_error(_LOG::out_s.str()));
		}
		size_t index = std::distance(fileNames.begin(),It);
		std::string fileGuid  =  rc["fileGuid"][index];
	#ifdef  GET_FILE_BY_GUID_BY_NAME		
			_LOG::out_s << " pathFile = " << pathFile << " fileName = " << fileName << " in fileNames.size() = " << fileNames.size() << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
			_LOG::out_s <<	" index = "<<	index  << " fileGuid = " << fileGuid  << std::endl;
			LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
			_LOG::out_s << "rc.size() = " << rc.size() << " rc.count('fileName') =" <<  rc.count("fileName") << " fileGuid = " << fileGuid << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
	#endif

		return (fileGuid);
	}


	#define DBG_QUERY_MAIN_DATA
	template
	<
	    typename Tsocket,
		typename bufferType
	>
	bool queryMainData(Tsocket& _socket,bufferType md)
	{
	#ifdef  DBG_QUERY_MAIN_DATA	
		BEGIN()
	#endif
		std::string fileGuid(md.fileGuid);
	#ifdef  DBG_QUERY_MAIN_DATA		
		LOGTOFILE(LOGHTML::messageType::MESSAGE,"try to getGuid...");
	#endif

		std::string fileName = md.fileName;	
		fileGuid = getFileGuidByName(md.pathFile,fileName);  
	

		size_t recordCount = 0;
		try
		{
			IO_CRUD::cachedRecords cr(fileGuid);
			cr.actuallyRecordSet(fileGuid);
			recordCount =  cr.exist(fileGuid) ? cr.get(fileGuid)["idx"].size() : 0 ;
		}
		catch(const std::runtime_error re)
		{
			_LOG::out_s <<	"re.what() = " << re.what() << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
			throw re;	
		}
		catch(const std::exception& e)
		{
			_LOG::out_s <<	"e.what() = " << e.what() << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
			throw e;	
		}
		//laye
	#ifdef  DEBUG_SELECT_TRAKT		
		LOGTOFILE(LOGHTML::messageType::MESSAGE,"try to write...");
		_LOG::out_s <<  "fileGuid = " << fileGuid << " layerGuid = " << md.layerGuid << std:: endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
		_LOG::out_s <<	"recordCount = " << recordCount << std::endl;
		LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
	#endif		
		size_t writed = 0;
		try
		{
			writed = IoTransactor::sendData<  Tsocket, bufferType >( _socket,md);
		}
		catch(const std::exception& e)
		{
			_LOG::out_s <<"CATCHED exception"<< e.what() << '\n';			
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
			throw e;	

		}	
	#ifdef  DEBUG_SELECT_TRAKT		
		_LOG::out_s <<	"writed = " <<	writed << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
		END();
	#endif		
		return (true);
	}

/***************************************************************************
 * TODO: Формирует буффер для отправки списока слоёв для файла 
 *  fileGuid в виде хэшей   
 ***************************************************************************/
#define LAYERS_BY_GUID_TO_SEND_DBG
	template
	<
	    typename Tsocket
	>
 	size_t LayersByGuidToSend(Tsocket& _socket , std::string fileGuid)
	{
	#ifdef  LAYERS_BY_GUID_TO_SEND_DBG		
		BEGIN();
	#endif	

		Layers::layersHelper _layers(fileGuid);
		_layers.setLayers();
		std::vector<SQLCMD::types::ul_long> layersHashes = _layers.getHashes();
		_layers.resetLayers();
		std::vector<layerType> layersIds;
		std::transform(layersHashes.begin(),layersHashes.end(),std::back_inserter(layersIds),[&_layers](SQLCMD::types::ul_long currentHash)
			{
			    SQLCMD::types::ul_long Id = *_layers.getIdByHash(currentHash); 
			    return (layerType( *_layers.getGuidById(Id),currentHash,Id, *_layers.getTimeById(Id)));
			}
				);
		shprotaBuff::writeTo bufflayers;
		for ( size_t i = 0; i < layersIds.size(); ++i)
		{
			auto buffer = 	    layersIds[i].toBinary();
	#ifdef  LAYERS_BY_GUID_TO_SEND_DBG		
		_LOG::out_s <<	"buffer.size() = " <<	buffer.size() <<  std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif	

			bufflayers.add("layerBinary",buffer);	
		}
		size_t Id = bufflayers.getId();
		std::vector<uint8_t> buff = bufflayers();
	#ifdef  LAYERS_BY_GUID_TO_SEND_DBG		
		_LOG::out_s <<	"buff.size() = " <<	buff.size() << " Id = " <<  Id <<  std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif	
		IoTransactor::_writeParams(_socket, buff.size() ,"_NEW_GUID_001_");	
		//	IOTRasactor::_writeParams()	
	 	size_t writed =	boost::asio::write(_socket,boost::asio::buffer( buff ));
	#ifdef  LAYERS_BY_GUID_TO_SEND_DBG		
		END();
	#endif	
		return writed;
	}
}
#endif

