#ifndef MINORITYVIEWPORT_H
#define MINORITYVIEWPORT_H

#include <list>

#include "../Data/ClientsList.h"
#include "../Data/Skeleton.h"
#include "../Data/Scene.h"

using namespace std;

namespace MultipleKinectsPlatformServer{

	class MinorityViewport{
		private:
			ClientsList *activeClients;

		public:
			MinorityViewport(ClientsList *clients);
			~MinorityViewport();
			void Calibrate();
			void LoadSkeleton(const Skeleton newSkeleton);
			void MergeScenes();
			list<Scene> GetScenes();
	};

}

#endif MINORITYVIEWPORT_H