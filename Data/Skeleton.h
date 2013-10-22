#ifndef SKELETON_H
#define SKELETON_H

#include "JSON\json.h"

#include <vector>
#include "Joint.h"

using namespace std;

namespace MultipleKinectsPlatformServer{

	class Skeleton{
		public:
			Skeleton(Json::Value raw_skeleton_json);
			~Skeleton();
			Joint GetJoint(Joint::JointType type);
		private:
			vector<Joint> joints;
			double pos_x;
			double pos_y;
			double pos_z;
			unsigned int client_id;
			unsigned int skeleton_id;
			string sensor_id;
	};
}
#endif