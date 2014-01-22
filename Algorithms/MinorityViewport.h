#ifndef MINORITYVIEWPORT_H
#define MINORITYVIEWPORT_H

#include <list>

#include "../Data/Client.h"
#include "../Data/ClientsList.h"
#include "../Data/Skeleton.h"
#include "../Data/Scene.h"

using namespace std;

namespace MultipleKinectsPlatformServer{

	class MinorityViewport{
		private:
			ClientsList *_clients;
		public:
			MinorityViewport(ClientsList *clients);
			~MinorityViewport();
			void Calibrate();
			bool CheckCalibration();
			string GetClientOrdering();
			void LoadSkeleton(Skeleton newSkeleton,long timeStamp);
			void MergeScenes();
			list<Scene> GetScenes();
	};

}

#endif MINORITYVIEWPORT_H