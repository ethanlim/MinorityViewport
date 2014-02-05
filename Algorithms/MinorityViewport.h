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
			ClientsList *_clients;

			vector<Sensor*> _sensors;
			vector<Scene*> _orderedScenes;
			set<Scene*> _scenesSet;

			Timer *_curTime;

			void MergeScenes();
		public:
			MinorityViewport(Timer *curTime, ClientsList *clients);
			~MinorityViewport();

			bool CalibrateSceneOrder();
			string GetClientOrdering();
			void LoadSkeleton(Skeleton newSkeleton);

			void RefreshScenesSet();

			Scene* GetGlobalScene();
			Scene* GetLocalScene(string sensorId);
			string SceneToJSON(Scene* convertingScene);
	};

}

#endif MINORITYVIEWPORT_H