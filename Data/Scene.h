#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <mutex>
#include <thread>

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

			map<unsigned short,Skeleton> _skeletons;
			unsigned int _dimensionX;
			unsigned int _dimensionY;
			unsigned int _dimensionZ;
			unsigned int _ordering;
			bool _calibrated;

			Scene *left;
			Scene *right;

			Mat *RotationMatrix;
			Mat *TranslatioMatrix;

			Timer *_curTime;
			
			unsigned short _refreshRate_ms;
			mutex _sceneMutex;
			thread *refreshThread;

			long _firstSkeletonObservedTime_ms;
			Skeleton *calibrationSkeleton;
		public:
			Scene(string sensorId,Timer *time);
			~Scene();

			unsigned int GetOrdering();
			bool GetCalibration();

			void SetOrdering(unsigned int order);
			void SetCalibration(bool calibrated);
			void SetDimensions(unsigned int x, unsigned int y, unsigned int z);
			void SetLeftRightScene(Scene *left, Scene *right);
			void SetCalibrationSkeleton(Skeleton *skeleton);
			void SetRotationTranslationMatrix(Mat R,Mat T);

			void Update(Skeleton newPerson);
			void Clear();
			long GetFirstSkeletonObservedTime_ms();

			map<unsigned short,Skeleton> GetSkeletons();
			map<unsigned short,long> GetTimeStamps();

			string ToJSON();
	};
}

#endif SCENE_H