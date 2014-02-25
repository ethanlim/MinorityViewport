#include "JobsQueue.h"

namespace MultipleKinectsPlatformServer{

	JobsQueue::JobsQueue(){
		_jsonQueue = new std::queue<Job>();
	}

	JobsQueue::~JobsQueue(){
		delete _jsonQueue;
	}

	void JobsQueue::push(string object,string time_stamp){
		if(!object.empty()&&!time_stamp.empty()){
			Job new_incoming_data(object,time_stamp);

			this->_jsonQueue->push(new_incoming_data);
		}
	}

	Job JobsQueue::front(){
		Job recvJob("","");
		try{
			recvJob = this->_jsonQueue->front();
		}catch(exception failedRetrieve){
			return recvJob;
		}
		return recvJob;
	}

	bool JobsQueue::empty(){
		return this->_jsonQueue->empty();
	}

	void JobsQueue::pop(){
		return this->_jsonQueue->pop();
	}
}