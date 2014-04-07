#ifndef MINORITYVIEWPORT_H
#define MINORITYVIEWPORT_H

#include <set>
#include <mutex>
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>

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

namespace MultipleDepthSensorsServer{

	class MinorityViewport{
		private:
			Timer *_curTime;
			ClientsList *_clients;
			Scene *_globalScene;
			thread *_mergethread;
			
			mutex _orderedSceneMutex;

			vector<Scene*>	_orderedScenes;
			set<Scene*>		_scenesSet;

			void RefreshScenesSet();
			void MergeScenes();
			Mat TransformSkeletonMatrix(Mat bodyFramesSkeleton,Mat R, Mat T);

			//Debugging Variables
			ofstream *mergingLogFile;
		public:
			MinorityViewport(Timer *curTime, ClientsList *clients);
			~MinorityViewport();

			unsigned int RegisterClient(string phyLocation, string ipAddr);
			string GetClientListing();
			void DeregisterClient(unsigned int clientId);
			void RegisterSensors(unsigned int clientId,string rawSensorsList);

			bool CalibrateSceneOrder();
			bool CalibrateScenes(unsigned int sceneAOrder,string skeletonsA_json,unsigned int sceneBOrder,string skeletonsB_json);
			void ProcessSensorData(string sensorData);
			void LoadSkeleton(Skeleton newSkeleton);

			Scene* GetGlobalScene();
			Scene* GetLocalSceneBySensorId(string sensorId);

			void ReportStatus(string msg);
	};

}

#endif MINORITYVIEWPORT_H