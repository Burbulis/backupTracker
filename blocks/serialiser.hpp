//#define __BOOST_SERIALISER_ON__
#ifndef __SERIALISER__
#define __SERIALISER__
#define _DEBUG_SERIALISER_ON_
#include "../base.h"

#include <boost/filesystem.hpp>
#include "dbqueries.hpp"
#include "../config/config.h"
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include "../log/_log.hpp"
#include "../DA/DA_.hpp"
#include "../config/config.h"
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

	#define STUB_SERIALISE
    static
	void
	stubSerialise(const DA::mainDescNorm& faBuffer ,std::function<bool (typename DA::ul_long)> statusChecker, std::string layerGuid = std::string())
	 // std::vector<std::string> approwedBlocks = std::vector<std::string>())
    {
	#ifdef STUB_SERIALISE
		BEGIN()
	#endif
		enum{FIRST_ELEMENT=0};
		layerGuid = (layerGuid.empty())?LAYER_QUERIES::getBaseLayer()["layerGuid"][0]:layerGuid;
		Config conf("config.json");
		std::string layerFolder = conf.read("pathToStore")+ "//" + layerGuid;//CONFIG::readJson(,"pathToStore")+"//" + layerGuid;
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
			if (!statusChecker(faBuffer.blocks[i].status))
				continue;	
			std::string  fileName = layerFolder +"//"+ blockGuid; 
			#ifdef STUB_SERIALISE	
			_LOG::out_s << "APPROWED -> J =(" << i << ") blockGuid::[" << blockGuid <<"]"<< " begin = " << faBuffer.blocks[i].begin << " status" << faBuffer.blocks[i].status 
				<< " end = " << faBuffer.blocks[i].end <<std::endl;
			LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);				
			#endif
			if(faBuffer.blocks[i].status != DA::FILLERS::FILESIZE_REDUCTION)
			{
			#ifdef STUB_SERIALISE	
				_LOG::out_s << "_FILENAME_  = " << fileName << " size = " << faBuffer.blocks[i].buffer.size() << std::endl;
				LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
			#endif
				std::vector<char> buffer(faBuffer.blocks[i].buffer.begin(),
					faBuffer.blocks[i].buffer.end());
				std::unique_ptr<FILE, decltype(&fclose)> file(fopen(fileName.c_str(),"wb+"),fclose);
				if (file)
				{
					fwrite(&buffer[0],1,buffer.size(),file.get());
				}
			}
        }
	}

}


#endif