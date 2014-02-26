#include "MultipleKinectsPlatformServer.h"

namespace MultipleKinectsPlatformServer{

	Core::Core(/*Server Address*/string address,/*Server Port*/string port){
	  try
	  {
		/* Communicate with a centralised time server */
		this->ReportStatus("Sync with Server Time");
		NTPClient timeClient("2.asia.pool.ntp.org");
		long timeFromServer = timeClient.RequestDatetime_UNIX();
		this->_time = new Timer(timeFromServer);
		this->_time->Start();
		
		this->ReportStatus("Create Key Objects");
		/* Initialise the Client Machine List */
		this->_clientList = new MultipleKinectsPlatformServer::ClientsList(this->_time);
		/* Create the jobs queue that process each incoming data from client machines */
		this->_jobQueue = new MultipleKinectsPlatformServer::JobsQueue();
		/* Create algorithm that merge the scenes */
		this->_minorityViewport = new MinorityViewport(this->_time,this->_clientList);

		this->ReportStatus("Server Starting");
		//Initialise the Server with the number of threads
		string docRoot = "C:\\Users\\ethanlim\\Documents\\Projects\\School\\MultipleKinectsPlatformServer\\Web";
		std::size_t num_threads = boost::lexical_cast<std::size_t>(20);
		this->_server = new http::server::server(address, 
												port, 
												docRoot, 
												this->_jobQueue, 
												num_threads,
												this->_clientList,
												this->_minorityViewport);

		this->ReportStatus("Server Started");
	  }
	  catch (std::exception& e)
	  {
		std::cerr << "exception: " << e.what() << "\n";
	  }
	}
	
	void Core::BeginListen(){
		try{
			this->_server->run();			
		}
		catch(std::exception& e){
			std::cerr << "exception: " << e.what() << "\n";
		}
	}

	void Core::ProcessJobs(){

		Json::Value root;   
		Json::Reader reader;
		string rawJSON;
		string timeStamp;

		while(true){

			JobQueueMutex.lock();
			Job recvJob;
			if(!this->_jobQueue->empty()){
				recvJob =  this->_jobQueue->front();
			
				rawJSON = recvJob.GetJobJSON();
				timeStamp = recvJob.GetTimeStamp();
			
				this->_jobQueue->pop();
			}
			JobQueueMutex.unlock();

			if (reader.parse(rawJSON,root))
			{
				for(unsigned short skeletons=0;skeletons<root.size();skeletons++){
					MultipleKinectsPlatformServer::Skeleton newSkeleton(root.get(skeletons,NULL),atol(timeStamp.c_str()));
					this->_minorityViewport->LoadSkeleton(newSkeleton);
				}
			}
		}
	}

	void Core::ReportStatus(string msg){
		cout << "Status : " << msg << endl;
	}
}

int main(int argc, char **argv)
{
	if(argc==1){
		throw exception("Program arguments are empty");
	}

	try{
		MultipleKinectsPlatformServer::Core platform(argv[1],argv[2]);

		// Start Server on a separate thread
		thread server_thread(&MultipleKinectsPlatformServer::Core::BeginListen,platform);

		// Process Job on a separate thread
		thread job_thread(&MultipleKinectsPlatformServer::Core::ProcessJobs,platform);

		server_thread.join();
		job_thread.join();
	}catch(exception &error){
		throw error;
	}

	return 0;
}
