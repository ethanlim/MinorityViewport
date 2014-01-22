#ifndef SENSOR_H
#define SENSOR_H

#include <string>
#include <map>

#include "Scene.h"
#include "Skeleton.h"
#include "JSON/json.h"

using namespace std;

namespace MultipleKinectsPlatformServer{
	class Sensor{
		private:
			string _id;
			unsigned int _ordering;
			bool _calibrated;
			Scene *_scene;
			
		public:
			Sensor(string id);
			~Sensor();
			bool CheckCalibration();
			void Calibrate();
			string GetId();
			unsigned int GetOrdering();

			void UpdateScene(Skeleton person,long timeStamp);
	};
}

#endif