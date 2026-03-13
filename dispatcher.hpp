#include "layers/layersIO.hpp"
#include "iocrud.hpp"
#include "IOTRansactor.hpp"
//#include "layersHelper.hpp"
#include "events.hpp"
//#include "DA_.hpp"

#define __SERIALISER_ON__


#ifndef STUB_DISPATCH_ACTION
#define STUB_DISPATCH_ACTION

	namespace DISPATCHER
	{

	void Update()
	{

		
	}

	template
		<
			typename ptrType,
			typename bufferType,
			//IO_CRUD::sqlInsert
			typename sqlInsertType, 
			//boost::asio::ip::tcp::socket
			typename socketType
		>
		void stub_dispatch_action(
			socketType& _socket,
			ptrType ptr,
			sqlInsertType& sqlInsert,
			bufferType& faBuffer
		)
		{
		#ifdef  STUB_DISPATCH_ACTION
			BEGIN();
			_LOG::out_s << "  faBuffertypeOf = " << faBuffer.typeOf  << " faBuffer.blocks.size() = " << faBuffer.blocks.size() <<  std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
		#endif
			if (ptr->is_complete())
			{
		#ifdef  STUB_DISPATCH_ACTION
		END();
		#endif
				return;
			}

		switch (faBuffer.typeOf)
				{
					case DA::CREATE_NEW_RECORDS:
					{
						EVENTS::createNewRecords(ptr,faBuffer,sqlInsert);
					}//CREATE_NEW_RECORDS:
					break;
					case	DA::UPDATE_RECORDS_COUNT12:
					{

		#ifdef STUB_DISPATCH_ACTION
						LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"UPDATE_RECORDS_COUNT");
		#endif
						if (faBuffer.blocks.empty())
						{
							ptr->readBucket();
							break;
						}
						try
						{
							if (sqlInsert.update(faBuffer))
							{
		#ifdef STUB_DISPATCH_ACTION			
						LOGTOFILE( LOGHTML::messageType::STRONG_WARNING,"record updated complete." );
		#endif				
							}
							ptr->readHeader();
						}
						catch(const std::exception& e)
						{
						//	_LOG::out_s << "exception detected : " << e.what();
						//	LOGTOFILE( LOGHTML::messageType::STRONG_WARNING,_LOG::out_s );
							ptr->rd_stop();
						}
					} //UPDATE_RECORDS_COUNT12:
					break;
					case DA::QUERY_FILES_AND_PATHS:
					{
						IoTransactor::queryFilesAndPaths<boost::asio::ip::tcp::socket>(_socket);	
						ptr->readHeader();
					} // QUERY_FILES_AND_PATHS:
					break;

					case DA::GET_RECORDS_FOR_CHECKING:
					{
		#ifdef STUB_DISPATCH_ACTION
			LOGTOFILE(LOGHTML::messageType::WARNING,"GET_INFO_RECORDS");
		#endif
						IoTransactor::queryMainData<boost::asio::ip::tcp::socket,DA::mainDescNorm>(_socket , faBuffer);					
						ptr->readHeader();
					}//GET_RECORDS_FOR_CHECKING:
					break;

					case DA::ADD_NEW_RECORD:
					{
		#ifdef STUB_DISPATCH_ACTION
			LOGTOFILE(LOGHTML::messageType::WARNING,"GET_INFO_RECORDS");
		#endif				
						EVENTS::addNewRecord(ptr,faBuffer,sqlInsert);
					}//			case ADD_NEW_RECORD:
					break;
					
		#define DBG_QUERY_LAYERS
					case DA::QUERY_LAYERS:
					{
		#ifdef DBG_QUERY_LAYERS
			LOGTOFILE(LOGHTML::messageType::WARNING,"GET_LAYERS_BY_FILEGUID:");
		#endif
						IoTransactor::LayersByGuidAnswer(faBuffer.fileGuid);
					} //QUERY_LAYERS:
					break;
				}		
		#ifdef  STUB_DISPATCH_ACTION
			END();
		#endif
		}
	}
#endif