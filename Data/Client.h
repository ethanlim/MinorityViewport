#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <vector>
#include <algorithm>
#include <regex>

#include "Sensor.h"
#include "JSON/json.h"

using namespace std;

namespace MultipleKinectsPlatformServer{

	class Client{
		private:
			unsigned int _id;
			string _location;
			string _ip_addr;
			bool _calibrated;
			vector<Sensor> _sensors;
		public:
			Client(unsigned int id, string loc,string ip_addr);
			~Client();
			unsigned int GetId();
			string ToJSON();
			void InitialSensorsList(string rawSensors_JSON);
	};
}

#endif