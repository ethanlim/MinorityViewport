#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <map>
#include <algorithm>

#include "Sensor.h"
#include "JSON/json.h"
#include "../Misc/Timer.h"

using namespace std;

namespace MinorityViewport{

	class Client{
		private:
			unsigned int _id;
			string _location;
			string _ip_addr;
			map<string,Sensor*> _sensors;
			Timer *_curTime;
		public:
			Client(Timer *time,unsigned int id, string loc,string ip_addr);
			~Client();
			unsigned int GetId();
			string ToJSON();
			void InitialSensorsList(string rawSensors_JSON);
			Sensor* ExtractSensor(string sensorId);
			map<string,Sensor*> GetSensorsList();
	};
}

#endif