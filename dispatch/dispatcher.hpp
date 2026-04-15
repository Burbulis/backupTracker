#include "layers/layersIO.hpp"
#include "iocrud.hpp"
#include "IO/IOTRansactor.hpp"
#include "log/_log.hpp"
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
	#define DISPATCH_ACTIONS_DBG
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
		#ifdef  DISPATCH_ACTIONS_DBG
			BEGIN();
			_LOG::out_s <<"  faBuffer.layerGuid = " << faBuffer.layerGuid << "  faBuffer.typeOf = " << faBuffer.typeOf  << " faBuffer.blocks.size() = " << faBuffer.blocks.size() <<  std::endl;
			LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
		#endif
			if (ptr->is_complete())
			{
		#ifdef  DISPATCH_ACTIONS_DBG
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

		#ifdef DISPATCH_ACTIONS_DBG
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
		#ifdef DISPATCH_ACTIONS_DBG			
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
		#ifdef DISPATCH_ACTIONS_DBG
			LOGTOFILE(LOGHTML::messageType::WARNING,"GET_INFO_RECORDS");
		#endif
						//faBuffer	
						IoTransactor::queryMainData<boost::asio::ip::tcp::socket,DA::mainDescNorm>(_socket , faBuffer);					
						ptr->readHeader();
					}//GET_RECORDS_FOR_CHECKING:
					break;

					case DA::ADD_NEW_RECORD:
					{
		#ifdef DISPATCH_ACTIONS_DBG
			LOGTOFILE(LOGHTML::messageType::WARNING,"ADD_NEW_RECORD");
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
						IoTransactor::LayersByGuidToSend<boost::asio::ip::tcp::socket>(_socket,faBuffer.fileGuid); //LayersByGuidAnswer(faBuffer.fileGuid);
						ptr->readHeader();
					} //QUERY_LAYERS:
					break;
				}		
		#ifdef  STUB_DISPATCH_ACTION
			END();
		#endif
		}
	}
#endif