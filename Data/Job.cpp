#include "Job.h"

namespace MinorityViewport{

	Job::Job(){

	}

	Job::Job(/*sensor data*/string i_sensor_data, /*time stamp*/ string i_time_stamp)
	{
		if(i_sensor_data.length()!=0&&i_time_stamp.length()!=0){
			sensor_data = i_sensor_data;
			time_stamp = i_time_stamp;
		}
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