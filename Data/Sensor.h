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
			bool _calibrated;
			Scene *_scene;
			
			Timer *_curTime;

		public:
			Sensor(Timer *time, string id);
			~Sensor();

			bool CheckCalibration();
			void Calibrate();
			string GetId();
			
			void SetCalibrate(bool calibrated);
			void UpdateScene(Skeleton person,long timeStamp);
			Scene* GetScene();
	};
}

#endif