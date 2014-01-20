#include "Client.h"

namespace MultipleKinectsPlatformServer{

	Client::Client(unsigned int id, string loc,string ip_addr)
		:_id(id),_location(loc),_ip_addr(ip_addr),_calibrated(false)
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

		json += "\"calibrated\":";
		if(this->_calibrated){
			json += "\"true\"" ;
		}else{
			json += "\"false\"";
		}
		json += "";

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

				string find = "\\";
				string replace = "\\";

				std::string escapedId  = id.asString();
				std::tr1::regex rx(find);
				std::string newId = std::tr1::regex_replace(escapedId, rx,replace);

				Sensor newSensor(newId);

				this->_sensors.push_back(newSensor);
			}
		}
	}
}