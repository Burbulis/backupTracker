#ifndef __CONFIG__
#define __CONFIG__

//#define BOOST_JSON_HEADER_ONLY
#include <boost/json.hpp>
#include <string>
#include <fstream>


class Config
{
    std::string jsonFileName;
    private:
    std::string readJson(std::string jsonfile,std::string key);
    public:
    Config(std::string jsonfile):jsonFileName(jsonfile){}
    std::string read(std::string key);
};

#endif