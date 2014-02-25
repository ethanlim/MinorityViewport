#ifndef MINORITYVIEWPORT_H
#define MINORITYVIEWPORT_H

#include <set>
#include <mutex>
#include <map>
#include <vector>
#include <algorithm>

/*********************/
/*		OPENCV		 */
/*********************/
#include <opencv2/core/core.hpp>

#include "../Data/Client.h"
#include "../Data/ClientsList.h"
#include "../Data/Skeleton.h"
#include "../Data/Sensor.h"
#include "../Data/Scene.h"
#include "../Misc/Timer.h"
#include "../Data/JSON/json.h"

using namespace cv;
using namespace std;

namespace MultipleKinectsPlatformServer{

	class MinorityViewport{
		private:
			Timer *_curTime;
			ClientsList *_clients;
			Scene *_globalScene;
			thread *_mergethread;

			mutex _orderedSceneMutex;
			vector<Scene*> _orderedScenes;
			set<Scene*> _scenesSet;

			void RefreshScenesSet();
			void MergeScenes();
		public:
			MinorityViewport(Timer *curTime, ClientsList *clients);
			~MinorityViewport();

			bool CalibrateSceneOrder();
			bool CalibrateScenes(unsigned int sceneAOrder,string skeletonA_json,unsigned int sceneBOrder,string skeletonB_json);
			void LoadSkeleton(Skeleton newSkeleton);

			Scene* GetGlobalScene();
			Scene* GetLocalSceneBySensorId(string sensorId);
	};

}

#endif MINORITYVIEWPORT_H