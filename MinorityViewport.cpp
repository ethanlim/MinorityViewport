#include "MinorityViewport.h"

namespace MinorityViewport{

	Core::Core(/*Server Address*/string address,/*Server Port*/string httpPort,string udpPort){
	  try
	  {
		/*Upgraded the Priority Class*/
		DWORD dwPriClass;
		dwPriClass = GetPriorityClass(GetCurrentProcess());
		cout << "Current priority class is " << std::hex << dwPriClass << endl;
		if(!SetPriorityClass(GetCurrentProcess(),HIGH_PRIORITY_CLASS)){
			cout << "Failed to upgrade priority class" << GetLastError() << endl;
		}else{
			dwPriClass = GetPriorityClass(GetCurrentProcess());
			cout << "Upgraded priority class is " << std::hex << dwPriClass << endl;
		}

		/* Communicate with a centralised time server */
		/*
		unsigned short randomTimeServerId = rand()%5;
		string timeServer = std::to_string(randomTimeServerId)+".asia.pool.ntp.org";
		this->ReportStatus("Sync with Time Server - "+ timeServer);
		NTPClient timeClient(timeServer);
		long timeFromServer = timeClient.RequestDatetime_UNIX();
		*/
		time_t ltime;
		this->_time = new Timer(time(&ltime));
		this->_time->Start();

		this->ReportStatus("Create Key Objects");
		/* Initialise the Client Machine List */
		this->_clientList = new MinorityViewport::ClientsList(this->_time);
		/* Create the jobs queue that process each incoming data from client machines */
		this->_jobQueue = new MinorityViewport::JobsQueue();
		/* Create algorithm that merge the scenes */
		this->_minorityViewport = new MinorityViewportAlgo(this->_time,this->_clientList);

		this->ReportStatus("Server Starting");
		//Initialise the Server with the number of threads
		string docRoot = "C:\\Users\\ethanlim\\Documents\\Projects\\School\\MinorityViewport\\Web";
		std::size_t num_threads = boost::lexical_cast<std::size_t>(5);
		this->_httpServer = new http::server::server(
												address, 
												httpPort, 
												docRoot, 
												num_threads,
												this->_minorityViewport
												);

		this->ReportStatus("Server Started");

		this->_udpServer = new MinorityViewport::UdpServer(address,atoi(udpPort.c_str()),this->_minorityViewport);
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

	void Core::ReportStatus(string msg){
		cout << "Core : " << msg << endl;
	}
}

int main(int argc, char **argv)
{
	srand(time(NULL));

	MinorityViewport::Core *platform;

	if(argc==1){
		throw exception("Program arguments are empty");
	}

	try{
		platform = new MinorityViewport::Core(argv[1],argv[2],argv[3]);
	
		if(platform!=NULL){
			thread server_thread(&MinorityViewport::Core::BeginListen,platform);
			thread udp_thread(&MinorityViewport::Core::BeginUdpListen,platform);

			server_thread.join();
			udp_thread.join();
		}
	}catch(exception &error){
		throw error;
	}

	return 0;
}
