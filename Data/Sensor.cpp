#include "Sensor.h"

namespace MultipleKinectsPlatformServer{
	
	Sensor::Sensor(string id)
		:_id(id),_calibrated(false),_ordering(0)
	{
	}

	Sensor::~Sensor()
	{

	}

	bool Sensor::CheckCalibration(){
		return this->_calibrated;
	}

	void Sensor::Calibrate(){

	}

	string Sensor::GetId(){
		return this->_id;
	}

	unsigned int Sensor::GetOrdering(){
		return this->_ordering;
	}

}