#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "Skeleton.h"

using namespace std;

namespace MultipleKinectsPlatformServer{

	class Scene{
		private:
			map<unsigned short,Skeleton> _skeletons;
			map<unsigned short,long> _timeStamps;
		public:
			Scene();
			~Scene();
			void Update(Skeleton newPerson,long timeStamp);
	};
}

#endif SCENE_H