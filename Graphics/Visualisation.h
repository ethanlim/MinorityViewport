#ifndef VISUALISATION_H
#define VISUALISATION_H

#ifdef WINDOWS
#include <Windows.h>
#else
#endif
#include <string>
#include <gl/GL.h>
#include "glut/glut.h"
#include "../Data/Skeleton.h"

using namespace std;

#define KEY_ESCAPE 27
#define PI 3.14159265f
#define CIRCLE_RAD 0.02f

namespace MultipleKinectsPlatformServer{

	extern Skeleton *global_skeleton;

	class Visualisation{
		public:
			Visualisation(int *argcp, char **argv);
			~Visualisation();
			static const int refresh_interval = 3000;
		private:
			static void Display();
			static void Keyboard(unsigned char key, int mousePositionX, int mousePositionY);
			static void Draw3DCircle(float x_pos, float y_pos, float z_pos,float radius);
			static void Draw3DLine(float x_start_pos,float y_start_pos, float z_start_pos, float x_end_pos, float y_end_pos, float z_end_pos);
			static void Timer(int value);
			bool static CheckPositionNotEmpty(double x_joint,double y_joint, double z_joint);
			void static Visualisation::DrawSkeleton(Skeleton *draw_skeleton);
			int width;
			int height;
			char* title;
			float field_of_view_angle;
			float z_near;
			float z_far;
	};
}
#endif
