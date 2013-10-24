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
		Json::Value tracking_mode = raw_skeleton_json.get("trackingMode",NULL);

		vector<Joint> newJoints;

		for(unsigned short joint=0;joint<joints_json.size();joint++){

			Joint::JointType type = (Joint::JointType)joint;

			string trackingMode = joints_json[joint].get("trackedMode",NULL).asString();
			Joint::Mode trackingType;

			if(trackingMode=="Tracked"){
				trackingType = Joint::Mode::Tracked;
			}else if(trackingMode=="NotTracked"){
				trackingType = Joint::Mode::NotTracked;
			}else if(trackingMode=="Inferred"){
				trackingType = Joint::Mode::Inferred;
			}

			Joint newJoint( type,
							joints_json[joint].get("X",NULL).asDouble(),
							joints_json[joint].get("Y",NULL).asDouble(),
							joints_json[joint].get("Z",NULL).asDouble(),
							trackingType
			);

			newJoints.push_back(newJoint);
		}

		vector_mutex.lock();
		this->joints.clear();
		this->joints = newJoints;
		vector_mutex.unlock();

		if(tracking_mode.asString()=="Tracked"){
			this->TrackingMode = Skeleton::Tracked;
		}else{
			this->TrackingMode = Skeleton::PositionOnly;
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

		Joint requestedJoint;
		
		vector_mutex.lock();
		if(this->joints.size()>0){
			requestedJoint = this->joints[(short)type];
		}
		vector_mutex.unlock();
	

		return requestedJoint;
	}

}