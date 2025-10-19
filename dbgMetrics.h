#ifndef __METRICS__
#define __METRICS__

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

class dbgMetrics
{
    std::string current;
    static  std::map<std::string, size_t> m;
    static  std::vector<std::string> fileFunc;
    static  size_t totalCount;
public:
	dbgMetrics(size_t line , std::string file ,std::string function);
	dbgMetrics(size_t line , std::string file ,std::string function, std::string arg);
	dbgMetrics(size_t line , std::string file ,std::string function, size_t arg);
 	static std::string check(void);
	~dbgMetrics();
    private:
    void std_message_gen(size_t line , std::string file ,std::string function,std::string arg = "NONE");
};
#endif