#include "Client.h"

namespace MultipleDepthSensorsPlatformServer{

	Client::Client(Timer *time, unsigned int id, string loc,string ip_addr)
	:_id(id),_location(loc),_ip_addr(ip_addr),_curTime(time)
	{
	}

	Client::~Client(){

		/* Delete all sensors under the control of the client */

		for(map<string,Sensor*>::iterator sensor= _sensors.begin(); sensor!= _sensors.end();sensor++){
			Sensor *curSensor = sensor->second;

			delete curSensor;
		}

		this->_sensors.clear();
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
		map<string,Sensor*>::iterator lastElem = this->_sensors.end();
		std::advance(lastElem,-1);
		for(map<string,Sensor*>::iterator itr = this->_sensors.begin();itr!=this->_sensors.end();itr++)
		{
			json += "{";

			json += "\"id\":";

			string sensorId = itr->second->GetId();
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
			if(itr->second->GetScene()->GetCalibration()){
				json += "\"true\"" ;
			}else{
				json += "\"false\"";
			}
			json += ",";

			json += "\"ordering\":";
			json += "\"" + std::to_string(itr->second->GetScene()->GetOrdering()) + "\"";


			json += "}";

			if(itr!=lastElem)
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

				Sensor *newSensor = new Sensor(this->_curTime,id.asString());

				this->_sensors.insert(std::pair<string,Sensor*>(id.asString(),newSensor));
			}
		}
	}

	Sensor* Client::ExtractSensor(string sensorId){
		if(this->_sensors.size()!=0){
			return this->_sensors.at(sensorId);
		}else{
			return NULL;
		}
	}

	map<string,Sensor*> Client::GetSensorsList(){
			return this->_sensors;
	}
}