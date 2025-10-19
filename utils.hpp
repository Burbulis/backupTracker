#include <string>
#include <set>
#include <boost/json/src.hpp>
#include "data_and_structs.hpp"
#include "dbquery.hpp"
#include "sql/sqlcmdType.hpp"

#ifndef __UTILS__
#define __UTILS__
namespace utils
{

  const std::vector<uint8_t>
	set_size(const std::vector<uint8_t> buffer,size_t newSize)
	{
		std::vector<uint8_t> Tmp;
		Tmp.resize(newSize);
		memset(&Tmp[0], 0, Tmp.size());
		memmove(&Tmp[0], buffer.data(), buffer.size());
		return Tmp;
	}
	namespace time
	{
		std::string
		toTimeString(time_t _time)
		{
			char tmp[50];
			struct tm local_tm  = *localtime(&_time);
			sprintf(tmp,"%02d-%02d-%02d %02d:%02d:%02d",  1900 + local_tm.tm_year,local_tm.tm_mon,local_tm.tm_mday, local_tm.tm_hour, local_tm.tm_min,local_tm.tm_sec );
			std::string str = tmp;
			return str;
		}
	}
    namespace minicache
    {
        struct Minicache
        {
        static std::set<size_t> _hashes;
            bool exist(size_t h)
            {
                if (!_hashes.count(h))
                {
                    _hashes.insert(h);
                    return (false);
                }
                return (true);
            }
            
        };

        types::ul_long
        str_hash(std::string str)
        {
            types::ul_long ret = 1;
            size_t l = str.length();
            for (size_t i =0 , j = l ; i < j ;++i,j--)
            ret += ret * (((str[i]*i) ^ j) ^ ((str[j]*j) ^ i));  
            return (ret);
        }

        types::ul_long
        str_hash(std::string str ,unsigned long idx)
        {
            types::ul_long ret = 1;
            size_t l = str.length();
            for (size_t i =0 , j = l ; i < j ;++i,j--)
                ret += idx ^ (((str[i]^i) ^ j) ^ ((str[j]^j) ^ i));  
            return (ret);
        }

        std::set<size_t> Minicache::_hashes;
    }


	template
	<
		typename bufferType
	>
  	static 
	bool isUpdateFinal(bufferType *faBuffer)
	{
		const size_t counter = bufferType::SIZE_OF_BLOCKS / sizeof(typename bufferType::CURRENT_TYPE);
		for (size_t i =0 ; i < counter ; ++i)
		{
			if (0 == std::string(faBuffer->buff.t[i].end).compare("_FINA"))
				return (true);
		}
		return (false);
	}

	template
	<
		typename bufferType
	>
  	static 
	size_t getCountOf(bufferType *faBuffer)
	{
		const size_t counter = bufferType::SIZE_OF_BLOCKS / sizeof(typename bufferType::CURRENT_TYPE);
		for (size_t i =0 ; i < counter ; ++i)
			if (0 == std::string(faBuffer->buff.t[i].end).compare("_END"))
				return ( (++i <= counter)? i : counter );
		return (counter);
	}
  
/*
	static 
	bool isFinal(bufferType *faBuffer)
	{
		for (size_t i =0 ; i < counter ; ++i)
		{
			if (0 == std::string(faBuffer->buff.t[i].end).compare("_END"))
				return (true);
		}
		return (false);
	}
*/
	template
	<
		typename bufferType
	>
	static bool isFiller(typename bufferType::CURRENT_TYPE& val)
	{
		return 	(
					(val.status == FILLERS::FILESIZE_REDUCTION)
				);

	}

	template
	<
		typename bufferType
	>
	static bool isFiller(const bufferType& val)
	{
		return 	(

					(val.status == FILLERS::FILESIZE_REDUCTION)||
					(val.status == FILLERS::FILLER_FILESIZE_REPLACEMENT)||
					(val.status == FILLERS::FILLER_FILESIZE_NO_CHANGED)
				//	&&	!_blockGuid.compare(blockGuid)
				);

	}

	template
	<
		typename bufferType
	>
	static bool isFiller(bufferType faBuffer,std::string blockGuid)
	{
		const size_t counter = faBuffer.blocks.size();	
	//	typename bufferType::CURRENT_TYPE *begin =  reinterpret_cast<typename bufferType::CURRENT_TYPE *>(&faBuffer->buff.t[0]);
//		typename bufferType::CURRENT_TYPE *end =  reinterpret_cast<typename bufferType::CURRENT_TYPE *>(&faBuffer->buff.t[counter]);
		auto It = 
		std::find_if(faBuffer.blocks.begin(),faBuffer.blocks.end(),[blockGuid](typename bufferType::blocksType::value_type& val)
			{
				std::string _blockGuid = val.blockGuid; 
				return 
				(
			//	(val.bucket == FILLERS::FILESIZE_INCREASE)||
				((val.status == FILLERS::FILESIZE_REDUCTION) && !_blockGuid.compare(blockGuid))
			//		(val.bucket == FILLERS::FILLER_FILESIZE_REPLACEMENT)||
			//		(val.bucket == FILLERS::FILLER_FILESIZE_NO_CHANGED))&&
					
				);
			});
		return (It != faBuffer.blocks.end());	
	}

/*
	template
	<
		typename bufferType
	>
	static bool isFiller(bufferType *faBuffer,std::string blockGuid)
	{
		const size_t counter = utils::getCountOf(faBuffer);	
		typename bufferType::CURRENT_TYPE *begin =  reinterpret_cast<typename bufferType::CURRENT_TYPE *>(&faBuffer->buff.t[0]);
		typename bufferType::CURRENT_TYPE *end =  reinterpret_cast<typename bufferType::CURRENT_TYPE *>(&faBuffer->buff.t[counter]);
		auto It = 
		std::find_if(reinterpret_cast<typename bufferType::CURRENT_TYPE *>(begin),reinterpret_cast<typename bufferType::CURRENT_TYPE *>(end),[blockGuid](const typename bufferType::CURRENT_TYPE& val)
			{
				std::string _blockGuid = val.blockGuid; 
				return 
				((
			//	(val.bucket == FILLERS::FILESIZE_INCREASE)||
					(val.bucket == FILLERS::FILESIZE_REDUCTION)||
					(val.bucket == FILLERS::FILLER_FILESIZE_REPLACEMENT)||
					(val.bucket == FILLERS::FILLER_FILESIZE_NO_CHANGED))&&
					!_blockGuid.compare(blockGuid)
				);
			}		);
		return (It!= end);	
	}
*/
	std::string readJson(std::string jsonfile,std::string key)
	{
		char tmp[100];
		std::string test;
		memset(tmp, 0, 100);
		std::ifstream readfile(jsonfile);
		boost::json::value _j;
		while (readfile.getline(tmp, 100))
		{
			std::string json;     
			auto count_ = strlen(tmp);
			std::copy(&tmp[0], &tmp[count_], std::back_inserter(json));
			try
			{
				_j = boost::json::parse(json);
			}
			catch (const std::exception& e)
			{
				continue;
			}
			std::string value = _j.at(key).as_string().c_str();
			return (value);
			std::cout << value << std::endl;
		}
		return (std::string());
	}

}
#endif