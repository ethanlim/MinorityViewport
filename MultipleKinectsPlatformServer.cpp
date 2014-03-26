#include "MultipleKinectsPlatformServer.h"

namespace MultipleKinectsPlatformServer{

	Core::Core(/*Server Address*/string address,/*Server Port*/string httpPort,string udpPort){
	  try
	  {
		/* Communicate with a centralised time server */
		unsigned short randomTimeServerId = rand()%5;
		string timeServer = std::to_string(randomTimeServerId)+".asia.pool.ntp.org";
		this->ReportStatus("Sync with Time Server - "+ timeServer);
		NTPClient timeClient(timeServer);
		long timeFromServer = timeClient.RequestDatetime_UNIX();
		this->_time = new Timer(timeFromServer);
		this->_time = new Timer(0);
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
		std::size_t num_threads = boost::lexical_cast<std::size_t>(5);
		this->_httpServer = new http::server::server(
												address, 
												httpPort, 
												docRoot, 
												this->_jobQueue, 
												num_threads,
												this->_clientList,
												this->_minorityViewport
												);

		this->ReportStatus("Server Started");

		this->_udpServer = new MultipleKinectsPlatformServer::UdpServer(address,atoi(udpPort.c_str()),this->_minorityViewport);
	  }
	  catch (std::exception& e)
	  {
		std::cerr << "exception: " << e.what() << "\n";
	  }
	}
	
	void Core::BeginListen(){
		try{
			this->_httpServer->run();			
		}
		catch(std::exception& e){
			std::cerr << "exception: " << e.what() << "\n";
		}
	}

	void Core::BeginUdpListen(){
		try{
			this->_udpServer->Run();			
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

			Job recvJob;
			JobThreadMutex.lock();
			recvJob =  this->_jobQueue->get();
			JobThreadMutex.unlock();
			
			rawJSON = recvJob.GetJobJSON();
			timeStamp = recvJob.GetTimeStamp();
			
			if (!rawJSON.empty()&&!timeStamp.empty()&&reader.parse(rawJSON,root))
			{
				for(unsigned short skeletons=0;skeletons<root.size();skeletons++){
					MultipleKinectsPlatformServer::Skeleton newSkeleton(root.get(skeletons,NULL),atol(timeStamp.c_str()));
					this->_minorityViewport->LoadSkeleton(newSkeleton);
				}
			}
		}
	}

	void Core::ReportStatus(string msg){
		cout << "Core : " << msg << endl;
	}
}

int main(int argc, char **argv)
{
	srand(time(NULL));

	MultipleKinectsPlatformServer::Core *platform;

	if(argc==1){
		throw exception("Program arguments are empty");
	}

	try{
		platform = new MultipleKinectsPlatformServer::Core(argv[1],argv[2],argv[3]);

		if(platform!=NULL){

			thread server_thread(&MultipleKinectsPlatformServer::Core::BeginListen,platform);
			thread udpServer_thread(&MultipleKinectsPlatformServer::Core::BeginUdpListen,platform);

			server_thread.join();
			udpServer_thread.join();
		}
	}catch(exception &error){
		throw error;
	}

	return 0;
}
