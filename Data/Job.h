#ifndef JOB_H
#define JOB_H

using namespace std;

#include <string>

namespace MinorityViewport{

	class Job{
		private:
			string sensor_data;
			string time_stamp;
		public:
			Job();
			Job(string i_sensor_data, string i_time_stamp);
			~Job();
			string GetJobJSON();
			string GetTimeStamp();
	};
}


#endif