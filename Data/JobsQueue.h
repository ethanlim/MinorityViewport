#ifndef JOBSQUEUE_H
#define JOBSQUEUE_H

#include "Job.h"
#include "JSON/json.h"
#include <queue>
#include <string>
#include <mutex>

using namespace std;

namespace MultipleDepthSensorsServer{

	class JobsQueue{
		private:
			queue<Job> *_jsonQueue;
			mutex _jobQueueMutex;
		public:
			JobsQueue();
			~JobsQueue();
			void push(string raw_json,string time_stamp);
			Job get();
	};
}

#endif
