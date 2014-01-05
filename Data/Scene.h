#ifndef SCENE_H
#define SCENE_H

#include <list>
#include "Skeleton.h"

using namespace std;

namespace MultipleKinectsPlatformServer{

	class Scene{
		private:
		public:
			void AddPerson(const Skeleton newPerson);
	};
}

#endif SCENE_H