#ifndef SKELETON_H
#define SKELETON_H

#include "JSON\json.h"

#include <vector>
#include <mutex>
#include "Joint.h"

using namespace std;

namespace MultipleKinectsPlatformServer{

	extern mutex vector_mutex;

	class Skeleton{
		public:
			enum Tracked
			{
				PositionOnly,
				Tracked
			} TrackingMode;

			Skeleton(Json::Value raw_skeleton_json);
			~Skeleton();
			Joint GetJoint(Joint::JointType type);
			double pos_x;
			double pos_y;
			double pos_z;
			vector<Joint> joints;
		private:
			unsigned int client_id;
			unsigned int skeleton_id;
			string sensor_id;
	};
}
#endif