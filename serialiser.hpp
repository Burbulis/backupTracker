//#define __BOOST_SERIALISER_ON__
#ifndef __SERIALISER__
#define __SERIALISER__
#define _DEBUG_SERIALISER_ON_
#include"base.h"

#include <boost/filesystem.hpp>
#include "dbquery.hpp"
#include "data_and_structs.hpp"
#include <iostream>
#include <fstream>
#include "_log.hpp"
#include "DA_.hpp"
#ifdef __BOOST_SERIALISER_ON__
#include <boost/archive/detail/register_archive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#endif
#include <sys/stat.h>

namespace diskOperations
{

struct  existFsObject
{
	existFsObject(std::string _fsObj):fsObj(_fsObj){}

	bool exist(void)
	{
		boost::filesystem::path p(fsObj);
		return (boost::filesystem::exists(p));
	}

private:
std::string fsObj;

};


struct Serialiser
{
    std::string fileName;
#ifndef  __BOOST_SERIALISER_ON__
    FILE *f;
#endif   
    Serialiser(const std::string& _fileName):f(NULL),fileName(_fileName) 
    {
    }

    bool operator()(std::vector<char>& buffer)
    {

        _LOG::out_s << "fileName = " << fileName << " buffer size()==" << buffer.size() <<  std::endl;
         //LOGTOFILE(_LOG::out_s);
       	LOGTOFILE(LOGHTML::messageType::NOTICE,_LOG::out_s);
#ifdef __BOOST_SERIALISER_ON__
        LOG("BOOST serialise method...");
        std::ofstream ofs(this->fileName, std::ios::out | std::ios::binary);
        boost::archive::binary_oarchive oa(ofs);
        oa << buffer;
        ofs.close();
#else
        LOGTOFILE(LOGHTML::messageType::NOTICE,"my serialise method...");
        f= fopen(fileName.c_str(),"wb+");
        if (NULL == f)
		{
         	LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"Serialiser()() -> (f) is null...error");
			_LOG::out_s << "can't open file : " << fileName << std::endl;
         	LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
			throw std::runtime_error(_LOG::out_s.str());
		    return (false);
		} 
	      LOGTOFILE(LOGHTML::messageType::NOTICE,"my serialise method...");

       size_t writed = fwrite(&buffer[0],1,buffer.size(),f);
	   _LOG::out_s << "writed =" << writed << " buffer.size() =" << buffer.size();
    	LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);


#endif  
             
        return (true);
    }


    ~Serialiser()
    {

#ifndef __BOOST_SERIALISER_ON__         
       
	    LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"~Serialiser() ->try to close..");
	    if (NULL != f) 
		{
		    LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"~Serialiser() -> (f) is not null...");
            fclose(f);
		}
		LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"~Serialiser() ->serialisation end.");
#endif        
    }

};
//
//DA::mainDescNorm faBuffer
//
//

