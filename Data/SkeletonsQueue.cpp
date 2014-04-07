#include "SkeletonsQueue.h"

namespace MultipleDepthSensorsServer{
	
	SkeletonsQueue::SkeletonsQueue(){
		this->skeleton_queue = new std::queue<Skeleton>();
	}

	SkeletonsQueue::~SkeletonsQueue(){
		delete this->skeleton_queue;
	}

	void SkeletonsQueue::push(Skeleton newSkeleton){
		this->skeleton_queue->push(newSkeleton);
	}

	Skeleton SkeletonsQueue::pop(){

		Skeleton recvSkeleton = this->skeleton_queue->front();

		this->skeleton_queue->pop();

		return recvSkeleton;
	}

	unsigned int SkeletonsQueue::get_size(){

		return this->skeleton_queue->size();
	}
}