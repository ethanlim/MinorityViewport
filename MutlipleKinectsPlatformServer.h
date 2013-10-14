
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

using namespace std;

namespace MultipleKinectsPlatformServer{

	class Core{
		public :
			Core(string address,string port);
			~Core();
			void BeginListen();
		private:
			void ProcessJobQueue(u_int threadId);
			static const u_int threadNum = 5;
			thread runners[threadNum];
			MultipleKinectsPlatformServer::JobsQueue *jobQueue;
			http::server::server *server;
	};
}

#endif