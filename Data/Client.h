#ifndef CLIENT_H
#define CLIENT_H

#include <string>

using namespace std;

namespace MultipleKinectsPlatformServer{

	class Client{
		private:
			unsigned int _id;
			string _location;
			string _ip_addr;
		public:
			Client(unsigned int id, string loc,string ip_addr);
			~Client();
			unsigned int GetId();
			string ToJSON();
	};
}

#endif