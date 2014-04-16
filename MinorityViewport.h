#ifndef MULTIPLEKINECTSPLATFORMSERVER_H
#define MULTIPLEKINECTSPLATFORMSERVER_H

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <ctime>

/*********************/
/*		BOOST		*/
/********************/
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

/*********************/
/*	PROJECT CLASSES	 */
/*********************/
#include "Networks/UdpServer.h"
#include "Networks/Server.h"
#include "Data/JobsQueue.h"
#include "Data/Job.h"
#include "Data/JSON/json.h"
#include "Data/Skeleton.h"
#include "Data/ClientsList.h"
#include "Algorithms/MinorityViewportAlgo.h"
#include "Networks/NTPClient.h"
#include "Misc/Timer.h"

using namespace std;

namespace MinorityViewport{
	
	mutex JobThreadMutex;	

	class Core{
		private:
			MinorityViewport::ClientsList *_clientList;
			MinorityViewport::JobsQueue *_jobQueue;
			
			http::server::server *_httpServer;
			MinorityViewport::UdpServer *_udpServer;

			MinorityViewportAlgo *_minorityViewport;
			Timer *_time;
			void ReportStatus(string message);
		public :
			Core(string address,string httpPort,string udpPort);
			void BeginListen();
			void BeginUdpListen();
			void ProcessJobs();
			void BeginVisualisation(int *argcp, char **argv);
	};
}

#endif
