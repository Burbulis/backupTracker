#ifndef _EXCEPTION_SQL_
#define _EXCEPTION_SQL_
#include <iostream>
#include <sstream>
struct SQL_THROW
{

	SQL_THROW(
		std::string message,
		std::string file,
		int line
		)
	{
		std::ostringstream xlog;
		xlog << " file:" << file << " line:" << line <<"["<< message <<"]" <<std::endl;
		std::cout << xlog.str() << std::endl;
		throw std::runtime_error(xlog.str());
	} 
/*	SQL_THROW(
		std::ostringstream& message,
		std::string file,
		int line
		)
	{
		std::ostringstream log;
		log << " file:" << file << " line:" << line <<" ["<< message.str() <<"]" <<std::endl;
		throw std::runtime_error(log.str());
		message.str("");
	}
*/
	~SQL_THROW()
	{
		//out_s.str("");
	}
	//private:
	//static std::ostringstream out_s;
};
//std::ostringstream SQL_THROW::out_s;
#define  THROW(x) SQL_THROW(x,__FILE__,__LINE__)
#endif
