#include "Joint.h"

namespace MultipleKinectsPlatformServer{

	Joint::Joint():Type(JointType::None),X(0),Y(0),Z(0){
	}

	Joint::Joint(JointType type, double x_pos,double y_pos,double z_pos,Mode mode):Type(type),X(x_pos),Y(y_pos),Z(z_pos),TrackingMode(mode){

	}

	Joint::~Joint(){
	}

	string Joint::ToJSON(){
		string json;

		json+="{";

		json+="\"type:\":";
		json+=to_string(this->Type);
		json+=",";

		json+="\"X\":";
		json+=to_string(this->X);
		json+=",";

		json+="\"Y\":";
		json+=to_string(this->Y);
		json+=",";

		json+="\"Z\":";
		json+=to_string(this->Z);

		json += "}";

		return json;
	}
}