#include "Client.h"

namespace MultipleKinectsPlatformServer{

	Client::Client(unsigned int id, string loc,string ip_addr)
		:_id(id),_location(loc),_ip_addr(ip_addr)
	{

	}

	Client::~Client(){

	}

	unsigned int Client::GetId(){
		return this->_id;
	}
}