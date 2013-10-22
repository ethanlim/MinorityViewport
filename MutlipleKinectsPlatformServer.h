
#ifndef MULTIPLEKINECTSPLATFORMSERVER_H
#define MULTIPLEKINECTSPLATFORMSERVER_H

#include <iostream>
#include <string>
#include <thread>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "Networks/Server.h"
#include "Data/JobsQueue.h"
#include "Graphics/Visualisation.h"
#include "Data/JSON/json.h"
#include "Data/Skeleton.h"

using namespace std;

namespace MultipleKinectsPlatformServer{

	Skeleton *global_skeleton = NULL;

	class Core{
		public :
			Core(string address,string port);
			~Core();
			void BeginListen();			
			void ProcessJobs();
			void BeginVisualisation(int *argcp, char **argv);
		private:
			MultipleKinectsPlatformServer::JobsQueue *jobQueue;
			http::server::server *server;
			MultipleKinectsPlatformServer::Visualisation *visualisation;
	};
}

#endif