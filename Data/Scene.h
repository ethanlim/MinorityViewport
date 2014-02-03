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
			
			unsigned int _ordering;
			bool _calibrated;

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
			bool GetCalibration();
			void SetCalibration(bool calibrated);

			void Update(Skeleton newPerson);
			void Clear();
			long GetFirstSkeletonObservedTime_ms();

			map<unsigned short,Skeleton> GetSkeletons();
			map<unsigned short,long> GetTimeStamps();

			string ToJSON();
	};
}

#endif SCENE_H