#include "MultipleKinectsPlatformServer.h"

namespace MultipleKinectsPlatformServer{

	Core::Core(/*Server Address*/string address,/*Server Port*/string port){
	  try
	  {
		// Initialise the Client Machine List
		this->clientList = new MultipleKinectsPlatformServer::ClientsList();

		// Create the jobs queue that process each incoming data from client machines
		this->jobQueue = new MultipleKinectsPlatformServer::JobsQueue();

		this->minorityViewport = new MinorityViewport();

		//Initialise the Server with the number of threads

		this->ReportStatus("Server Starting");

		string docRoot = "C:\\Users\\ethanlim\\Documents\\Projects\\School\\MultipleKinectsPlatformServer";
		std::size_t num_threads = boost::lexical_cast<std::size_t>(20);
		this->server = new http::server::server(address, port, docRoot, this->jobQueue, num_threads,this->clientList);
		
		this->ReportStatus("Server Started");

	  }
	  catch (std::exception& e)
	  {
		std::cerr << "exception: " << e.what() << "\n";
	  }

	}

	Core::~Core(){
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
		while(true){
			if(this->jobQueue->get_size()>0){
				Json::Value root;   
				Json::Reader reader;

				Job recvJob =  this->jobQueue->pop();

				string rawJSON = recvJob.GetJobJSON();
				string timeStamp = recvJob.GetTimeStamp();

				if (reader.parse(rawJSON,root))
				{
					for(unsigned short skeletons=0;skeletons<root.size();skeletons++){
					    
						MultipleKinectsPlatformServer::Skeleton newSkeleton(root.get(skeletons,NULL));

					}
				}
			}
		}
	}

	void Core::BeginVisualisation(int *argc, char **argv){

		/* TODO: Restore the visualisation soon */
		//this->visualisation = new Visualisation(argc,argv);
	}

	void Core::ReportStatus(string msg){
		cout << "Status : " << msg << endl;
	}
}

int main(int argc, char **argv)
{
	MultipleKinectsPlatformServer::Core platform(argv[1],argv[2]);

	// Start Server on a separate thread
	thread server_thread(&MultipleKinectsPlatformServer::Core::BeginListen,platform);

	// Process Job on a separate thread
	thread job_thread(&MultipleKinectsPlatformServer::Core::ProcessJobs,platform);

	// Begin Visual
	thread ui_thread(&MultipleKinectsPlatformServer::Core::BeginVisualisation,platform,&argc,argv);

	server_thread.join();
	job_thread.join();
	ui_thread.join();

	return 0;
}
