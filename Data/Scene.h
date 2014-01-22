#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <mutex>
#include <thread>

#include "Skeleton.h"
#include "../Misc/Timer.h"

using namespace std;

namespace MultipleKinectsPlatformServer{

	class Scene{
		private:
			map<unsigned short,Skeleton> _skeletons;
			map<unsigned short,long> _timeStamps;
			
			unsigned int _ordering;

			Timer *_curTime;
			
			unsigned short _refreshRate_ms;
			mutex _sceneMutex;
			thread *refreshThread;

			long _firstSkeletonObservedTime_ms;
		public:
			Scene(Timer *time);
			~Scene();

			unsigned int GetOrdering();
			void SetOrdering(unsigned int order);

			void Update(Skeleton newPerson,long timeStamp);
			void Clear();
			long GetFirstSkeletonObservedTime_ms();
	};
}

#endif SCENE_H