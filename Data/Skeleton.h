#ifndef SKELETON_H
#define SKELETON_H

#include "JSON/json.h"

/*********************/
/*		OPENCV		 */
/*********************/
#include <opencv2/core/core.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <mutex>
#include "Joint.h"

using namespace cv;
using namespace std;

namespace MinorityViewport{
	
	class Skeleton{
		private:
			unsigned int client_id;
			unsigned short skeleton_id;	//id given by sensor
			string sensor_id;
			long time_stamp;

			bool shared;

		public:
			double pos_x;
			double pos_y;
			double pos_z;

			enum Tracked
			{
				PositionOnly,
				Tracked
			} TrackingMode;

			Skeleton(Json::Value raw_skeleton_json,long time_stamp);
			~Skeleton();
			Joint GetJoint(Joint::JointType type);
			
			void SetShared();
			void UnsetShared();
			vector<Joint> joints;
			void SetJoints(vector<Joint> new_joints);
			bool checkFullSetOfJoints();

			Mat ComputeCentroid(ofstream *fileObj,bool writeToFile=false);
			Mat GetCompletePointsVectorMatrix(ofstream *fileObj,bool writeToFile=false);
			void ConvertVectorMatrixtoSkeletonPoints(Mat vectorMatrix);

			unsigned int GetClientId();
			string GetSensorId();
			unsigned short GetSkeletonId();
			
			string ToJSON();
	};
}
#endif
