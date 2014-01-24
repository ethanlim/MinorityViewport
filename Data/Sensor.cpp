#include "Sensor.h"

namespace MultipleKinectsPlatformServer{
	
	Sensor::Sensor(Timer *time, string id)
	:_id(id),_curTime(time)
	{
		this->_scene = new Scene(time);
	}

	Sensor::~Sensor()
	{

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