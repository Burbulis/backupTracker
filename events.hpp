//#include "layersIO.hpp"
#include "iocrud.hpp"
#include "IO/IOTRansactor.hpp"
#include "layers/layersHelper.h"

#ifndef DISPATCHER_EVENTS
#define DISPATCHER_EVENTS

namespace DISPATCHER
{
    namespace EVENTS
    {

#define DBG_CREATE_NEW_RECORD
        template	
        <
                typename ptrType,
                typename bufferType,
                typename sqlInsertType 
        >
        void createNewRecords(ptrType ptr,bufferType& faBuffer,sqlInsertType& sqlInsert)
        {
    #ifdef DBG_CREATE_NEW_RECORD
            BEGIN();
    #endif	
    #ifdef __SERIALISER_ON__
            try
            {
                diskOperations::stubSerialise(faBuffer, [](DA::ul_long var){
                    return (var == DA::FILLERS::FILESIZE_INCREASE || var == DA::FILLERS::FILLER_FILESIZE_CREATED);
                });
            }
            catch(std::exception& re)
            {
                _LOG::out_s << "serialise exception = " << re.what()  << std::endl;
                LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
            }
    #endif	
            Layers::layersHelper	layerHelper;
            if (!layerHelper.Empty())
            {
                auto layerHash = *layerHelper.getHashById( layerHelper.getFirstLayerId());
    #ifdef DBG_CREATE_NEW_RECORD
                _LOG::out_s << " faBuffer.blocks.size() = " << faBuffer.blocks.size() << std::endl;
                LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
                LOGTOFILE(LOGHTML::messageType::MESSAGE,"is not empty!");
    #endif

                if (
                    !sqlInsert.execInsert(faBuffer,layerHash,[](DA::ul_long var){return true;})
                )
                {
                    ptr->readBucket();
    #ifdef DBG_CREATE_NEW_RECORD
            END();
    #endif	
                    return;
                }
                else
                {
    #ifdef  DBG_CREATE_NEW_RECORD
                        LOGTOFILE(LOGHTML::messageType::MESSAGE,"FINALISED!");
    #endif	
                }
            }
            ptr->readHeader();
    #ifdef DBG_CREATE_NEW_RECORD
            END();
    #endif	
 
        }	


    #define DBG_ADD_NEW_RECORD
        template	
        <
                typename ptrType,
                typename bufferType,
                typename sqlInsertType 
        >
        void addNewRecord(ptrType ptr,bufferType& faBuffer,sqlInsertType& sqlInsert)
        {
                std::string fileGuid = std::string(faBuffer.fileGuid);
                std::string sessionId = std::string(faBuffer.sessionId);
      #ifdef DBG_ADD_NEW_RECORD
         BEGIN();
        _LOG::out_s << "fileGuid = " << fileGuid << "sessionId " << sessionId  << " faBuffer.blocks.size() = " << faBuffer.blocks.size() << std::endl;
        LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
    #endif
            Layers::layersHelper layerHelper(fileGuid);
            std::string layerGuid ="";
            SQLCMD::types::ul_long layerHash = 0;
            const std::string RECORD_APPROWED = std::string("NEW_LAYER_APPROWED");
            bool IT_IS_FIRST_UPDATE = false; 
            bool APPROWED = !sessionId.compare(RECORD_APPROWED);
    #ifdef DBG_ADD_NEW_RECORD
        _LOG::out_s << "APPROWED =" << APPROWED << "RECORD_APPROWED = " << RECORD_APPROWED << "sessionId = " << sessionId << std::endl;
        LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
    #endif
                if ((layerHelper.Empty()) && (APPROWED))
                {
                    IT_IS_FIRST_UPDATE = true;
                    LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"RESULT_IS_EMPTY ... Refuse..");
                    layerGuid = layersAPI::createNewLayer();	
                    layerHash = utils::minicache::str_hash(layerGuid) / 100000;
                }
                else
                {
                    LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"RESULT_IS_NOT_EMPTY");
                    layerHelper.resetLayers();	
                    SQLCMD::types::ul_long lastId = layerHelper.getLastLayerId();
                    layerGuid = *layerHelper.getGuidById(lastId);
                    //getlayerSomeThingRelationNameByName<SQLCMD::types::ul_long , std::string>("layerGuid","layerId",lastId); 					
                    layerHash = *layerHelper.getHashById(lastId);
                    //getlayerSomeThingRelationNameByName<SQLCMD::types::ul_long , SQLCMD::types::ul_long>("layerHash","layerId",lastId);	
    #ifdef DBG_ADD_NEW_RECORD
    _LOG::out_s << "fileGuid =" << fileGuid << "layerHash = " << layerHash << "lastId = " << lastId << "layerGuid =" << layerGuid << std::endl;
    LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
    LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"GETTING LAYER ATTRIBUTE PASSED..");
    #endif
                }
            #ifdef __SERIALISER_ON__
                try
                {
                    diskOperations::stubSerialise(faBuffer,[](DA::ul_long var){
                    return (var == DA::FILLERS::FILESIZE_INCREASE || var == DA::FILLERS::FILLER_FILESIZE_CREATED || DA::FILLERS::FILLER_FILESIZE_REPLACEMENT);
                },layerGuid);
                }
                catch(std::exception& re)
                {
                    _LOG::out_s << "serialise exception = " << re.what()  << std::endl;
                    LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
                }
            #endif	
        
                if (sqlInsert.execInsert(faBuffer,layerHash,[](DA::ul_long var){
                       return (var == DA::FILLERS::FILESIZE_INCREASE || var == DA::FILLERS::FILLER_FILESIZE_CREATED);
                }))
                {
                    // final transfer token detected.
                    if (IT_IS_FIRST_UPDATE)
                    {
    #ifdef DBG_ADD_NEW_RECORD
    LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"IT_IS_FIRST_UPDATE: try to flush");
    #endif
                            sqlInsert.flushAnyWay();
                #ifdef DBG_ADD_NEW_RECORD
                    LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"flush finished...");
                #endif	
                    }
                    ptr->readHeader();
                    return;
                }
                ptr->readBucket();
               // ptr->readHeader();
               return ;

        }	


    }
}
#endif