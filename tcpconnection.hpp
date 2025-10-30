#ifndef __TCP_CONNECTION__DB_QUERIES_
#define __TCP_CONNECTION__DB_QUERIES_

#include "base.h"
#include "DA_.hpp"
#include "SqClass.h"
#include "miniCache.hpp"
#include "iocrud.hpp"
#include "shprota/ReadFromBuff.h"
#include "IOTRansactor.hpp"
#include "layersHelper.hpp"
#include "BlocksAndLayers.hpp"
#define _DEBUG_HEADER_TRAKT_ON_
#define DEBUG_SELECT_TRAKT
//#define _DEBUG_CREATE_RECORDS_ON_
//#define _DEBUG_ANY_KEY_WAITING_ON_
//#define _DEBUG_UPDATE_RECORDS_ON_
//#define _DEBUG_SERIALISER_ON_
#define __SERIALISER_ON__

//static char *testBuffer = NULL;

static void DBG(std::string message)
{
	std::cout << message << std::endl;
	getc(stdin);
}


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
	mem_mgr _mheader;
	static mem_mgr _mreader;
    std::map<std::string,mainDesc> mDesc_s;
	std::set<std::string> clientsId_s;
    boost::asio::io_service& io_s;
    boost::asio::ip::tcp::socket _socket;

	public:
    tcp_connection(boost::asio::io_service& _io):io_s(_io),_socket(_io),
    deep_of_recursion(0),sqlInsert(400)
    { 
		TRACE();
		const size_t FIRST_REQ_BUFFER_SIZE = 4000;
	    LOGTOFILE(LOGHTML::messageType::MESSAGE,"++tcp_connection()");	
		_mheader.reinit(FIRST_REQ_BUFFER_SIZE);
		_LOG::out_s << "_mheader.size() = " << 	_mheader.size() << " headerType::BUFFER_SIZE = " << headerType::BUFFER_SIZE << std::endl; 
		LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
		_mreader.reinit(writeBufferType_12::SIZE_OF_BLOCKS);
	    LOGTOFILE(LOGHTML::messageType::MESSAGE,"--tcp_connection()");	
   }



	template
	<
		typename bufferType
	>
    static ptr create(boost::asio::io_service& io)
    {
    	TRACE();
		_LOG::out_s << "bufferType::TBUFF_SIZE = " << bufferType::TBUFF_SIZE << std::endl; 
		LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
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
		LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"try to stop tcp engine...");
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
			DA::mainDescNorm md = mainDescAndBlocksExtractor(Tmp);
#ifdef DBG_STUB_BUCKETS_RD_HANDLER
			_LOG::out_s << "Tmp.size() =" << Tmp.size()  << " xfer = " << xfer << " md.fileGuid = " << md.fileGuid  << 
			" md.sessionId = " << md.sessionId << 
			" md.blocks.size() = " << md.blocks.size()  <<  std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);	
#endif

			stub_dispatch_action(md);
					
	

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

		std::string token("_START_OF_BUCKET_");
		const size_t begin_position = _mreader.findSeq<std::string>(token);// ( sq_buffer_.end() != iter )? std::distance(sq_buffer_.begin(),iter) : 0;
		std::unique_ptr<writeBufferType_12> faBuffer_12 = _mreader.make<writeBufferType_12>(begin_position);
#if defined(_DEBUG_UPDATE_RECORDS_ON_) || defined(_DEBUG_CREATE_RECORDS_ON_)
			_LOG::out_s << " begin_position = " << begin_position << std::endl;
			LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);	
#endif
		if (faBuffer_12->buff.md.typeOf == UPDATE_RECORDS_COUNT6)
		{
			std::unique_ptr<writeBufferType_6> faBuffer_6 = _mreader.make<writeBufferType_6>(begin_position);
#if defined(_DEBUG_UPDATE_RECORDS_ON_) || defined(_DEBUG_CREATE_RECORDS_ON_)
			_LOG::out_s << "faBuffer_6->buff.md.typeOf = " << faBuffer_6->buff.md.typeOf << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING, _LOG::out_s );
