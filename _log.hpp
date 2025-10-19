
#ifndef _LOG_

#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <map>
#include <vector>
#include "dbgMetrics.h"
#define _LOG_

struct _LOG
{
  static std::string timeStamp();

	_LOG(
		std::string message,
		std::string file,
        int line
		);

	_LOG(
		std::ostringstream& message,
		std::string file,
		int line
		);

	_LOG(
		std::string message,
	//	std::string file,
    //    int line,
		std::string toFile
		)
	{
		std::ostringstream log;
		log << _LOG::timeStamp() << "[" << message << "]" << std::endl; 
	//	std::ofstream write(toFile,std::ios::app);
	//	write << log.str();

	}




	~_LOG();
	static	std::ostringstream out_s;
	private:
	void to_file()
	{

	}
};

struct LOGHTML
{
	enum messageType
	{
		MESSAGE,
		NOTICE,
		WARNING,
		STRONG_WARNING
	};

	std::string
	switchColor(messageType type)
	{
		std::string color="<font color =";
		switch(type)
		{
			case MESSAGE:
				color += "'black'>";
			break;	
			case NOTICE:
				color += "'#FF00FF'>";
			break;
			case WARNING:
				color += "'#0000FF'>";
			break;
			case STRONG_WARNING:
				color += "'red'>";
			break;
			default:
				color += "'black'>";
		}
		return ( color );
	}	

	static void Init(std::string fileName)
	{
		FILE *f = fopen(fileName.c_str(),"wt+");
		fputs("<html><body>",f);
		fclose(f);
	}

	static void Destroy(std::string fileName)
	{
		FILE *f = fopen(fileName.c_str(),"at+");
		if (NULL == f)
		{
			throw std::runtime_error("error of log close");	
		}
		fputs("</html></body>",f);
		fclose(f);
	}

	LOGHTML(messageType type,
		std::string message,
		std::string file,
        int line,
		std::string toFile
		)
	{
		
		std::string _str = switchColor(type); 
		std::ostringstream xlog;
		xlog << _str << _LOG::timeStamp() << " file:" << file << " line:" <<  line << " ["<< message <<"]" << "</p>" << std::endl; 
		std::ofstream write(toFile,std::ios::app);
		write << xlog.str();
	}

	LOGHTML(
		messageType type,
		std::ostringstream& message,
		std::string file,
		int line,
		std::string toFile
		)
	{
		std::string _str = switchColor(type); 
		std::ostringstream log;
		log << _str << _LOG::timeStamp() << " file:" << file << " line:" <<  line << " [" << message.str() << " ] </p>" << std::endl; 
		std::ofstream write(toFile,std::ios::app);
		write << log.str();
		message.str("");
	}

//	template
//	<
//		typename T
//	>
	static
	std::ostringstream
	showSeq(const std::vector< std::string >&  seq)
	{

		std::ostringstream log;
		log << "**size of seq = " << seq.size() << std::endl; 
		for (size_t i = 0; i < seq.size() ; ++i)
			log <<	seq[i]  << std::endl;
		return (log);
	}


};



#define LOG(x) _LOG(x,std::string("console.log"));
//#define LOGTOFILE(x) _LOG(x,__FILE__,__LINE__,"console.log");
#define LOGINIT() LOGHTML::Init("log.html");
#define LOGSTOP() LOGHTML::Destroy("log.html");
#define BEGIN() LOGHTML(LOGHTML::messageType::MESSAGE,std::string("++") += std::string(&__FUNCTION__[0],&__FUNCTION__[strlen(__FUNCTION__)]),__FILE__,__LINE__,"log.html");
#define END()   LOGHTML(LOGHTML::messageType::MESSAGE,std::string("--") += std::string(&__FUNCTION__[0],&__FUNCTION__[strlen(__FUNCTION__)]),__FILE__,__LINE__,"log.html");
#define LOGTOFILE(TYPE,x) LOGHTML(TYPE,x,__FILE__,__LINE__,"log.html");
#define TRACE() dbgMetrics METRICS(__LINE__,__FILE__,__func__);
#define TRACE_ARG(x) dbgMetrics METRICS(__LINE__,__FILE__,__func__,x);
#define TRACE_STR() dbgMetrics::check()

#endif
