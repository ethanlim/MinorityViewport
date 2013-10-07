#include "MutlipleKinectsPlatformServer.h"

namespace MultipleKinectsPlatformServer{

	Core::Core(){
	  try
	  {
		string address = "127.0.0.1"; //localhost
		string port = "1626";
		string docRoot = "C:\\Users\\ethanlim\\Documents\\Projects\\School\\MultipleKinectsPlatformServer";
		MultipleKinectsPlatformServer::JobsQueue *jobQueue;

		// Initialise the server.
		jobQueue = new MultipleKinectsPlatformServer::JobsQueue();
		std::size_t num_threads = boost::lexical_cast<std::size_t>(2);

		http::server::server s(address, port, docRoot, jobQueue, num_threads);

		// Run the server until stopped.
		s.run();
	  }
	  catch (std::exception& e)
	  {
		std::cerr << "exception: " << e.what() << "\n";
	  }

	}

	Core::~Core(){

	}

}

int main( int argc, const char* argv[] )
{
	MultipleKinectsPlatformServer::Core platform;

	return 0;
}
