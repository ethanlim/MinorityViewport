#ifndef SENSOR_H
#define SENSOR_H

#include <string>
#include <map>

#include "Scene.h"
#include "Skeleton.h"
#include "JSON/json.h"
#include "../Misc/Timer.h"

using namespace std;

namespace MultipleKinectsPlatformServer{
	class Sensor{
		private:
			string _id;
			Scene *_scene;
			
			Timer *_curTime;
		public:
			Sensor(Timer *time, string id);
			~Sensor();

			string GetId();
			
			void UpdateScene(Skeleton person,long timeStamp);
			Scene* GetScene();
	};
}

#endif