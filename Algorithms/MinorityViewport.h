#ifndef MINORITYVIEWPORT_H
#define MINORITYVIEWPORT_H

#include <list>

#include "../Data/Skeleton.h"
#include "../Data/Scene.h"

using namespace std;

namespace MultipleKinectsPlatformServer{

	class MinorityViewport{
		private:
		public:
			MinorityViewport();
			~MinorityViewport();
			void LoadSkeleton(const Skeleton newSkeleton);
			void MergeScenes();
			list<Scene> GetScenes();
	};

}

#endif MINORITYVIEWPORT_H