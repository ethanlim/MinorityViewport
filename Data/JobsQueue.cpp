#include "JobsQueue.h"

namespace MultipleKinectsPlatformServer{

	JobsQueue::JobsQueue(){
		json_queue = new std::queue<string>();
	}

	JobsQueue::~JobsQueue(){
		delete json_queue;
	}

	void JobsQueue::push(string object){
		this->json_queue->push(object);
	}

	string JobsQueue::pop(){

		string recvJson = this->json_queue->front();

		this->json_queue->pop();

		return recvJson;
	}
}