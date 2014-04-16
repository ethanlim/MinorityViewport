#ifndef SENSOR_H
#define SENSOR_H

#include <string>
#include <map>

#include "Scene.h"
#include "Skeleton.h"
#include "JSON/json.h"
#include "../Misc/Timer.h"

using namespace std;

namespace MinorityViewport{
	class Sensor{
		private:
			Scene *_scene;
			Timer *_curTime;
			string _id;
		public:
			Sensor(Timer *time, string id);
			~Sensor();

			string GetId();
			
			void UpdateScene(Skeleton person);
			Scene* GetScene();
	};
}

#endif