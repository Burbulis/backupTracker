#ifndef __TCP_CONNECTION__DB_QUERIES_
#define __TCP_CONNECTION__DB_QUERIES_

#include "base.h"
#include "DA/DA_.hpp"
#include "sql/SqClass.h"
#include "cache/minicache.h"
#include "iocrud.hpp"
#include "dispatch/dispatcher.hpp"
#include "shprota/ReadFromBuff.h"
#include "IO/IOTRansactor.hpp"
//#include "layers/layersHelper.hpp"
//#include "BlocksAndLayers.hpp"
#define _DEBUG_HEADER_TRAKT_ON_
#define DEBUG_SELECT_TRAKT
#define __SERIALISER_ON__


class tcp_connection
    :public boost::enable_shared_from_this<tcp_connection>
{
    public:
    typedef boost::shared_ptr<tcp_connection>  ptr;  
private:
	IO_CRUD::sqlInsert sqlInsert;
	std::mutex guard;	
    static ptr ptr_this;
    static bool complete;
    size_t deep_of_recursion;
	mem_mgr<uint8_t> _mheader;
	mem_mgr<uint8_t> _mreader;
    //std::map<std::string,mainDesc> mDesc_s;
	std::set<std::string> clientsId_s;
    boost::asio::io_service& io_s;
    boost::asio::ip::tcp::socket _socket;

	public:
    tcp_connection(boost::asio::io_service& _io):io_s(_io),_socket(_io),
    deep_of_recursion(0),sqlInsert(400)
    { 
	    const size_t FIRST_BUCKETS_SIZE = 14000000;
		const size_t FIRST_REQ_BUFFER_SIZE = 4000;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,"++tcp_connection()");	
		_mheader.reinit(FIRST_REQ_BUFFER_SIZE);
		_LOG::out_s << "_mheader.size() = " << 	_mheader.size() <<  std::endl; 
		LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
		
		_mreader.reinit(FIRST_BUCKETS_SIZE);
   }



	//template
	//<
	//	typename bufferType
	//>
    static ptr create(boost::asio::io_service& io)
    {
    	tcp_connection::reinit();
    
		_LOG::out_s << "ptr_this.use_count() ==" << ptr_this.use_count() << std::endl;
		LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);

        if (nullptr == ptr_this)
        {
            LOGTOFILE(LOGHTML::messageType::WARNING,"++init tcp_connection..");
	      //  _LOG::out_s << "ptr_this.use_count() ==" << ptr_this.use_count() << std::endl;
    		//LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
			tcp_connection *t = new tcp_connection(io);
			LOGTOFILE(LOGHTML::messageType::WARNING,"t is complete created!");
            ptr_this =ptr(t);
			LOGTOFILE(LOGHTML::messageType::WARNING,"--init tcp_connection..");
        }
        else
        {
            LOGTOFILE(LOGHTML::messageType::WARNING,"++reinit tcp_connection..");
             ptr_this.reset(new tcp_connection(io));
            LOGTOFILE(LOGHTML::messageType::WARNING,"--reinit tcp_connection..");
        }
        _LOG::out_s << "ptr_this.use_count() ==" << ptr_this.use_count() << std::endl;
		LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
		return  ptr_this; 
	}

	static void destroy(void)
	{
        ptr_this.reset();
	}

    void rd_stop()
    {
		complete = true;
		if (_socket.is_open())
		{
			if (!io_s.stopped())
			{
				complete = true;
				boost::system::error_code ec;
				_socket.shutdown(
					boost::asio::ip::tcp::socket::shutdown_both,ec);
				if (ec)
				{
					LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"shutdown error!");
					ec.clear();
				}
				_socket.close(ec);
				if (ec)
				{
					LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"error close socket...");
					ec.clear();
				}
			}
		}
    }

