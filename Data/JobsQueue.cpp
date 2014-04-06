#include "JobsQueue.h"

namespace MultipleDepthSensorsPlatformServer{

	JobsQueue::JobsQueue(){
		_jsonQueue = new std::queue<Job>();
	}

	JobsQueue::~JobsQueue(){
		delete _jsonQueue;
	}

	void JobsQueue::push(string object,string time_stamp){
		if(object!=""&&time_stamp!=""){

			this->_jobQueueMutex.lock();

			Job new_incoming_data(object,time_stamp);

			this->_jsonQueue->push(new_incoming_data);

			this->_jobQueueMutex.unlock();
		}
	}

	Job JobsQueue::get(){
		Job recvJob("","");

		this->_jobQueueMutex.lock();

		if(!this->_jsonQueue->empty()){
			recvJob =  this->_jsonQueue->front();
			this->_jsonQueue->pop();
		}
		this->_jobQueueMutex.unlock();

		return recvJob;
	}
}