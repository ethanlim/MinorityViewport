#ifndef SENSOR_H
#define SENSOR_H

#include <string>
#include <algorithm>

#include "JSON/json.h"

using namespace std;

namespace MultipleKinectsPlatformServer{
	class Sensor{
		private:
			string _id;
			unsigned int _ordering;
			bool _calibrated;
		public:
			Sensor(string id);
			~Sensor();
			bool CheckCalibration();
			void Calibrate();
			string GetId();
			unsigned int GetOrdering();
	};
}

#endif