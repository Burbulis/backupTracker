#include "config.h"


	std::string Config::readJson(std::string jsonfile,std::string key)
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
		}
		return (std::string());
	}

	std::string Config::read(std::string key)
	{
		return (readJson(jsonFileName,key));	
	}