#endif
			return (true);	
		}
#if defined(_DEBUG_UPDATE_RECORDS_ON_) || defined(_DEBUG_CREATE_RECORDS_ON_)
			_LOG::out_s << "faBuffer_12->buff.md.typeOf = " << faBuffer_12->buff.md.typeOf << std::endl;
			LOGTOFILE(LOGHTML::messageType::WARNING, _LOG::out_s );
#endif
		//	dispatch_action<writeBufferType_12>(faBuffer_12);
		//readHeader();
#if defined(_DEBUG_UPDATE_RECORDS_ON_) || defined(_DEBUG_CREATE_RECORDS_ON_)
		LOGTOFILE(LOGHTML::messageType::WARNING,"--BUCKETS_RD_HANDLER()");
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
			throw std::runtime_error(std::string("Connection error : ") + ec.message());	
		}	
		const size_t HEADER_SIZE = _mheader.size(); 
		
		//mainDesc md = 
		DA::mainDesc_s md =	mainDescExctractor(xfer);
		
		//DA::mainDescNorm md = mainDescAndBlocksExtractor(Tmp);
		#ifdef DBG_HEADER_RD_HANDLER	
			_LOG::out_s << "md.fileGuid = " << md.fileGuid << " HEADER_SIZE =" << HEADER_SIZE  
			<< " xfer = " << xfer << " md.size"  << md.size <<  "  md.sessionId = " <<  md.sessionId  << " md.typeOf = " << md.typeOf << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
			END()
		#endif
	//	stub_dispatch_action(md);	
		switch (md.typeOf)
		{


			case CREATE_NEW_RECORDS:
			{
				#ifdef DBG_HEADER_RD_HANDLER
						LOGTOFILE(LOGHTML::messageType::WARNING,"CREATE_NEW_RECORDS");
				#endif
				_mreader.reinit(md.size);
				readBucket();	
			}
			break;

			case UPDATE_RECORDS_COUNT12:	
			{
				#ifdef DBG_HEADER_RD_HANDLER
						LOGTOFILE(LOGHTML::messageType::WARNING,"UPDATE_RECORDS_COUNT12");
				#endif
				_mreader.reinit(md.size);
				readBucket();				
			}
			break;	

			case GET_RECORDS_FOR_CHECKING:
			{
			#ifdef DBG_GET_RECORDS_FOR_CHECKING
				LOGTOFILE(LOGHTML::messageType::WARNING,"GET_INFO_RECORDS");
			#endif
				IoTransactor::queryMainData<boost::asio::ip::tcp::socket,DA::mainDescNorm>(_socket,DA::mainDescNorm(md));					
				readHeader();
			}
			break;

			case DA::QUERY_FILES_AND_PATHS:
			{
		#ifdef DBG_HEADER_RD_HANDLER
				LOGTOFILE(LOGHTML::messageType::WARNING,"QUERY_FILES_AND_PATHS");
		#endif
				IoTransactor::queryFilesAndPaths(_socket);	
				readHeader();
			}
			break;

			break;

		}
		
	#ifdef DBG_HEADER_RD_HANDLER	
		END()
	#endif	
	}

 /*	#define DBG_HEADER_RD_HANDLER
   void HEADER_RD_HANDLER(const boost::system::error_code &ec,size_t xfer)
    {
		#ifdef DBG_HEADER_RD_HANDLER	
			BEGIN()
		#endif	
		if (ec)
		{		
			throw std::runtime_error(std::string("Connection error : ") + ec.message());	
		}	
		const size_t HEADER_SIZE = _mheader.size(); 
	#ifdef DBG_HEADER_RD_HANDLER	
		LOGTOFILE(LOGHTML::messageType::WARNING,"++HEADER_RD_HANDLER()");
		_LOG::out_s << "rd_transferred = " << xfer << " _mheader.size()" << HEADER_SIZE << std::endl;
		LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
	#endif

	
		if (
		   (xfer == 0) 
		|| ( ec ) 
		|| ( xfer !=  HEADER_SIZE)
		|| (headerType::BUFFER_SIZE != HEADER_SIZE )
		)
		{
	#ifdef DBG_HEADER_RD_HANDLER	
			_LOG::out_s << "Error = " << ec.message() << " HEADER_SIZE =" << HEADER_SIZE  << " xfer = " << xfer <<  " headerType::BUFFER_SIZE = " << headerType::BUFFER_SIZE << std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
	#endif	
			FILE *f = fopen("header_err.bin","wb+");
			fwrite(_mheader.getMem().data(),_mheader.size(),1,f);
			fclose(f);
			return;
		}

		//DA::
		mainDescExctractor(xfer);
		
		if (md.chk == ERRORS::UNCKNOWN_ERROR)
		{
			readHeader();
			return;
		}	
		_mreader.reinit(md.size);
	#ifdef DBG_HEADER_RD_HANDLER	
		_LOG::out_s << "|md.size = " 	  		  << md.size      <<  std::endl;
		_LOG::out_s << "|_mreader.size() = "	  << HEADER_SIZE     << std::endl;
		_LOG::out_s << "|md.sessionId = " << md.sessionId << " md.typeOf = " << md.typeOf << std::endl;
	//	_LOG::out_s << "|DA::QUERY_FILES_AND_PATHS =" << DA::QUERY_FILES_AND_PATHS <<  std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif	
	#ifdef DBG_HEADER_RD_HANDLER
		LOGTOFILE(LOGHTML::messageType::WARNING,"CURRENT EVENT:");
	#endif
		switch (md.typeOf)
		{
			case GET_RECORDS_FOR_CHECKING:
			{
			#ifdef DBG_HEADER_RD_HANDLER
				LOGTOFILE(LOGHTML::messageType::WARNING,"GET_INFO_RECORDS");
			#endif
				queryMainData(md);					
				readHeader();
			}
				break;
			case DA::QUERY_FILES_AND_PATHS:
			{
		#ifdef DBG_HEADER_RD_HANDLER
				LOGTOFILE(LOGHTML::messageType::WARNING,"QUERY_FILES_AND_PATHS");
		#endif
				queryFilesAndPaths();	
				readHeader();
			}
				break;
			
			case DA::ADD_NEW_RECORD:
			{
				#ifdef DBG_HEADER_RD_HANDLER
						LOGTOFILE(LOGHTML::messageType::WARNING,"ADD_NEW_RECORD");
				#endif
				readBucket();
			}
				break;
			case CREATE_NEW_RECORDS:
			{
				#ifdef DBG_HEADER_RD_HANDLER
						LOGTOFILE(LOGHTML::messageType::WARNING,"CREATE_NEW_RECORDS");
				#endif
				readBucket();	
			}
				break;
			case UPDATE_RECORDS_COUNT12:
			{
				#ifdef DBG_HEADER_RD_HANDLER
					LOGTOFILE(LOGHTML::messageType::WARNING,"UPDATE_RECORDS_COUNT12");
				#endif
				readBucket();		
			}
				break;
			case UPDATE_RECORDS_COUNT6:
			{
				#ifdef DBG_HEADER_RD_HANDLER
					LOGTOFILE(LOGHTML::messageType::WARNING,"UPDATE_RECORDS_COUNT6");
				#endif
				readBucket();
			}
				break;	
			default:
				break;
		}

		#ifdef DBG_HEADER_RD_HANDLER	
			END()
		#endif	
		return;
    }// void HEADER_RD_HANDLER(boost::system::error_code const &ec,size_t xfer)
	*/
	static void
	showConsoleTraceLog(void)
	{
		std::cout << TRACE_STR() << std::endl;
	}


    static bool is_complete()
    {
		if (complete)
		 	DB::sqlQueryCache::flush();
		return (complete);
    }

    void readHeader()
    {
		TRACE();
		if (is_complete())
			return;
		const size_t FIRST_REQ_BUFFER_SIZE = 4000;	
		//_mheader.reinit(FIRST_REQ_BUFFER_SIZE);	
	//	_mheader.setBlock();
	#ifdef _DEBUG_HEADER_TRAKT_ON_	
		LOGTOFILE(LOGHTML::messageType::WARNING,"++readHeader()");	
	//	_LOG::out_s << "complte_var == "   		  << is_complete() <<std::endl;
		_LOG::out_s << "try to readHeader buffer" <<std::endl;
		_LOG::out_s << "deep_of_recursion = " 	  << deep_of_recursion << std::endl;
        _LOG::out_s << "_mheader.size() =" 		  << _mheader.size() << std::endl;
		_LOG::out_s << "_mheader.isblocked() = "  << _mheader.isBlocked() << std::endl;
	//	_LOG::out_s << "sizeof(mainDesc) =" 	  << sizeof(mainDesc) << std::endl;
	//	_LOG::out_s << "sizeof(fileAttrib) = " 	  << sizeof(fileAttrib) << std::endl;
//		_LOG::out_s << "sizeof(_fileAttrib) = "   << sizeof(_fileAttrib) << std::endl; 
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
		std::cout << "size = " << size << std::endl; 
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

	#define DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
	DA::mainDescNorm mainDescAndBlocksExtractor(std::vector<uint8_t> buffer)
	{
	   enum {TYPE_OF =1 , FILENAME ,PATH,TOKEN,SESSION_ID,FILE_GUID,CREATION_TIME,LAST_ACCESS_TIME,LAST_WRITE_TIME,BUFFER};
	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		BEGIN()
	#endif	

		shprotaBuff::ReadFrom rf(buffer);

	#ifdef DBG_MAIN_DESC_AND_BLOCK_EXTRACTOR
		_LOG::out_s <<	" rf.getCount() = " << rf.getCount() << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
		_LOG::out_s <<	" rf.getBuffer(BUFFER).size() = " << rf.getBuffer(BUFFER).size() << std::endl;
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
			attr.begin 		= block.getString(E_BEGIN);
			attr.index 		= *block.extract64(E_INDEX);
			attr.blockGuid 	= block.getString(E_BLOCKGUID);
			attr.chk 		= *block.extract64(E_CHK);
			attr.status 	= *block.extract32(E_STATUS);
			if (attr.status != FILLERS::FILESIZE_REDUCTION)
			{
				attr.buffer=  block.getBuffer(E_BUFFER);
			}
			attr.end		= block.getString(E_END);

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
	#define DBG_MAIN_DESC_EXTRACTOR
	
	DA::mainDesc_s mainDescExctractor(size_t  transferred)
	{
	#ifdef DBG_MAIN_DESC_EXTRACTOR
		BEGIN()
	#endif		
		//std::unique_ptr<headerType> faBuffer = std::make_unique<headerType>();	
		//size_t position = 0;
		//size_t tmp;
		auto  SZ = _mheader.size(); 
		printf("SZ = %d\r\n",SZ);
		std::vector<uint8_t> buffer;
		auto _tmp = _mheader.getMem();
		buffer.resize(_tmp.size());
		memmove(&buffer[0],&_tmp[0],_tmp.size());	
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

 #define STUB_DISPATCH_ACTION

	//template
	//<
	//	typename bufferType
//	>
    void stub_dispatch_action(
		DA::mainDescNorm md
	)
    {
	#ifdef  STUB_DISPATCH_ACTION
		BEGIN();
		_LOG::out_s << "  md.typeOf = " << md.typeOf  << std::endl;
		LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
	#endif
		if (is_complete())
	#ifdef  STUB_DISPATCH_ACTION
	END();
	#endif
			return;

	switch (md.typeOf)
			{
				case CREATE_NEW_RECORDS:
				{

				#ifdef __SERIALISER_ON__
					try
					{
						diskOperations::stubSerialise(md);
					}
					catch(std::exception& re)
					{
						_LOG::out_s << "serialise exception = " << re.what()  << std::endl;
						LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
					}
				#endif	
					LayersIO::layersHelper	layerHelper;
					if (!layerHelper.IsEmpty())
					{
						SQLCMD::types::ul_long layerHash = layerHelper.getFirstLayer< SQLCMD::types::ul_long >("layerHash");
					#ifdef STUB_DISPATCH_ACTION
						_LOG::out_s << " md.blocks.size() = " << md.blocks.size() << std::endl;
						LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
					#endif
						LOGTOFILE(LOGHTML::messageType::MESSAGE,"is not empty!");
						if (
							!sqlInsert.exec(md,layerHash)
						)
						{
							readBucket();
							return;
						}
						else
						{
							#ifdef  STUB_DISPATCH_ACTION
								LOGTOFILE(LOGHTML::messageType::MESSAGE,"FINALISED!");
							#endif	
						}
					}
					readHeader();
				}
				break;
				case	UPDATE_RECORDS_COUNT12:
				{

	#if defined(_DEBUG_UPDATE_RECORDS_ON_) || defined(_DEBUG_CREATE_RECORDS_ON_)

					LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"UPDATE_RECORDS_COUNT12");
	#endif

					try
					{
						if (sqlInsert.update<DA::mainDescNorm>(md))
						{
	#ifdef _DEBUG_UPDATE_RECORDS_ON_			
						LOGTOFILE( LOGHTML::messageType::STRONG_WARNING,"record updated complete." );
	#endif				
							readHeader();
						}
					}
					catch(const std::exception& e)
					{
					//	_LOG::out_s << "exception detected : " << e.what();
					//	LOGTOFILE( LOGHTML::messageType::STRONG_WARNING,_LOG::out_s );
						rd_stop();
					}
				}
				break;
							case DA::QUERY_FILES_AND_PATHS:
			{
		#ifdef DBG_HEADER_RD_HANDLER
				LOGTOFILE(LOGHTML::messageType::WARNING,"QUERY_FILES_AND_PATHS");
		#endif
				IoTransactor::queryFilesAndPaths<boost::asio::ip::tcp::socket>(_socket);	
				readHeader();
			}
			break;

			case GET_RECORDS_FOR_CHECKING:
			{
			#ifdef DBG_GET_RECORDS_FOR_CHECKING
				LOGTOFILE(LOGHTML::messageType::WARNING,"GET_INFO_RECORDS");
			#endif
				IoTransactor::queryMainData<boost::asio::ip::tcp::socket,DA::mainDescNorm>(_socket , md);					
				readHeader();
			}
			break;
			#define DBG_QUERY_LAYERS
			case QUERY_LAYERS:
			{
				#ifdef DBG_QUERY_LAYERS
					LOGTOFILE(LOGHTML::messageType::WARNING,"GET_LAYERS_BY_FILEGUID:");
				#endif
				IoTransactor::LayersByGuidAnswer(md.fileGuid);
			}
			break;
			}		
	#ifdef  STUB_DISPATCH_ACTION
		END();
	#endif
	}


/*	template
	<
		typename bufferType
	>
    void dispatch_action(
		const std::unique_ptr<bufferType>& faBuffer
	)
    {
	#ifdef  _DEBUG_UPDATE_RECORDS_ON_
		BEGIN();
	#endif

		if (is_complete())
		{
			LOGTOFILE(LOGHTML::messageType::MESSAGE,"all complete!");
			return;
		}

//	#ifdef _DEBUG_CREATE_RECORDS_ON_ | _DEBUG_UPDATE_RECORDS_ON_
	#if defined(_DEBUG_UPDATE_RECORDS_ON_) || defined(_DEBUG_CREATE_RECORDS_ON_)

		_LOG::out_s << "| bufferType::BUFFER_SIZE =" << bufferType::BUFFER_SIZE << std::endl;
		_LOG::out_s << "| bufferType::SIZE_OF_BLOCKS =" << bufferType::SIZE_OF_BLOCKS << std::endl;
		_LOG::out_s << "| _mreader.size() =" << _mreader.size() << std::endl;	
		_LOG::out_s << "| complete() == " << is_complete() << std::endl;
		_LOG::out_s << "| sizeof(fileAttrib) =" << sizeof(fileAttrib) << std::endl;
		_LOG::out_s << "| sizeof( faBuffer.buff.md ) == " << sizeof( faBuffer->buff.md ) << std::endl;
		_LOG::out_s << "| sizeof( faBuffer.buff.md.typeOf ) == " << sizeof( faBuffer->buff.md.typeOf ) << std::endl;
		_LOG::out_s << "| faBuffer->buff.md.typeOf   =" << faBuffer->buff.md.typeOf << std::endl;
		_LOG::out_s << "| faBuffer->buff.md.size     =" << faBuffer->buff.md.size << std::endl;
		_LOG::out_s << "| faBuffer->buff.md.fileName =" << faBuffer->buff.md.fileName << std::endl;
		_LOG::out_s << "| faBuffer->buff.md.pathFile =" << faBuffer->buff.md.pathFile << std::endl; 
		_LOG::out_s << "| faBuffer->buff.md.fileGuid =" << faBuffer->buff.md.fileGuid <<std::endl; 	
		_LOG::out_s << "| faBuffer->buff.md.sessionId =" << faBuffer->buff.md.sessionId <<std::endl; 
		_LOG::out_s << "| faBuffer->buff.md.ftCreationTime   =" << std::string(faBuffer->buff.md.ftCreationTime) << std::endl;
		_LOG::out_s << "| faBuffer->buff.md.ftLastAccessTime =" << std::string(faBuffer->buff.md.ftLastAccessTime) << std::endl;
		_LOG::out_s << "| faBuffer->buff.md.ftLastWriteTime =" << std::string(faBuffer->buff.md.ftLastWriteTime) << std::endl;
		_LOG::out_s << "| faBuffer->buff.md.chk =" << faBuffer->buff.md.chk << std::endl;
		LOGTOFILE(LOGHTML::messageType::WARNING,_LOG::out_s);
		LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"******************************************");
		_LOG::out_s << "***bufferType::BUFFER_SIZE = " << bufferType::BUFFER_SIZE << std::endl;
		_LOG::out_s << "(faBuffer->buff.md.size == bufferType::SIZE_OF_BLOCKS) = " << (faBuffer->buff.md.size == bufferType::SIZE_OF_BLOCKS) << "***bufferType::SIZE_OF_BLOCKS = " << bufferType::SIZE_OF_BLOCKS << std::endl;
		LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
		LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"******************************************");
	#endif


		auto chk_ = uid_sAndId_s::chkCoder<ui_int,ul_long>(
			reinterpret_cast<char *>(&faBuffer->buff.data[0]),
			reinterpret_cast<char *>(&faBuffer->buff.data[bufferType::SIZE_OF_BLOCKS]),1
			);

	#ifdef _DEBUG_UPDATE_RECORDS_ON_
			_LOG::out_s << "chk_ = " << chk_ << std::endl; 	
			LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif		
		if (faBuffer->buff.md.chk != chk_)
		{
	#ifdef _DEBUG_CREATE_RECORDS_ON_	
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"CHECKSUM ERROR!");
	#endif	
			rd_stop();
			return;
		}

		switch (faBuffer->buff.md.typeOf)
		{
			case	UPDATE_RECORDS_COUNT12:
			{

	#if defined(_DEBUG_UPDATE_RECORDS_ON_) || defined(_DEBUG_CREATE_RECORDS_ON_)

					LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"UPDATE_RECORDS_COUNT12");
	#endif

				try
				{
					if (sqlInsert.update<bufferType>(faBuffer))
					{
	#ifdef _DEBUG_UPDATE_RECORDS_ON_			
						LOGTOFILE( LOGHTML::messageType::STRONG_WARNING,"record updated complete." );
	#endif				
						readHeader();
					}
				}
				catch(const std::exception& e)
				{
				//	_LOG::out_s << "exception detected : " << e.what();
				//	LOGTOFILE( LOGHTML::messageType::STRONG_WARNING,_LOG::out_s );
					rd_stop();
				}
			}
			break;
			
			case	UPDATE_RECORDS_COUNT6:
			{
	#if defined(_DEBUG_UPDATE_RECORDS_ON_) || defined(_DEBUG_CREATE_RECORDS_ON_)
				LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"UPDATE_RECORDS_COUNT6");
	#endif
				try
				{
					if (sqlInsert.update<bufferType>(faBuffer))
					{
	#ifdef _DEBUG_UPDATE_RECORDS_ON_	
						LOGTOFILE( LOGHTML::messageType::NOTICE,"record updated COMPLETE.");
	#endif					
					}
					else
					{
	#ifdef _DEBUG_UPDATE_RECORDS_ON_	
						LOGTOFILE( LOGHTML::messageType::STRONG_WARNING,"record updated UNCOMPLETE.");
	#endif					
					}
					readHeader();
				}
				catch(const std::bad_alloc& ba)
				{
					_LOG::out_s << "exception detected : " << ba.what();
					LOGTOFILE( LOGHTML::messageType::STRONG_WARNING,_LOG::out_s );
					rd_stop();
				} 
				catch(const std::exception& e)
				{
					_LOG::out_s << "exception detected : " << e.what();
					LOGTOFILE( LOGHTML::messageType::STRONG_WARNING,_LOG::out_s );
					rd_stop();
				}
			}
			break;

			case CREATE_NEW_RECORDS:
			{
	#if defined(_DEBUG_UPDATE_RECORDS_ON_) || defined(_DEBUG_CREATE_RECORDS_ON_)
				LOGTOFILE(LOGHTML::messageType::MESSAGE,"++CREATE_NEW_RECORDS");
	#endif
	#ifdef _DEBUG_CREATE_RECORDS_ON_
				
				LOGTOFILE(LOGHTML::messageType::MESSAGE,"bucket check complete!");
	#endif
	#ifdef __SERIALISER_ON__
				try
				{
					diskOperations::serialise<bufferType>(faBuffer);
				}
				catch(std::exception& re)
				{
					_LOG::out_s << "serialise exception = " << re.what()  << std::endl;
					LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
				}
	#endif	
				LAYER_QUERIES::layersHelper layerHelper;	
				if (!layerHelper.IsEmpty())
				{
					SQLCMD::types::ul_long layerHash = layerHelper.getFirstLayer< SQLCMD::types::ul_long >("layerHash");
				#ifdef _DEBUG_CREATE_RECORDS_ON_
					_LOG::out_s << " layerHash = " << layerHash << std::endl;
					LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
				#endif
					LOGTOFILE(LOGHTML::messageType::MESSAGE,"is not empty!");
					if (sqlInsert.exec<bufferType>(faBuffer.get(),layerHash))
					{
					#ifdef _DEBUG_CREATE_RECORDS_ON_	
						LOG(TRACE_STR());
					#endif	
						readBucket();
						return;
					}
				}
				else
				{
					LAYER_QUERIES::createBaseLayer(); 
				}
				readBucket();
				#ifdef _DEBUG_CREATE_RECORDS_ON_
				LOGTOFILE(LOGHTML::messageType::MESSAGE,"--CREATE_NEW_RECORDS");
				#endif		
			}
			break;
			
			case GET_RECORDS_FOR_CHECKING:
			{
	#if defined(_DEBUG_UPDATE_RECORDS_ON_) || defined(_DEBUG_CREATE_RECORDS_ON_)
				LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"GET_RECORDS_FOR_CHECKING");
	#endif
				readHeader();
			}
			break;

			case ADD_NEW_RECORD:
			{
				std::string fileGuid = std::string(faBuffer->buff.md.fileGuid);
				std::string sessionId = std::string(faBuffer->buff.md.sessionId);
				LAYER_QUERIES::layersHelper layerHelper(fileGuid);	
				layerHelper.setLayers();
				std::string layerGuid ="";
				SQLCMD::types::ul_long layerHash = 0;

	#if defined(_DEBUG_UPDATE_RECORDS_ON_) || defined(_DEBUG_CREATE_RECORDS_ON_)
				LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"++ADD_NEW_RECORD");
				_LOG::out_s << "sessionId " << sessionId  << std::endl;
				LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
	#endif
				const std::string RECORD_APPROWED = std::string("NEW_LAYER_APPROWED");
				bool IT_IS_FIRST_UPDATE = false; 
				bool APPROWED = !sessionId.compare(RECORD_APPROWED);
#ifdef _DEBUG_UPDATE_RECORDS_ON_
				_LOG::out_s << "APPROWED =" << APPROWED << "RECORD_APPROWED = " << RECORD_APPROWED << "sessionId = " << sessionId << std::endl;
				LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
#endif
				if ((layerHelper.IsEmpty()) && (APPROWED))
				//!layerByGuid.begin()->first.compare(DB::_QUERY_RESULT_IS_EMPTY))
				{
					IT_IS_FIRST_UPDATE = true;
					LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"RESULT_IS_EMPTY ... Refuse..");
					layerGuid = LAYER_QUERIES::createNewLayer();	
					layerHash = utils::minicache::str_hash(layerGuid) / 100000;
				}
				else
				{
					LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"RESULT_IS_NOT_EMPTY");
					layerHelper.refresh();	
					SQLCMD::types::ul_long lastId = layerHelper.getLastLayer< SQLCMD::types::ul_long >("layerId");
					layerHash = layerHelper.getlayerSomeThingRelationNameByName<SQLCMD::types::ul_long , SQLCMD::types::ul_long>("layerHash","layerId",lastId);	
					layerGuid = layerHelper.getlayerSomeThingRelationNameByName<SQLCMD::types::ul_long , std::string>("layerGuid","layerId",lastId); 					
				#ifdef _DEBUG_UPDATE_RECORDS_ON_
					_LOG::out_s << "fileGuid =" << fileGuid << "layerHash = " << layerHash << "lastId = " << lastId << "layerGuid =" << layerGuid << std::endl;
					LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
					LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"GETTING LAYER ATTRIBUTE PASSED..");
				#endif
				}

				#ifdef __SERIALISER_ON__
					try
					{
						diskOperations::serialise<bufferType>(faBuffer,layerGuid);//добавить layer.
					}
					catch(std::exception& re)
					{
						_LOG::out_s << "serialise exception = " << re.what()  << std::endl;
						LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
					}
				#endif	
	
				if (sqlInsert.exec<bufferType>(faBuffer.get(),layerHash))
				{
					// final transfer token detected.
					if (IT_IS_FIRST_UPDATE)
					{
					#ifdef _DEBUG_UPDATE_RECORDS_ON_
						LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"IT_IS_FIRST_UPDATE: try to flush");
					#endif
						sqlInsert.flushAnyWay();
					#ifdef _DEBUG_UPDATE_RECORDS_ON_
						LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"flush finished...");
					#endif	
					}
					std::cout << "** try to read Header **" << std::endl;
					readHeader();
				//	readBucket();
					return;
				}
				//LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,TRACE_STR());
				readBucket();
				return ;
			}//			case ADD_NEW_RECORD:
			break;
			default:
				LOGTOFILE(LOGHTML::messageType::MESSAGE,"INVALID_SELECTION!");
			break;	
		};

	#ifdef  _DEBUG_UPDATE_RECORDS_ON_
		LOGTOFILE(LOGHTML::messageType::MESSAGE,"--dispatch_action()");
	#endif
    }// void dispatch_action(
*/

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
mem_mgr tcp_connection::_mreader;
#endif