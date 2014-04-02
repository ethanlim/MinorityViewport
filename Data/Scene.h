#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <mutex>
#include <thread>
#include <iostream>
#include <ostream>

/*********************/
/*		OPENCV		 */
/*********************/
#include <opencv2/core/core.hpp>

#include "Skeleton.h"
#include "../Misc/Timer.h"

using namespace std;

namespace MultipleKinectsPlatformServer{

	class Scene{
		private:
			string _sensorId;
			
			// id given by server
			map<unsigned short,Skeleton> _skeletons;
			unsigned int _dimensionX;
			unsigned int _dimensionY;
			unsigned int _dimensionZ;
			unsigned int _ordering;
			bool _calibrated;

			Scene *left;
			Scene *right;

			Mat RMatrix;	//Rotation Matrix that rotate all vectors here to the prev scene
			Mat TMatrix;	//Translation Matrix that translate all vectors here to the prev scenen

			Timer *_timer;
			
			unsigned long _refreshRate_ms;//ms
			mutex _sceneMutex;
			thread *_refreshThread;

			long _firstSkeletonObservedTime_ms;
			Skeleton *calibrationSkeleton;
		public:
			Scene(string sensorId,Timer *time);
			Scene(unsigned int dim_x, unsigned int dim_y,unsigned int dim_z,Timer *time);
			~Scene();

			unsigned int GetOrdering();
			bool GetCalibration();

			void SetOrdering(unsigned int order);
			void SetCalibration(bool calibrated);
			void SetDimensions(unsigned int x, unsigned int y, unsigned int z);
			void SetLeftRightScene(Scene *left, Scene *right);
			void SetCalibrationSkeleton(Skeleton *skeleton);
			
			Mat GetRMatrix(ofstream *fileObj,bool writeToFile);
			Mat GetTMatrix(ofstream *fileObj,bool writeToFile);
			Scene* GetLeftFrame();
			Scene* GetRightFrame();
			void SetRAndT(Mat R,Mat T);

			void Update(unsigned short serverSkeletonId, Skeleton newPerson);
			void Remove(unsigned short serverSkeletonId);
			void Clear();
			void ManualClear();
			long GetFirstSkeletonObservedTime_ms();

			map<unsigned short,Skeleton> GetSkeletons();
			map<unsigned short,long> GetTimeStamps();

			string ToJSON();
	};
}

#endif SCENE_H