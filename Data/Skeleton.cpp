#include "Skeleton.h"

namespace MinorityViewport{

	Skeleton::Skeleton(Json::Value raw_skeleton_json,long time_stamp){

		Json::Value joints_json = raw_skeleton_json.get("joints",NULL);
		Json::Value client_id = raw_skeleton_json.get("client_id",NULL);
		Json::Value pos_x = raw_skeleton_json.get("pos_x",NULL);
		Json::Value pos_y = raw_skeleton_json.get("pos_y",NULL);
		Json::Value pos_z = raw_skeleton_json.get("pos_z",NULL);
		Json::Value sensor_id = raw_skeleton_json.get("sensor_id",NULL);
		Json::Value skeleton_id = raw_skeleton_json.get("skeleton_id",NULL);
		Json::Value tracking_mode = raw_skeleton_json.get("trackingMode",NULL);

		vector<Joint> newJoints;

		for(unsigned short joint=0;joint<joints_json.size();joint++){

			Joint::JointType type = (Joint::JointType)joint;

			Json::Value tracking_mode_joint = joints_json[joint].get("trackedMode",NULL);
			Joint::Mode trackingType;

			if(tracking_mode_joint!=NULL){
				string trackingMode = joints_json[joint].get("trackedMode",NULL).asString();
		
				if(trackingMode=="Tracked"){
					trackingType = Joint::Mode::Tracked;
				}else if(trackingMode=="NotTracked"){
					trackingType = Joint::Mode::NotTracked;
				}else if(trackingMode=="Inferred"){
					trackingType = Joint::Mode::Inferred;
				}
			}else{
				trackingType = Joint::Mode::NotTracked;
			}

			Joint newJoint( type,
							joints_json[joint].get("X",NULL).asDouble(),
							joints_json[joint].get("Y",NULL).asDouble(),
							joints_json[joint].get("Z",NULL).asDouble(),
							trackingType
			);

			newJoints.push_back(newJoint);
		}

		this->SetJoints(newJoints);

		if(tracking_mode!=NULL&&tracking_mode.asString()=="Tracked"){
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
		this->time_stamp = time_stamp;
		this->shared = false;
	}

	Skeleton::~Skeleton(){

	}

	void Skeleton::SetShared(){
		this->shared = true;
	}

	void Skeleton::UnsetShared(){
		this->shared=false;
	}

	bool Skeleton::checkFullSetOfJoints(){

		for(unsigned int joints=0;joints<this->joints.size();joints++){

			if(this->joints[joints].X==0||this->joints[joints].Y==0||this->joints[joints].Z==0){

				return false;
			}
		}

		return true;
	}

	Mat Skeleton::ComputeCentroid(ofstream *fileObj,bool writeToFile){

		double x=this->pos_x,y=this->pos_y,z=this->pos_z;

		for(int vectorIdx = 0;vectorIdx<this->joints.size();vectorIdx+=1){
			
			x += this->joints.at(vectorIdx).X; 
			y += this->joints.at(vectorIdx).Y;
			z += this->joints.at(vectorIdx).Z;

		}

		x/=this->joints.size()+1;
		y/=this->joints.size()+1;
		z/=this->joints.size()+1;
		Mat centroid = (Mat_<double>(1,3) << x,y,z);

		if(writeToFile){
			centroid.convertTo(centroid,CV_64F);
			*fileObj << centroid.at<double>(0,0) << ",";
			*fileObj << centroid.at<double>(0,1) << ",";
			*fileObj << centroid.at<double>(0,2) << " ";

			fileObj->close();
		}

		/* 1x3 matrix*/
		return centroid;
	}

	Mat Skeleton::GetCompletePointsVectorMatrix(ofstream *fileObj,bool writeToFile){

		/* Create a nx3 matrix n is the number of point vectors */
		Mat vectorMatrix = (Mat_<double>(1,3) << this->pos_x, this->pos_y,this->pos_z);
		
		if(writeToFile){
			*fileObj << this->pos_x << ",";
			*fileObj << this->pos_y << ",";
			*fileObj << this->pos_z << " ";
			*fileObj << endl;
		}

		for(int vectorIdx = 0;vectorIdx<this->joints.size();vectorIdx+=1){
			
			Mat row = (Mat_<double>(1,3) <<  this->joints.at(vectorIdx).X, 
											 this->joints.at(vectorIdx).Y,
											 this->joints.at(vectorIdx).Z);

			if(writeToFile){
				*fileObj << this->joints.at(vectorIdx).X << ",";
				*fileObj << this->joints.at(vectorIdx).Y << ",";
				*fileObj << this->joints.at(vectorIdx).Z << " ";
				*fileObj << endl;
			} 

			vectorMatrix.push_back(row);
		}

		if(writeToFile){
			fileObj->close();
		}

		vectorMatrix.convertTo(vectorMatrix,CV_32F);

		return vectorMatrix;
	}

	void Skeleton::ConvertVectorMatrixtoSkeletonPoints(Mat vectorMatrix){

		vectorMatrix.convertTo(vectorMatrix, CV_64F);

		for(unsigned int row=0;row<vectorMatrix.rows;row+=1){

			if(row==0){
				this->pos_x = vectorMatrix.at<double>(row,0);
				this->pos_y = vectorMatrix.at<double>(row,1);
				this->pos_z = vectorMatrix.at<double>(row,2);
			}else{
				this->joints.at(row-1).X = vectorMatrix.at<double>(row,0);
				this->joints.at(row-1).Y = vectorMatrix.at<double>(row,1);
				this->joints.at(row-1).Z = vectorMatrix.at<double>(row,2);
			}
		}
	}

	void Skeleton::SetJoints(vector<Joint> new_joints){
		joints.clear();
		joints = new_joints;
	}

	Joint Skeleton::GetJoint(Joint::JointType type){
		
		Joint bodyJoint;

		if(joints.size()>0){
			bodyJoint = joints[(short)type];
		}

		return bodyJoint;
	}

	string Skeleton::GetSensorId(){
		return this->sensor_id;
	}

	unsigned int Skeleton::GetClientId(){
		return this->client_id;
	}

	unsigned short Skeleton::GetSkeletonId(){
		return this->skeleton_id;
	}

	string Skeleton::ToJSON(){
		string json;

		json+="{";

		json+="\"id\":";
		json+=to_string(this->GetSkeletonId());
		json+=",";

		json+="\"client_id\":";
		json+=to_string(this->GetClientId());
		json+=",";

		json+="\"sensor_id\":";
		string sensorId = this->GetSensorId();
		size_t i = sensorId.find('\\');
		while (i != string::npos)
		{
			string part1 = sensorId.substr(0, i);
			string part2 = sensorId.substr(i + 1);
			sensorId = part1 + "\\\\" + part2;
			i = sensorId.find('\\', i + 4);
		}

		json += "\"" + sensorId + "\"";
		json += ",";

		json+="\"time_stamp\":";
		json+=to_string(this->time_stamp);
		json+=",";

		json+="\"shared\":";
		if(this->shared)
			json += "\"true\"" ;
		else
			json += "\"false\"" ;
		json+=",";

		json+="\"pos_x\":";
		json+=to_string(this->pos_x);
		json+=",";

		json+="\"pos_y\":";
		json+=to_string(this->pos_y);
		json+=",";

		json+="\"pos_z\":";
		json+=to_string(this->pos_z);
		json+=",";

		json+="\"joints\":";
		json+="[";
		vector<Joint>::iterator lastJoint =  this->joints.end();

		if(this->joints.size()>0){
			std::advance(lastJoint,-1);
			for(vector<Joint>::iterator itr = this->joints.begin();itr!=this->joints.end();itr++)
			{
				json += itr->ToJSON();

				if(itr!=lastJoint)
				{
					json += ",";
				}
			}
		}
		json+="]";

		json += "}";

		return json;
	}
}