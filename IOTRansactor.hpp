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
    //enum{FILEGUID = 1,SIZEOF};
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
	//	size_t fileSize = LAYER_QUERIES::getFileSizeByGuid(fileGuid);
	//	auto fileSize = layersAndBlocks::getFileSizeByGuid(fileGuid); 
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

//		FILE *f = fopen("test.bin","wb+");
//		fwrite(blocksAsBuffer.data(),1,blocksAsBuffer.size(),f);
//		fclose(f);		
		
		size_t writed =	boost::asio::write(_socket,boost::asio::buffer(blocksAsBuffer));	

	#ifdef  SEND_DATA
		_LOG::out_s <<"writed = " <<  writed << std::endl; 	
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
		END()
	#endif
		return writed;
	}


}

