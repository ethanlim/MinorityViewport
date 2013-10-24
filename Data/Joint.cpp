#include "Joint.h"

namespace MultipleKinectsPlatformServer{

	Joint::Joint():Type(JointType::None),X(0),Y(0),Z(0){
	}

	Joint::Joint(JointType type, double x_pos,double y_pos,double z_pos,Mode mode):Type(type),X(x_pos),Y(y_pos),Z(z_pos),TrackingMode(mode){

	}

	Joint::~Joint(){
	}

}