#include "MutlipleKinectsPlatformServer.h"

namespace MultipleKinectsPlatformServer{

	Core::Core(string address,string port){
	  try
	  {
		string docRoot = "C:\\Users\\ethanlim\\Documents\\Projects\\School\\MultipleKinectsPlatformServer";

		// Initialise the Job Queue
		this->jobQueue = new MultipleKinectsPlatformServer::JobsQueue();

		std::size_t num_threads = boost::lexical_cast<std::size_t>(5);

		//Initialise the Server
		this->server = new http::server::server(address, port, docRoot, this->jobQueue, num_threads);

		//Initialise view window
		this->visualisationWindow = new Visualisation();

	  }
	  catch (std::exception& e)
	  {
		std::cerr << "exception: " << e.what() << "\n";
	  }

	}

	Core::~Core(){
		//delete this->jobQueue;
	}

	void Core::BeginListen(){
		try{
			this->server->run();
		}
		catch(std::exception& e){
			std::cerr << "exception: " << e.what() << "\n";
		}
	}

	void Core::ProcessJobs(){
		while(1){
			if(this->jobQueue->get_size()>0){
				Json::Value root;   
				Json::Reader reader;

				string rawJSON = this->jobQueue->pop();

				bool parsingSuccessful = reader.parse(rawJSON,root);

				if (parsingSuccessful)
				{
					for(unsigned short numOfSkeletons=0;root.size();numOfSkeletons++){

					}
				}
			}
		}
	}
}

int main( int argc, const char* argv[] )
{
	MultipleKinectsPlatformServer::Core platform("127.0.0.1","1626");

	// Start Server on a separate thread
	thread server_thread(&MultipleKinectsPlatformServer::Core::BeginListen,platform);

	// Process Job on a separate thread
	thread job_thread(&MultipleKinectsPlatformServer::Core::ProcessJobs,platform);

	server_thread.join();
	job_thread.join();

	return 0;
}