//	template
//	<
//		typename bufferType
//	>
	#define STUB_SERIALISE

    static
	void
	stubSerialise(const DA::mainDescNorm& faBuffer , std::string layerGuid = std::string(), std::vector<std::string> approwedBlocks = std::vector<std::string>())
	{
		enum{FIRST_ELEMENT=0};
		layerGuid = (layerGuid.empty())?LAYER_QUERIES::getBaseLayer()["layerGuid"][0]:layerGuid;
		std::string layerFolder = utils::readJson("config.json","pathToStore")+"//" + layerGuid;
	#ifdef STUB_SERIALISE
		_LOG::out_s << "_FOLDERNAME_  = " << layerFolder << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
	#endif
		existFsObject fo(layerFolder);
		if (!fo.exist())
		{
			int mk_res = mkdir(layerFolder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			std::string const ErrorMessage = "path not exist ,can't create directory -" + layerFolder;
			if (mk_res)
				throw std::runtime_error( ErrorMessage );
		}
		const size_t counter = faBuffer.blocks.size();
	
		for (size_t i = 0 ;	i < counter ; ++i)
		{	
			std::string blockGuid = std::string(faBuffer.blocks[i].blockGuid);

			if ((approwedBlocks.empty())||(std::find(approwedBlocks.begin(),approwedBlocks.end(),blockGuid)!=approwedBlocks.end()))
			{
				std::string  fileName = layerFolder +"//"+ blockGuid; 
				#ifdef STUB_SERIALISE	
				_LOG::out_s << "APPROWED -> J =(" << i << ") blockGuid::[" << blockGuid <<"]"<< " begin = " << faBuffer.blocks[i].begin << " status" << faBuffer.blocks[i].status 
					<< " end = " << faBuffer.blocks[i].end <<std::endl;
				LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);				
				#endif
				//serialise if it is real block (chk == 0 if it block Filler)
				if (
					((faBuffer.blocks[i].status != FILLERS::FILESIZE_INCREASE) && (faBuffer.blocks[i].status != FILLERS::FILESIZE_REDUCTION))
				)
				{
				#ifdef STUB_SERIALISE	
					_LOG::out_s << "_FILENAME_  = " << fileName << " size = " << faBuffer.blocks[i].buffer.size() << std::endl;
					LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
				#endif
					Serialiser s(fileName);
					std::vector<char> adapter(faBuffer.blocks[i].buffer.begin(),
						faBuffer.blocks[i].buffer.end());
					s(adapter);
				}
			}
        }
	}
	/// @brief сериализация блоков.
	/// @tparam bufferType -тип для буффера с данными для unique_ptr который содержит в себе буффер с элементами для сериализации 
	/// @param faBuffer 
	template
	<
		typename bufferType
	>
    static
	void
	serialise(const std::unique_ptr<bufferType>& faBuffer , std::string layerGuid = std::string(), std::vector<std::string> approwedBlocks = std::vector<std::string>())
	{
		enum{FIRST_ELEMENT=0};
		layerGuid = (layerGuid.empty())?LAYER_QUERIES::getBaseLayer()["layerGuid"][0]:layerGuid;
		std::string layerFolder = utils::readJson("config.json","pathToStore")+"//" + layerGuid;
		_LOG::out_s << "_FOLDERNAME_  = " << layerFolder << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
		existFsObject fo(layerFolder);
		if (!fo.exist())
		{
			int mk_res = mkdir(layerFolder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			std::string const ErrorMessage = "path not exist ,can't create directory -" + layerFolder;
			if (mk_res)
				throw std::runtime_error( ErrorMessage );
		}
		const size_t counter = utils::getCountOf(faBuffer.get());
		for (size_t j = 0 ;	j < counter ; ++j)
		{	
			std::string blockGuid = std::string(faBuffer->buff.t[j].blockGuid);

			if ((approwedBlocks.empty())||(std::find(approwedBlocks.begin(),approwedBlocks.end(),blockGuid)!=approwedBlocks.end()))
			{
				std::string  fileName = layerFolder +"//"+ blockGuid; 
				#ifdef _DEBUG_SERIALISER_ON_	
				_LOG::out_s << "APPROWED -> J =(" << j << ") blockGuid::[" << blockGuid <<"]"<< "begin = " << faBuffer->buff.t[j].begin << "faBuffer->buff.t[].bucket" << faBuffer->buff.t[j].bucket << " end = " << faBuffer->buff.t[j].end <<std::endl;
				LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);				
				#endif
				//serialise if it is real block (chk == 0 if it block Filler)
				if (
					((faBuffer->buff.t[j].bucket != FILLERS::FILESIZE_INCREASE) && (faBuffer->buff.t[j].bucket != FILLERS::FILESIZE_REDUCTION))
				)
				{
				#ifdef _DEBUG_SERIALISER_ON_	
					_LOG::out_s << "_FILENAME_  = " << fileName << " size = " << faBuffer->buff.t[j].size << std::endl;
					LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
				#endif
					Serialiser s(fileName);
					std::vector<char> adapter(&faBuffer->buff.t[j].buffer[0],&faBuffer->buff.t[j].buffer[faBuffer->buff.t[j].size]);
					s(adapter);
				}
			}
        }
	}
}


#endif