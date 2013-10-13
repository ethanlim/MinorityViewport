#include "MutlipleKinectsPlatformServer.h"

namespace MultipleKinectsPlatformServer{

	Core::Core(string address,string port){
	  try
	  {
		string docRoot = "C:\\Users\\ethanlim\\Documents\\Projects\\School\\MultipleKinectsPlatformServer";

		// Initialise the Job Queue
		this->jobQueue = new MultipleKinectsPlatformServer::JobsQueue();

		std::size_t num_threads = boost::lexical_cast<std::size_t>(2);

		//Initialise the Server
		this->server = new http::server::server(address, port, docRoot, this->jobQueue, num_threads);
	  }
	  catch (std::exception& e)
	  {
		std::cerr << "exception: " << e.what() << "\n";
	  }

	}

	Core::~Core(){
		delete this->jobQueue;
	}

	void Core::BeginListen(){
		try{
			this->server->run();
		}
		catch(std::exception& e){
			std::cerr << "exception: " << e.what() << "\n";
		}
	}
}

int main( int argc, const char* argv[] )
{
	MultipleKinectsPlatformServer::Core platform("127.0.0.1","1626");

	platform.BeginListen();

	return 0;
}
