#include "Job.h"

namespace MultipleKinectsPlatformServer{

	Job::Job(){

	}

	Job::Job(/*sensor data*/string i_sensor_data, /*time stamp*/ string i_time_stamp)
		: sensor_data(i_sensor_data),time_stamp(i_time_stamp)
	{

	}

	Job::~Job(){

	}

	string Job::GetJobJSON(){
		return this->sensor_data;
	}

	string Job::GetTimeStamp(){
		return this->time_stamp;
	}
}