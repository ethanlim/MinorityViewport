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
		public:
			Sensor(string id);
			~Sensor();
	};
}

#endif