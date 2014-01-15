#include "JobsQueue.h"

namespace MultipleKinectsPlatformServer{

	JobsQueue::JobsQueue(){
		json_queue = new std::queue<Job>();
	}

	JobsQueue::~JobsQueue(){
		delete json_queue;
	}

	void JobsQueue::push(string object,string time_stamp){

		Job new_incoming_data(object,time_stamp);

		this->json_queue->push(new_incoming_data);
	}

	Job JobsQueue::pop(){

		Job obtainedJob = this->json_queue->front();
		
		this->json_queue->pop();

		return obtainedJob;
	}

	unsigned int JobsQueue::get_size(){

		return this->json_queue->size();
	}
}