#ifndef JOBSQUEUE_H
#define JOBSQUEUE_H

#include "JSON\json.h"
#include <queue>
#include <string>

using namespace std;

namespace MultipleKinectsPlatformServer{

	class JobsQueue{
		public:
			JobsQueue();
			~JobsQueue();
			void push(string raw_json);
			string pop();
			unsigned int get_size();
		private:
			queue<string> *json_queue;
	};
}

#endif