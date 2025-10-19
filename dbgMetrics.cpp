#include "dbgMetrics.h"
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>
//#include <mutex>

std::map<std::string, size_t> dbgMetrics::m;
std::vector<std::string> dbgMetrics::fileFunc;
size_t dbgMetrics::totalCount;
//std::mutex dbgMetrics::_lock;


void 
dbgMetrics::std_message_gen(size_t line , std::string file ,std::string function,std::string arg)
{
    current = " ( " + std::to_string(totalCount)+" ) " + file + " // " + function + "//" + arg; 
    if (m.count(current) == 0)
        fileFunc.push_back(current);
    ++m[current];
    ++totalCount;  
}

dbgMetrics::dbgMetrics(size_t line , std::string file ,std::string function)
{
    std_message_gen(line,file,function);
}

dbgMetrics::dbgMetrics(size_t line , std::string file ,std::string function,std::string arg)
{
    std_message_gen(line,file,function,arg);
}

dbgMetrics::dbgMetrics(size_t line , std::string file ,std::string function,size_t arg)
{
    std_message_gen(line,file,function, std::to_string(arg) );
}

   std::string 
   dbgMetrics::check(void)
   {
       std::ostringstream str_s;
       for (auto i : fileFunc)
           str_s << "METRICA fileFunc = [" << i << "] = " <<  ((m[i]==0)?"closed": ("call count = " + std::to_string(m[i]))) << std::endl;
       return (str_s.str());
   }

    dbgMetrics::~dbgMetrics()
    {
        --m[current];
    }
