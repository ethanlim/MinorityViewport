#include "Sensor.h"

namespace MultipleDepthSensorsServer{
	
	Sensor::Sensor(Timer *time, string id)
	:_id(id),_curTime(time)
	{
		this->_scene = new Scene(id,time);
	}

	Sensor::~Sensor()
	{
		delete this->_scene;
	}

	string Sensor::GetId(){
		return this->_id;
	}

	void Sensor::UpdateScene(Skeleton person){
		this->_scene->Update(person.GetSkeletonId(),person);
	}

	Scene* Sensor::GetScene(){
		return this->_scene;
	}
}