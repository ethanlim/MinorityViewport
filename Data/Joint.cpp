#include "Joint.h"

namespace MultipleKinectsPlatformServer{

	Joint::Joint(Joint::JointType type, double x_pos,double y_pos,double z_pos):Type(type),X(x_pos),Y(y_pos),Z(z_pos){

	}

	Joint::~Joint(){
	}

}