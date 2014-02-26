#ifndef SKELETON_H
#define SKELETON_H

#include "JSON/json.h"

/*********************/
/*		OPENCV		 */
/*********************/
#include <opencv2/core/core.hpp>

#include <vector>
#include <map>
#include <mutex>
#include "Joint.h"

using namespace cv;
using namespace std;

namespace MultipleKinectsPlatformServer{
	
	class Skeleton{
		private:
			unsigned int client_id;
			unsigned short skeleton_id;	//id given by sensor
			string sensor_id;
			long time_stamp;

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
			
			vector<Joint> joints;
			void SetJoints(vector<Joint> new_joints);

			Mat ComputeCentroid();
			Mat GetCompletePointsVectorMatrix();

			unsigned int GetClientId();
			string GetSensorId();
			unsigned short GetSkeletonId();
			
			string ToJSON();
	};
}
#endif
