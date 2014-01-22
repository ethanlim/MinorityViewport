#ifndef SKELETON_H
#define SKELETON_H

#include "JSON/json.h"

#include <vector>
#include <mutex>
#include "Joint.h"

using namespace std;

namespace MultipleKinectsPlatformServer{

	extern std::mutex joints_mutex;

	class Skeleton{
		private:
			unsigned int client_id;
			unsigned short skeleton_id;
			string sensor_id;
		public:
			double pos_x;
			double pos_y;
			double pos_z;

			enum Tracked
			{
				PositionOnly,
				Tracked
			} TrackingMode;

			Skeleton(Json::Value raw_skeleton_json);
			~Skeleton();
			Joint GetJoint(Joint::JointType type);
			
			vector<Joint> joints;
			void SetJoints(vector<Joint> new_joints);

			unsigned int GetClientId();
			string GetSensorId();
			unsigned short GetSkeletonId();
	};
}
#endif
