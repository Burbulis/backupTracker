#ifndef __TCP_CONNECTION__DB_QUERIES_
#define __TCP_CONNECTION__DB_QUERIES_

#include "base.h"
#include "DA_.hpp"
#include "sql/SqClass.h"
#include "miniCache.hpp"
#include "iocrud.hpp"
#include "dispatcher.hpp"
#include "shprota/ReadFromBuff.h"
#include "IOTRansactor.hpp"
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


	//#define DBG_HEADER_RD_HANDLER
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
			_LOG::out_s << "md.fileGuid = " << md.fileGuid << " HEADER_SIZE =" << HEADER_SIZE  
			<< " xfer = " << xfer << " md.size = "  << md.size <<  "  md.sessionId = " <<  md.sessionId  << " md.typeOf = " << md.typeOf << " faBuffer.blocks.size() = " 
			<< faBuffer.blocks.size() << std::endl;
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
	   enum {TYPE_OF =1 , FILENAME ,PATH,TOKEN,SESSION_ID,FILE_GUID,CREATION_TIME,LAST_ACCESS_TIME,LAST_WRITE_TIME,BUFFER};
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		BEGIN()
	#endif	

		shprotaBuff::ReadFrom rf(buffer);

	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		_LOG::out_s <<	" rf.getCount() = " << rf.getCount() << " BUFFER = " << BUFFER << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
		_LOG::out_s << " rf.getString(FILENAME)" << rf.getString(FILENAME);
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
		_LOG::out_s << " rf.getString(FILENAME)" << rf.getString(PATH);
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
		_LOG::out_s << "rf.getString(TOKEN)" << rf.getString(TOKEN);
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
		_LOG::out_s <<" rf.getBuffer(BUFFER).size() = " << rf.getBuffer(BUFFER).size() << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif


		shprotaBuff::ReadFrom blocks(rf.getBuffer(BUFFER));
		std::vector<DA::tdataAttrib_s> _blocks;

		const size_t finalCount = blocks.getCount(); 
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		_LOG::out_s <<	" finalCount = " << finalCount << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif

	   //enum { ELAYERGUID = 1, EINDEX, EBLOCKGUID, ECHK, ESTATUS , EBUFFER };
      //  enum {  E_INDEX = 1, E_BLOCKGUID, E_CHK, E_STATUS ,E_BUFFER };
         enum { E_BEGIN = 1, E_INDEX, E_BLOCKGUID, E_CHK, E_STATUS, E_BUFFER, E_END };

		for (size_t i = 1; i <= finalCount; ++i)
		{
			DA::tdataAttrib_s attr;
			shprotaBuff::ReadFrom block(blocks.getBuffer(i));
			//block.getCount()
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		_LOG::out_s <<	" i = " << i <<  " block.getCount() == " << block.getCount() << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif		
			attr.begin = block.getString(E_BEGIN);
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		_LOG::out_s <<	 " attr.begin = " << attr.begin << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif		

			attr.index	= *block.extract64(E_INDEX);			
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		_LOG::out_s <<	 " attr.index = " << attr.index << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif		
			attr.blockGuid 	= block.getString(E_BLOCKGUID);
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		_LOG::out_s <<	 " attr.blockGuid = " << attr.blockGuid << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif		
			attr.chk 		= *block.extract64(E_CHK);
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		_LOG::out_s <<	 " attr.chk= " << attr.chk << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif	
			attr.status 	= *block.extract32(E_STATUS);
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		_LOG::out_s <<	" attr.status = " << attr.status << " attr.blockGuid = " << attr.blockGuid << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif
			if ((attr.status != DA::FILLERS::FILESIZE_REDUCTION) && (attr.status != DA::FILLERS::FILLER_IS_ZERO_FILE))
			{
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		LOGTOFILE(LOGHTML::messageType::MESSAGE,"FILESIZE_NOT_REDUCTED_OR_DELETED!");
	#endif
		
				attr.buffer=  block.getBuffer(E_BUFFER);
				attr.end		= block.getString(E_END);
			}
			else
			{
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		LOGTOFILE(LOGHTML::messageType::MESSAGE,"FILESIZE_REDUCTED OR DELETED!");
	#endif		
				attr.end		= block.getString(E_END - 1);
			}
			
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		_LOG::out_s <<	" attr.end = " <<  attr.end << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif
			_blocks.push_back(attr);
		} 
		DA::mainDescNorm md(rf.getString(FILE_GUID),rf.getString(TOKEN),rf.getString(SESSION_ID),
		rf.getString(PATH),rf.getString(FILENAME), *rf.extract32(TYPE_OF),*rf.extract64(CREATION_TIME),
		*rf.extract64(LAST_ACCESS_TIME),*rf.extract64(LAST_WRITE_TIME), _blocks);
		
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		END();
	#endif
		return (md);
	}


	//mainDesc
	//#define DBG_MAIN_DESC_EXTRACTOR
	
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


		enum{ TYPEOF = 1, FILE_GUID ,PATH,FILENAME, SESSION_ID,SIZE};	
		shprotaBuff::ReadFrom rf(buffer);
		
	#ifdef DBG_MAIN_DESC_EXTRACTOR
		_LOG::out_s <<	" rf.getCount() = " << rf.getCount() << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif

	
		DA::mainDesc_s md("NOT_HAVE_ANY_TOKEN",
					rf.getString(PATH),
					rf.getString(FILENAME),
					rf.getString(FILE_GUID),
					rf.getString(SESSION_ID),
					*rf.extract32(TYPEOF),
					*rf.extract64(SIZE)
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