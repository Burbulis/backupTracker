#include <boost/asio.hpp>
#include "utils.hpp"
#include "shprota/ReadFromBuff.h"
#include "BlocksAndLayers.hpp"
#include "datasetAPI.hpp"
#include "miniCache.hpp"
namespace IoTransactor
{

template
<
    typename Tsocket
>
static size_t _writeParams(Tsocket& _socket ,const uint64_t _sizeOf ,std::string fileGuid)
{
	shprotaBuff::writeTo wt;
    wt.add(fileGuid);
    wt.add(uint64_t(_sizeOf));
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
		TRACE();
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

		IO_CRUD::cachedRecords cr(fileGuid);
		bool existed = cr.exist(fileGuid);
		DB::my_recordSet sq = cr.get_current_record(fileGuid);//(existed)?cr.get_current_record(fileGuid) : DB::EMPTY();

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
	
		
	#ifdef  SEND_DATA
		if ((valChk.size()!=valFGuid.size())||(valFGuid.size()!= valBlockGuid.size())||(valIdx.size()!=valBlockGuid.size())||(valSize.size()!=valBlockGuid.size()))
		{
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"Size of SEQ mismach!");		
		}
		bool dbg_ = cr.exist(fileGuid);
		_LOG::out_s << "fileguid " << fileGuid << ((dbg_ == true) ? std::string(" - FOUND"):std::string("NOT FOUND"))  << std::endl; 
		LOGTOFILE((dbg_)?LOGHTML::messageType::MESSAGE : LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);		
		_LOG::out_s << "valBlockGuid.size() = " << valBlockGuid.size()  << " / valIdx.size() =" << valIdx.size() << std::endl; 
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif	
	
		auto valLayer=sq["layerGuid"];
		_LOG::out_s  << "valLayer.size() = " << valLayer.size() << " ret query empty? = " <<  valChk.empty() << "||" << valIdx.empty() << "||" << valBlockGuid.empty() <<  std::endl;
		LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
		if ( valChk.empty() || valBlockGuid.empty() )
		{
			throw std::runtime_error("can't execute IO_CRUD::selectMainDataSqlCmd!");			
		}
		shprotaBuff::writeTo blocks;
		//enum{BEGIN = 1,LAYER_GUID,BLOCK_GUID,CHK,INDEX,STATUS,SIZE,END};
		for (size_t i = 0 ;i< valBlockGuid.size(); ++i)
		{		
			shprotaBuff::writeTo block;
			block.add(std::string("ABCD_"));
			block.add(valLayer[i]);
			block.add(valBlockGuid[i]);
			block.add(valChk[i]);
			block.add(valIdx[i]);
			block.add(valStatus[i]);
			block.add(valSize[i]);
			block.add(std::string("_DCBA"));
			blocks.add(block());
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
					auto fileSize = layersAndBlocks::getFileSizeByGuid( fileGuids[i]);
					_LOG::out_s << "fileSize = " << *fileSize << " fileGuid[" << i  << "]" << fileGuids[i]  << std::endl;
					LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
					shprotaBuff::writeTo wt;
					wt.add(fileGuids[i]); //enum{FILE_GUID = 1,PATH_FILE,FILE_NAME,FILE_SIZE}
					wt.add(pathFiles[i]);
					wt.add(fileNames[i]);
					wt.add(*fileSize);
					ret.add(wt());
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
			//writeParams<bufferType_12,baseFileDesc,false>(fileNames.size(),"_NEW_GUID_001_");
	 size_t writed =	boost::asio::write(_socket,boost::asio::buffer( buff ));
	#ifdef DBGQUERY_FILES_AND_PATHS	
		_LOG::out_s << "writed = " << writed << std::endl;
		LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
	#endif
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
		IO_CRUD::_fileCache fc;
		DB::my_recordSet rc;
		try
		{ 
			rc = fc.getFilesAndGuidsByPath(md.pathFile);
		}
		catch(std::runtime_error &re)
		{
			throw re;	
		}
	#ifdef  DBG_QUERY_MAIN_DATA		
			_LOG::out_s << "rc.size() = " << rc.size() << " rc.count('fileName') =" <<  rc.count("fileName") << " fileGuid = " << fileGuid << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
	#endif
		std::string fileName = md.fileName;	
		
		std::vector<std::string> fileNames = rc["fileName"];
	#ifdef  DBG_QUERY_MAIN_DATA		
			_LOG::out_s << "md.pathFile = " << md.pathFile << " fileName = " << fileName << " in fileNames.size() = " << fileNames.size() << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
	#endif
		std::vector<std::string>::iterator It =	std::find_if(fileNames.begin(),fileNames.end(),[fileName](std::string str){
				return (!fileName.compare(str));
			});

	#ifdef  DBG_QUERY_MAIN_DATA		
		types::ul_long _hash =  utils::minicache::str_hash(fileName)/10000;
		if (fileNames.end() != It)
		{ 
			_LOG::out_s << "PATH_FOUND = " << *It << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
		}
		else
		{

			_LOG::out_s << "fileName  = " << fileName << " _hash = " << _hash << " NOT_FOUND" << std::endl;
			throw (std::runtime_error(_LOG::out_s.str()));
		}
	#endif

		size_t index = std::distance(fileNames.begin(),It);

		fileGuid  = (!fileGuid.compare(std::string("NOT_PRESENT")))? rc["fileGuid"][index] : fileGuid;

	#ifdef  DBG_QUERY_MAIN_DATA		
		_LOG::out_s <<	" index = "<<	index  << " fileGuid = " << fileGuid  << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif
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
		
	#ifdef  DEBUG_SELECT_TRAKT		
		LOGTOFILE(LOGHTML::messageType::MESSAGE,"try to write...");
		_LOG::out_s <<  "fileGuid = " << fileGuid << std:: endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
		_LOG::out_s <<	"recordCount = " << recordCount << std::endl;
		_LOG::out_s <<  "bufferType_4000::SIZE_OF_BLOCKS =" << bufferType_4000::SIZE_OF_BLOCKS << std::endl;
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
		_LOG::out_s <<	"writed = " <<	writed << "bufferType_12::SIZE_OF_BLOCKS = " << bufferType_4000::SIZE_OF_BLOCKS << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
		END();
	#endif		
		return (true);
	}

/***************************************************************************
 * TODO: Формирует буффер для отправки списока слоёв для файла с fileGuid в виде хэшей   
 ***************************************************************************/
	std::vector<uint8_t> LayersByGuidAnswer(std::string fileGuid)
	{
		LayersIO::layersHelper Layers(fileGuid);
		Layers.setLayers();
		std::vector<SQLCMD::types::ul_long> layersHashes =  Layers.getlayerSomeThingByName< std::vector<SQLCMD::types::ul_long> >("layerHash");
		shprotaBuff::writeTo bufflayers;
  		for ( size_t i = 0; i < layersHashes.size(); ++i)
    		bufflayers.add(uint64_t(layersHashes[i]));
  		size_t count = bufflayers.getId();
  		return bufflayers();
	}





}

