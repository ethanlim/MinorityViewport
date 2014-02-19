#ifndef MINORITYVIEWPORT_H
#define MINORITYVIEWPORT_H

#include <set>
#include <map>
#include <vector>
#include <algorithm>

#include "../Data/Client.h"
#include "../Data/ClientsList.h"
#include "../Data/Skeleton.h"
#include "../Data/Sensor.h"
#include "../Data/Scene.h"
#include "../Misc/Timer.h"

using namespace std;

namespace MultipleKinectsPlatformServer{

	class MinorityViewport{
		private:
			Timer *_curTime;
			ClientsList *_clients;
			Scene* _globalScene;

			vector<Scene*> _orderedScenes;
			set<Scene*> _scenesSet;

			void RefreshScenesSet();
			void MergeScenes();
		public:
			MinorityViewport(Timer *curTime, ClientsList *clients);
			~MinorityViewport();

			bool CalibrateSceneOrder();
			void LoadSkeleton(Skeleton newSkeleton);

			Scene* GetGlobalScene();
			Scene* GetLocalSceneBySensorId(string sensorId);
	};

}

#endif MINORITYVIEWPORT_H