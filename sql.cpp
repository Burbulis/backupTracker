#ifndef _HEADERS_
#define _HEADERS_
#include <csignal>
#include "tcpconnection.hpp"



struct __io
{
    boost::asio::io_service io;
    boost::asio::io_service& get(){return io;} 
};

class server:public __io
{
	tcp_connection::ptr connector;
	boost::asio::ip::tcp::acceptor acceptor;
	public:
    server(size_t port):acceptor(get(),
	boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),port))
    {
		_LOG::out_s << "typeWithBuffer::TBUFF_SIZE =" << bufferType_12::TBUFF_SIZE << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
		connector = tcp_connection::create<headerType>(get());
		_LOG::out_s << "run_one::connector->complete()==" << tcp_connection::is_complete() << std::endl;
		LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
		start();
    }

    bool stopped()
    {
		return (get().stopped());
    }

    void stop()
    {

		get().stop();
    }
    
    void start(void)
    {
        _LOG::out_s << "started server" << std::endl; 
        LOGTOFILE(LOGHTML::messageType::MESSAGE,_LOG::out_s);
        LAYER_QUERIES::createBaseLayer();
		start_accepting();
        std::cout << "run_one started..." << std::endl; 
		boost::system::error_code err;

	    do {
			get().run_one(err);
        } while(!tcp_connection::is_complete()) ;
		
		_LOG::out_s << "error code = " << err << std::endl;
		LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
        std::cout << "is_complete ==  " << tcp_connection::is_complete() << std::endl; 
    }

    ~server()
    {
		LOGTOFILE(LOGHTML::messageType::STRONG_WARNING , "destroy server .."); 
		while  (!stopped())
		{
			stop();
		}
		if (stopped())
		{
			LOGTOFILE(LOGHTML::messageType::NOTICE,"try to reset io...");
			get().reset();
			tcp_connection::destroy();
		}
    }

    void start_accepting()
    {
		acceptor.async_accept(connector->socket(),[this](boost::system::error_code ec)
		{
			if (!ec)
			{
				std::cout << "new connection: " << connector->socket()
					.remote_endpoint()
					.address().to_string().data() <<std::endl;
				try
				{
					LOGTOFILE(LOGHTML::messageType::NOTICE,"try to set readHeader event..");
					connector->readHeader();
				}
				catch (std::runtime_error& ext)
				{
					_LOG::out_s << "runtime exception with message =" << ext.what() ;//<< "error code = " << err << std::endl;
					LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
					// << std::endl;
				}
				}
				else
				{
					acceptor.close();
				}
				start_accepting();
		});
	}
};

namespace
{
    volatile std::sig_atomic_t gSignalStatus = 0;
}

void signal_handler(int signal)
{
    gSignalStatus = signal;
	
	tcp_connection::showConsoleTraceLog();
	std::cout << "stop the program.." << std:: endl;
	LOGSTOP();
	exit(signal);
}

int main(int argc,char *argv[])
{
		
LOGINIT();
std::signal(SIGINT, signal_handler);
while(true)
{
    server s(2002);    
    std::cout <<"restart..." << std::endl;
}


  return 0;
}





#endif