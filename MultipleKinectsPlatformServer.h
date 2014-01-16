#ifndef MULTIPLEKINECTSPLATFORMSERVER_H
#define MULTIPLEKINECTSPLATFORMSERVER_H

#define _WIN32_WINNT 0x0501

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include "Networks/Server.h"
#include "Data/JobsQueue.h"
#include "Graphics/Visualisation.h"
#include "Data/Job.h"
#include "Data/JSON/json.h"
#include "Data/Skeleton.h"
#include "Data/ClientsList.h"
#include "Algorithms/MinorityViewport.h"
#include "Networks/NTPClient.h"
#include "Misc/Timer.h"

using namespace std;

namespace MultipleKinectsPlatformServer{
	
	/* Global Variables */
	std::mutex joints_mutex;

	class Core{
		private:
			MultipleKinectsPlatformServer::ClientsList *clientList;
			MultipleKinectsPlatformServer::JobsQueue *jobQueue;
			http::server::server *server;
			MultipleKinectsPlatformServer::Visualisation *visualisation;
			MinorityViewport *minorityViewport;
			Timer *time;
			void ReportStatus(string message);

		public :
			Core(string address,string port);
			~Core();
			void BeginListen();			
			void ProcessJobs();
			void BeginVisualisation(int *argcp, char **argv);
	};
}

#endif
