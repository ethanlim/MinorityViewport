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

		if(joints_json.size()!=0){

			this->Tracked = Skeleton::Joints;

			for(unsigned short joint=0;joint<joints_json.size();joint++){

				Joint::JointType type = (Joint::JointType)joint;

				Joint newJoint( type,
								joints_json[joint].get("X",NULL).asDouble(),
								joints_json[joint].get("Y",NULL).asDouble() ,
								joints_json[joint].get("Z",NULL).asDouble()
				);

				this->joints.push_back(newJoint);
			}

		}else{
			this->Tracked = Skeleton::PositionOnly;
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

	Joint Skeleton::GetJoint(Joint::JointType type){

		try{

		for(unsigned short joint=0;joint<this->joints.size();joint++){

			if(joints[joint].Type == type){
				return joints[joint];
			}

		}

		}catch(exception ex){

			cout << ex.what();
		}

		return Joint();
	}

}