#include "Sensor.h"

namespace MultipleKinectsPlatformServer{
	
	Sensor::Sensor(Timer *time, string id)
	:_id(id),_calibrated(false),_curTime(time)
	{
		this->_scene = new Scene(time);
	}

	Sensor::~Sensor()
	{

	}

	bool Sensor::CheckCalibration(){
		return this->_calibrated;
	}

	void Sensor::SetCalibrate(bool calibrated){
		this->_calibrated = calibrated;
	}

	string Sensor::GetId(){
		return this->_id;
	}

	void Sensor::UpdateScene(Skeleton person, long timeStamp){
		this->_scene->Update(person,timeStamp);
	}

	Scene* Sensor::GetScene(){
		return this->_scene;
	}
}