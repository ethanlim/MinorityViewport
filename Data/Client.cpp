#include "Client.h"

namespace MultipleKinectsPlatformServer{

	Client::Client(unsigned int id, string loc,string ip_addr)
		:_id(id),_location(loc),_ip_addr(ip_addr),_calibrated(false)
	{

	}

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
}