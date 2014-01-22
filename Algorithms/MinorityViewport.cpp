#include "MinorityViewport.h"

using namespace std;

namespace MultipleKinectsPlatformServer{

	MinorityViewport::MinorityViewport(ClientsList *clients)
	:_clients(clients)
	{

	}

	MinorityViewport::~MinorityViewport(){

	}

	void MinorityViewport::Calibrate(){

		

	}

	bool MinorityViewport::CheckCalibration(){

		bool calibrated;

		return calibrated;
	}

	void MinorityViewport::LoadSkeleton(Skeleton newSkeleton,long timeStamp){

		unsigned int clientId = newSkeleton.GetClientId();
		string sensorId = newSkeleton.GetSensorId();
		
		Client* client = this->_clients->At(clientId);

		Sensor* sensor = client->ExtractSensor(sensorId);

		sensor->UpdateScene(newSkeleton,timeStamp);
	}

	void MinorityViewport::MergeScenes(){

	}

}