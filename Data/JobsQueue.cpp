#include "JobsQueue.h"

namespace MultipleKinectsPlatformServer{

	JobsQueue::JobsQueue(){
		_jsonQueue = new std::queue<Job>();
	}

	JobsQueue::~JobsQueue(){
		delete _jsonQueue;
	}

	void JobsQueue::push(string object,string time_stamp){

		Job new_incoming_data(object,time_stamp);

		this->_jsonQueue->push(new_incoming_data);
	}

	Job JobsQueue::front(){
		return this->_jsonQueue->front();
	}

	bool JobsQueue::empty(){
		return this->_jsonQueue->empty();
	}

	void JobsQueue::pop(){
		return this->_jsonQueue->pop();
	}
}