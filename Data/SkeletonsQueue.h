#ifndef SKELETONSQUEUE_H
#define SKELETONSQUEUE_H

#include <queue>
#include "Skeleton.h"

using namespace std;

namespace MinorityViewport{

	class SkeletonsQueue{
		public:
			SkeletonsQueue();
			~SkeletonsQueue();
			void push(Skeleton newSkeleton);
			Skeleton pop();
			unsigned int get_size();
		private:
			queue<Skeleton> *skeleton_queue;
	};
}

#endif