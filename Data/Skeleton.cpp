#include "Skeleton.h"

namespace MultipleKinectsPlatformServer{

	Skeleton::Skeleton(Json::Value raw_skeleton_json){

		Json::Value joints_json = raw_skeleton_json.get("Joints",NULL);

		Json::Value client_id = raw_skeleton_json.get("clientId",NULL);

		Json::Value pos_x = raw_skeleton_json.get("pos_x",NULL);

		Json::Value pos_y = raw_skeleton_json.get("pos_y",NULL);

		Json::Value pos_z = raw_skeleton_json.get("pos_z",NULL);

		Json::Value sensor_id = raw_skeleton_json.get("sensorId",NULL);

		Json::Value skeleton_id = raw_skeleton_json.get("skeleton",NULL);

		for(unsigned short joint=0;joint<joints_json.size();joint++){

			Joint newJoint(joints_json[joint].get("X",NULL).asDouble(),joints_json[joint].get("Y",NULL).asDouble() ,joints_json[joint].get("Z",NULL).asDouble());

			this->joints.push_back(newJoint);
		}

		this->client_id = client_id.asUInt();
		this->pos_x = pos_x.asDouble();
		this->pos_y = pos_y.asDouble();
		this->pos_z = pos_z.asDouble();
		this->skeleton_id = skeleton_id.asUInt();
		this->sensor_id = sensor_id.asString();
	}

	Skeleton::~Skeleton(){

	}


}