#define DBG_STUB_BUCKETS_RD_HANDLER

	bool STUB_BUCKETS_RD_HANDLER(const boost::system::error_code &ec,size_t xfer)
	{
#ifdef DBG_STUB_BUCKETS_RD_HANDLER
	BEGIN()
#endif
			std::vector<uint8_t> Tmp(_mreader.getMem().begin(),_mreader.getMem().end());
			DA::mainDescNorm faBuffer = mainDescAndBlocksExtractor(Tmp);
#ifdef DBG_STUB_BUCKETS_RD_HANDLER
			_LOG::out_s << "Tmp.size() =" << Tmp.size()  << " xfer = " << xfer << " faBuffer.fileGuid = " << faBuffer.fileGuid  <<  
			" faBuffer.sessionId = " << faBuffer.sessionId << 	" faBuffer.blocks.size() = " << faBuffer.blocks.size()  <<  std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);	
#endif
			DISPATCHER::stub_dispatch_action<ptr,DA::mainDescNorm,IO_CRUD::sqlInsert,boost::asio::ip::tcp::socket>(_socket,ptr_this,sqlInsert,faBuffer);

#ifdef DBG_STUB_BUCKETS_RD_HANDLER
	END()
#endif
	}


	bool BUCKETS_RD_HANDLER(const boost::system::error_code &ec,size_t xfer)
	{	
		if (ec)
		{		
			throw std::runtime_error(std::string("Connection error : ") + ec.message());	
		}
#if defined(_DEBUG_UPDATE_RECORDS_ON_) || defined(_DEBUG_CREATE_RECORDS_ON_)
			LOGTOFILE(LOGHTML::messageType::WARNING,"++BUCKETS_RD_HANDLER()");
			_LOG::out_s << " xfer = " << xfer << " _mreader.size() = " << _mreader.size() << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
#endif

		if (_mreader.size()!= xfer)
		{
			_LOG::out_s << " xfer = " << xfer << " _mreader.size() =" << _mreader.size() << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
			rd_stop();	
			return (false);
		}		
		//mainDescAndBlocksExtractor()
#if defined(_DEBUG_UPDATE_RECORDS_ON_) || defined(_DEBUG_CREATE_RECORDS_ON_)
		END();
#endif
		return (true);
	}


	#define DBG_HEADER_RD_HANDLER
	#define DBG_GET_RECORDS_FOR_CHECKING
    void HEADER_RD_HANDLER(const boost::system::error_code &ec,size_t xfer)
	{
		#ifdef DBG_HEADER_RD_HANDLER	
			BEGIN()
		#endif	
		if (ec)
		{		
			std::string error = std::string("Connection error : ") + ec.message();
			std::cout << "error = " << error << std::endl;
			throw std::runtime_error(error);	
		}	
		const size_t HEADER_SIZE = _mheader.size(); 
		DA::mainDesc_s md =	mainDescExctractor();
		_mreader.reinit(md.size);
		DA::mainDescNorm faBuffer = DA::mainDescNorm(md);
		#ifdef DBG_HEADER_RD_HANDLER
			_LOG::out_s << "md.fileGuid = " << md.fileGuid << " md.layerGuid = " << md.layerGuid << " HEADER_SIZE =" << HEADER_SIZE  
			<< " xfer = " << xfer << " md.size = "  << md.size <<  "  md.sessionId = " <<  md.sessionId  << " md.typeOf = " << md.typeOf << " faBuffer.blocks.size() = " 
			<< faBuffer.blocks.size() << " faBuffer.layerGuid = " << faBuffer.layerGuid << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
			END()
		#endif
			_mreader.reinit(md.size);

		DISPATCHER::stub_dispatch_action<ptr,DA::mainDescNorm,IO_CRUD::sqlInsert,boost::asio::ip::tcp::socket>(_socket,ptr_this,sqlInsert,faBuffer);

	#ifdef DBG_HEADER_RD_HANDLER	
		END()
	#endif	
	}


    static bool is_complete()
    {
		//if (complete)
		 //	DB::sqlQueryCache::flush();
		return (complete);
    }

    void readHeader()
    {
		if (is_complete())
			return;
		const size_t FIRST_REQ_BUFFER_SIZE = 4000;	
	#ifdef _DEBUG_HEADER_TRAKT_ON_	
		LOGTOFILE(LOGHTML::messageType::WARNING,"++readHeader()");	
		_LOG::out_s << "try to readHeader buffer" <<std::endl;
		_LOG::out_s << "deep_of_recursion = " 	  << deep_of_recursion << std::endl;
        _LOG::out_s << "_mheader.size() =" 		  << _mheader.size() << std::endl;
		_LOG::out_s << "_mheader.isblocked() = "  << _mheader.isBlocked() << std::endl;
		LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
		LOGTOFILE(LOGHTML::messageType::WARNING,"set HEADER_RD_HANDLER..");
		
	#endif	
	
		boost::asio::async_read(
			_socket,
        	boost::asio::buffer(_mheader.getMem()),
			boost :: asio :: transfer_all() ,
			boost::bind(&tcp_connection::HEADER_RD_HANDLER,shared_from_this(),boost::placeholders::_1,boost::placeholders::_2)
		);
	#ifdef _DEBUG_HEADER_TRAKT_ON_	
		LOGTOFILE(LOGHTML::messageType::WARNING,"--readHeader()");	
        _LOG::out_s <<"is_complete() =" << is_complete() << std::endl;
		LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
	#endif
		std::cout << "readHeader exit" << std::endl;
    }

	#define DBG_READ_BUCKET

	void readBucket()
	{
#ifdef DBG_READ_BUCKET
		BEGIN()
#endif
		if (is_complete())
			return;

#ifdef DBG_READ_BUCKET
		_LOG::out_s << "_mreader.size() =" << _mreader.size() << " deep_of_recursion = " << deep_of_recursion << std::endl;
		LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
#endif		
		

		size_t size = _mreader.size();
	//	std::cout << "size = " << size << std::endl; 
		boost::system::error_code ec;	

		//memset(&testBuffer[0],0,size);
		boost::asio::async_read(
							_socket,
							boost::asio::buffer(_mreader.getMem()),
							 boost::asio::transfer_exactly(size),
							boost::bind(&tcp_connection::STUB_BUCKETS_RD_HANDLER,shared_from_this(),boost::placeholders::_1,boost::placeholders::_2)
					);
#ifdef DBG_READ_BUCKET
		END()
#endif	
	}

	//#define DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
	DA::mainDescNorm mainDescAndBlocksExtractor(std::vector<uint8_t> buffer)
	{
	  // enum {TYPE_OF =1 , FILENAME ,PATH,TOKEN,SESSION_ID,FILE_GUID,CREATION_TIME,LAST_ACCESS_TIME,LAST_WRITE_TIME,BUFFER,END = BUFFER};
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		BEGIN()
	#endif	

		shprotaBuff::ReadFrom rf(buffer);
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		auto umap = rf.getUmap();
    for (auto v : umap)
	{
    		_LOG::out_s <<	" [" << v.first << "/" << v.second << "]" << std::endl;
			 LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	}
		#endif
	 auto _blocks = rf.getBuffer("blocks");
		
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		_LOG::out_s <<	" _blocks.size() = " << _blocks.size() << std::endl;//<< " BUFFER = " << BUFFER << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
		_LOG::out_s <<	" rf.getCount() = " << rf.getCount() << std::endl;//<< " BUFFER = " << BUFFER << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
		_LOG::out_s << " rf.getString(FILENAME)" << rf.getString("fileName");
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
		_LOG::out_s << " rf.getString(PATH)" << rf.getString("pathFile");
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
		_LOG::out_s << "rf.getString(TOKEN)" << rf.getString("token");
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
		_LOG::out_s << " rf.getString(FILEGUID)" << rf.getString("fileGuid");
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
		if (rf.checkField("blocks"))
		{
			LOGTOFILE(LOGHTML::messageType::MESSAGE,"blocks - found!");
		}
		else
		{
			LOGTOFILE(LOGHTML::messageType::MESSAGE,"blocks - NOT found!");
		}
		_LOG::out_s <<" blocks = " << _blocks.size() << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif

	
		shprotaBuff::ReadFrom blocks(_blocks);
		std::vector<DA::tdataAttrib_s> v_blocks;

		const size_t finalCount = blocks.getCount(); 
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		_LOG::out_s <<	" blocks.getCount() = " << finalCount << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif

	   //enum { ELAYERGUID = 1, EINDEX, EBLOCKGUID, ECHK, ESTATUS , EBUFFER };
      //  enum {  E_INDEX = 1, E_BLOCKGUID, E_CHK, E_STATUS ,E_BUFFER };
       //  enum { E_BEGIN = 1, E_INDEX, E_BLOCKGUID, E_CHK, E_STATUS, E_BUFFER, E_END };

		if (blocks.checkField("BinaryBuffers"))
		{
			LOGTOFILE(LOGHTML::messageType::MESSAGE,"(0)BinaryBuffers - detected!");
		}
		else
		{
			LOGTOFILE(LOGHTML::messageType::MESSAGE,"(0)BinaryBuffers - not detected!");
		}

		for (size_t i = 1; i <= finalCount; ++i)
		{
			DA::tdataAttrib_s attr;
			auto vBlock = blocks.getBuffer(i);
			shprotaBuff::ReadFrom block(vBlock);
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		auto umap = block.getUmap();
		for (auto v : umap)
		{
				_LOG::out_s <<	" [" << v.first << "/" << v.second << "]" << std::endl;
				LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
		}
	#endif		
			//block.getCount()
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		_LOG::out_s <<	" i = " << i <<  " block.getCount() == " << block.getCount() << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif		
			attr.begin = block.getString("begin");
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		_LOG::out_s <<	 " attr.begin = " << attr.begin << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif		

			attr.index	= *block.extract64("index");			
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		_LOG::out_s <<	 " attr.index = " << attr.index << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif		
			attr.blockGuid 	= block.getString("blockGuid");
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		_LOG::out_s <<	 " attr.blockGuid = " << attr.blockGuid << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
		if (block.checkField("chkSum"))
			LOGTOFILE(LOGHTML::messageType::MESSAGE,"chkSum - found!");
	#endif		
		attr.chk 		= *block.extract64("chkSum");
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		_LOG::out_s <<	 " attr.chk= " << attr.chk << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif	
			
			attr.status 	= *block.extract32("status");
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		_LOG::out_s <<	" attr.status = " << attr.status << " attr.blockGuid = " << attr.blockGuid <<  std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif
			if ((attr.status != DA::FILLERS::FILESIZE_REDUCTION) && (attr.status != DA::FILLERS::FILLER_IS_ZERO_FILE))
			{
				attr.buffer		=  block.getBuffer("BinaryBuffer");
				attr.end		=  block.getString("end");
			}
			else
			{
				attr.end		= block.getString("end");
			}
			v_blocks.push_back(attr);		
		} 
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		_LOG::out_s << "v_blocks.size() = " << v_blocks.size() <<std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif

		auto fileGuid = rf.getString("fileGuid");
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		_LOG::out_s <<	" fileGuid = " <<  fileGuid << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif

	
		auto token = rf.getString("token");
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		_LOG::out_s <<	" token = " <<  token << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
		if (rf.checkField("sessionId"))
			LOGTOFILE(LOGHTML::messageType::MESSAGE,"sessionId - found!");
	
	#endif


		auto sessionId = rf.getString("sessionId");
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		_LOG::out_s <<	" sessionId = " <<  sessionId << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif
	
		auto filePath  = rf.getString("pathFile");
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		_LOG::out_s <<	" filePath = " <<  filePath << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif
		auto fileName  = rf.getString("fileName");
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		_LOG::out_s <<	" fileName = " <<  fileName << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif

		auto typeOf = *(rf.extract32("typeOf"));
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		_LOG::out_s <<	" typeOf = " <<  typeOf << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif



		
		auto creationTime = *(rf.extract64("creationTime"));
		auto lastAccessTime = *(rf.extract64("lastAccessTime"));
		auto lastWriteTime = *(rf.extract64("lastWriteTime"));
		DA::mainDescNorm md(fileGuid,token,sessionId,
		filePath,fileName, typeOf,creationTime,
		lastAccessTime,lastWriteTime, v_blocks);
		
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		END();
	#endif
		return (md);
	}


	//mainDesc
	#define DBG_MAIN_DESC_EXTRACTOR
	
	DA::mainDesc_s mainDescExctractor(void)
	{
	#ifdef DBG_MAIN_DESC_EXTRACTOR
		BEGIN()
	#endif		
		//std::unique_ptr<headerType> faBuffer = std::make_unique<headerType>();	
		//size_t position = 0;
		//size_t tmp;
		//size_t  SZ = _mheader.size(); 
		//printf("SZ = %d\r\n",SZ);

		std::vector<uint8_t> buffer;
		std::vector<uint8_t> _tmp = _mheader.getMem();

	#ifdef DBG_MAIN_DESC_EXTRACTOR
		_LOG::out_s <<	" _tmp.size() = " << _tmp.size() << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif

		buffer.resize(_tmp.size());
		memmove(&buffer[0],&_tmp[0],_tmp.size());	

	#ifdef DBG_MAIN_DESC_EXTRACTOR
		_LOG::out_s <<	"after memmove buffer.size() = " << buffer.size() << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif


         enum { TYPEOF = 1, FILE_GUID =2, PATH = 3, FILENAME = 4, SESSION_ID = 5,LAYER_GUID = 6, SIZE = 7 };

		shprotaBuff::ReadFrom rf(buffer);
		
	#ifdef DBG_MAIN_DESC_EXTRACTOR
		_LOG::out_s <<	" rf.getCount() = " << rf.getCount() << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif
	
		auto pathFile  = rf.getString("pathFile"); 
	#ifdef DBG_MAIN_DESC_EXTRACTOR
		_LOG::out_s <<	" pathFile = " << pathFile << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif

		auto fileName  = rf.getString("fileName");
	#ifdef DBG_MAIN_DESC_EXTRACTOR
		_LOG::out_s <<	" fileName = " << fileName << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif

		auto fileGuid  = rf.getString("fileGuid");
	#ifdef DBG_MAIN_DESC_EXTRACTOR
		_LOG::out_s <<	" fileGuid = " << fileGuid << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif
		auto sessionId = rf.getString("sessionId");

	#ifdef DBG_MAIN_DESC_EXTRACTOR
		_LOG::out_s <<	" sessionId = " << sessionId << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif
		if (rf.checkField("layerGuid"))
		{
			auto layerGuid = rf.getString("layerGuid");
	#ifdef DBG_MAIN_DESC_EXTRACTOR
		_LOG::out_s <<	" layerGuid = " << ((layerGuid.empty())? std::string("not detected") : layerGuid )<< std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif
		}
		if (rf.checkField("typeOf"))
		{	
			auto typeOf = *(rf.extract32("typeOf"));
	#ifdef DBG_MAIN_DESC_EXTRACTOR
		_LOG::out_s <<	" typeOf = " << typeOf << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif
		}

		if (rf.checkField("size"))
		{	
			auto size =   *(rf.extract64("size"));

	#ifdef DBG_MAIN_DESC_EXTRACTOR
		_LOG::out_s <<	" size = " << size << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif
		}
		DA::mainDesc_s md("NOT_HAVE_ANY_TOKEN",
					rf.getString("pathFile"),
					rf.getString("fileName"),
					rf.getString("fileGuid"),
					rf.getString("sessionId"),
					rf.checkField("layerGuid")?rf.getString("layerGuid") : "",
					rf.checkField("typeOf")? *(rf.extract32("typeOf")) : 0,
					rf.checkField("size")? *(rf.extract64("size")) : 0
				);
		#ifdef DBG_MAIN_DESC_EXTRACTOR
			END()
		#endif		
		return (md);
	}

    boost::asio::ip::tcp::socket&
    socket() 
    { return _socket; }

    ~tcp_connection()
    {
    }
private:
    static void reinit()
    {
		#ifdef  _DEBUG_UPDATE_RECORDS_ON_
		LOGTOFILE(LOGHTML::messageType::MESSAGE,"++reinit()");
		#endif
		complete = false;
        ptr_this.reset();
		#ifdef  _DEBUG_UPDATE_RECORDS_ON_
		LOGTOFILE(LOGHTML::messageType::MESSAGE,"--reinit()");
		#endif
    }

};//tcp_connection
tcp_connection::ptr tcp_connection::ptr_this = nullptr;
bool tcp_connection::complete = false;
//mem_mgr tcp_connection::_mreader;
#endif