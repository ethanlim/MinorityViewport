
#include <string>

using namespace std;

namespace MultipleKinectsPlatformServer{

	class Joint{
		public:
			enum JointType
			{
				AnkleLeft,
				AnkleRight,
				ElbowLeft,
				ElbowRight,
				FootLeft,
				FootRight,
				HandLeft,
				HandRight,
				Head,
				HipCenter,
				HipLeft,
				HipRight,
				KneeLeft,
				KneeRight,
				ShoulderCenter,
				ShoulderLeft,
				ShoulderRight,
				Spine,
				WristLeft,
				WristRight
			} Type;

			Joint(JointType type, double x_pos,double y_pos,double z_pos);
			~Joint();
			double X,Y,Z;
		private:
	};

}