	#include "_log.hpp"

    std::ostringstream _LOG::out_s;

    
	std::string 
    _LOG::timeStamp()
	{
		std::ostringstream strs;
		auto t = std::time(nullptr);
		//tm time;
		auto time = std::localtime(&t);//err = localtime(&time ,&t);
		strs << "|" << std::put_time(time, "%d-%m-%Y %H-%M-%S") << "|";
    	return (strs.str());
	}

	_LOG::_LOG(
		std::string message,
		std::string file,
        int line
		)
	{
		std::ostringstream xlog;
		xlog << timeStamp() <<" file:" << file << " line:" <<  line <<" ["<< message <<"]" <<std::endl;
		std::cout << xlog.str();
	} 

	_LOG::_LOG(
		std::ostringstream& message,
		std::string file,
		int line
		)
	{
		std::ostringstream log;
		log << timeStamp() << " file:" << file << " line:" << line <<" ["<< message.str() <<"]" <<std::endl;
		std::cout << log.str();
		message.str("");
	}
/*
	_LOG::_LOG(
		std::ostringstream& message,
		std::string file,
		int line,
		std::string toFile
		)
	{
		std::ostringstream log;
		log << timeStamp() << " file:" << file << " line:" << line <<" ["<< message.str() <<"]" <<std::endl;
		std::ofstream write(toFile,std::ios::app);
		write << log.str();
		message.str("");
	}*/
/*	_LOG::_LOG(
		std::string message,
		std::string file,
        int line,
		std::string toFile
		)
	{
		std::ostringstream xlog;
		xlog << " file:" << file << " line:" <<  line <<" ["<< message <<"]" <<std::endl;
		std::ofstream write(toFile,std::ios::app);
		write << xlog.str();
	} 
*/
	_LOG::~_LOG()
	{
		out_s.str("");
	}