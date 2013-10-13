#ifndef JOBSQUEUE_H
#define JOBSQUEUE_H

#include "JSON\json.h"
#include <queue>
#include <string>
#include <iostream>

using namespace std;

namespace MultipleKinectsPlatformServer{

	class JobsQueue{
		public:
			JobsQueue();
			~JobsQueue();
			void push(string raw_json);
			string pop();
		private:
			queue<string> *json_queue;
	};
}

#endif