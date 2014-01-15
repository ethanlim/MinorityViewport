#ifndef JOBSQUEUE_H
#define JOBSQUEUE_H

#include "Job.h"
#include "JSON/json.h"
#include <queue>
#include <string>

using namespace std;

namespace MultipleKinectsPlatformServer{

	class JobsQueue{
		public:
			JobsQueue();
			~JobsQueue();
			void push(string raw_json,string time_stamp);
			Job pop();
			unsigned int get_size();
		private:
			queue<Job> *json_queue;
	};
}

#endif
