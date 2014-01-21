#include "Client.h"

namespace MultipleKinectsPlatformServer{

	Client::Client(unsigned int id, string loc,string ip_addr)
		:_id(id),_location(loc),_ip_addr(ip_addr)
	{}

	Client::~Client(){

	}

	unsigned int Client::GetId(){
		return this->_id;
	}

	string Client::ToJSON(){
		string json;

		json += "{";

		json += "\"id\":";
		json += to_string(this->_id);
		json += ",";

		json += "\"location\":";
		json += "\"" + this->_location + "\"";
		json += ",";

		json += "\"address\":";
		json += "\"" + this->_ip_addr + "\"" ;
		json += ",";

		json += "\"sensors\":";
		json += "[";
		for(unsigned int sensor=0;sensor<this->_sensors.size();sensor+=1)
		{
			json += "{";

			json += "\"id\":";

			string sensorId = this->_sensors[sensor].GetId();
			size_t i = sensorId.find('\\');
			while (i != string::npos)
			{
				string part1 = sensorId.substr(0, i);
				string part2 = sensorId.substr(i + 1);
				sensorId = part1 + "\\\\" + part2;
				i = sensorId.find('\\', i + 4);
			}

			json += "\"" + sensorId + "\"";
			json += ",";

			json += "\"calibrated\":";
			if(this->_sensors[sensor].CheckCalibration()){
				json += "\"true\"" ;
			}else{
				json += "\"false\"";
			}
			json += ",";

			json += "\"ordering\":";
			json += "\"" + std::to_string(this->_sensors[sensor].GetOrdering()) + "\"";


			json += "}";

			if(sensor!=this->_sensors.size()-1)
			{
				json += ",";
			}
		}
		json += "]";


		json += "}";

		return json;
	}

	void Client::InitialSensorsList(string rawSensors_JSON){

		Json::Value root;   
		Json::Reader reader;

		if (reader.parse(rawSensors_JSON,root)){

			Json::Value sensors_JSON = root.get("Sensors",NULL);
			Json::Value sensor_JSON;

			for(unsigned short sensor=0;sensor<sensors_JSON.size();sensor++){

				sensor_JSON = sensors_JSON[sensor];
				
				Json::Value id = sensor_JSON.get("id",NULL);

				Sensor newSensor(id.asString());

				this->_sensors.push_back(newSensor);
			}
		}
	}
}