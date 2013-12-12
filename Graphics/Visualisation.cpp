#include "Visualisation.h"

namespace MultipleKinectsPlatformServer{

	Visualisation::Visualisation(int *argcp, char **argv){
		
		//Window Configurations
		this->width = 1280;
		this->height = 1024;
		this->title = "FYP - MultipleKinectsPlatform";
		this->field_of_view_angle = 45;
		this->z_near = 1.0f;
		this->z_far = 500.0f;
	
		glutInit(argcp, argv);     // GLUT initialization
		glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );						// Display Mode
		glutInitWindowSize(this->width,this->height);									// set window size
		glutInitWindowPosition(0,0);
		glutCreateWindow(this->title);													// create Window
		glutDisplayFunc(&Visualisation::Display);										// register Display Function
		glutIdleFunc(&Visualisation::Display );											// register Idle Function
		glutKeyboardFunc(&Visualisation::Keyboard);

		glMatrixMode(GL_PROJECTION);													// select projection matrix
		glViewport(0, 0, this->width, this->height);									// set the viewport
		glMatrixMode(GL_PROJECTION);													// set matrix mode
		glLoadIdentity();																// reset projection matrix
		GLfloat aspect = (GLfloat) this->width / this->height;
		gluPerspective(this->field_of_view_angle, aspect, this->z_near, this->z_far);	// set up a perspective projection matrix
		glMatrixMode(GL_MODELVIEW);														// specify which matrix is the current matrix
		glShadeModel( GL_SMOOTH );
		glClearDepth( 1.0f );															// specify the clear value for the depth buffer
		glEnable( GL_DEPTH_TEST );
		glDepthFunc( GL_LEQUAL );
		glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );							// specify implementation-specific hints
		glClearColor(0.0, 0.0, 0.0, 1.0);												// specify clear values for the color buffers	

		glutMainLoop();
	}

	Visualisation::~Visualisation(){

	}

	void Visualisation::Display(){

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		     // Clear Screen and Depth Buffer
		glLoadIdentity();
		glTranslatef(0.0f,0.0f,-3.0f);			
		
		try{

			if(global_skeleton!=NULL){

				Visualisation::DrawSkeleton(global_skeleton);

			}

		}catch(exception drawException){

			cout << drawException.what();
		}

		glutSwapBuffers();
	}

	void Visualisation::Keyboard(unsigned char key, int mousePositionX, int mousePositionY){
		 switch (key) 
		  {
			case KEY_ESCAPE:        
			  exit ( 0 );   
			  break;      
			default:      
			  break;
		  }
	}

	void Visualisation::Timer(int value) {
		glutPostRedisplay();													 // Post a paint request to activate display()
		glutTimerFunc(Visualisation::refresh_interval,&Visualisation::Timer, 0); // subsequent timer call at milliseconds
	}

	void Visualisation::Draw3DCircle(float x_pos, float y_pos,float z_pos, float radius){
		glBegin(GL_LINE_LOOP); 
			glColor3f(0.0f,0.75f,0.0f);
			int num_seg = 200;
			for(int ii = 0; ii < num_seg; ii++) 
			{ 
				float theta = 2.0f * PI * float(ii) / float(num_seg);//get the current angle 

				float x = radius * cosf(theta);//calculate the x component 
				float y = radius * sinf(theta);//calculate the y component 

				glVertex3f(x + x_pos, y + y_pos,z_pos);//output vertex 
			} 
		glEnd();
	}

	void Visualisation::Draw3DLine(float x_start_pos,float y_start_pos, float z_start_pos, float x_end_pos, float y_end_pos, float z_end_pos){
		glBegin(GL_LINES);
			glVertex3f(x_start_pos,y_start_pos,z_start_pos);
			glVertex3f(x_end_pos,y_end_pos,z_end_pos);
		glEnd();
	}

	void Visualisation::DrawSkeleton(Skeleton *draw_skeleton){

		if(draw_skeleton!=NULL){
			
			if(draw_skeleton->TrackingMode == Skeleton::PositionOnly){

				if(Visualisation::CheckPositionNotEmpty(draw_skeleton->pos_x,draw_skeleton->pos_y,draw_skeleton->pos_z))
					Visualisation::Draw3DCircle(draw_skeleton->pos_x,draw_skeleton->pos_y,draw_skeleton->pos_z,CIRCLE_RAD);	

			}else if(draw_skeleton->TrackingMode == Skeleton::Tracked){

				if(Visualisation::CheckPositionNotEmpty(draw_skeleton->pos_x,draw_skeleton->pos_y,draw_skeleton->pos_z))
					Visualisation::Draw3DCircle(draw_skeleton->pos_x,draw_skeleton->pos_y,draw_skeleton->pos_z,CIRCLE_RAD);	

				if(Visualisation::CheckPositionNotEmpty(draw_skeleton->GetJoint(Joint::Head).X, draw_skeleton->GetJoint(Joint::Head).Y, draw_skeleton->GetJoint(Joint::Head).Z)){
					Visualisation::Draw3DCircle(draw_skeleton->GetJoint(Joint::Head).X,draw_skeleton->GetJoint(Joint::Head).Y,draw_skeleton->GetJoint(Joint::Head).Z,CIRCLE_RAD);
					Visualisation::Draw3DLine(draw_skeleton->GetJoint(Joint::Head).X,draw_skeleton->GetJoint(Joint::Head).Y,draw_skeleton->GetJoint(Joint::Head).Z,
											  draw_skeleton->GetJoint(Joint::ShoulderCenter).X,draw_skeleton->GetJoint(Joint::ShoulderCenter).Y,draw_skeleton->GetJoint(Joint::ShoulderCenter).Z);
				}

				if(Visualisation::CheckPositionNotEmpty(draw_skeleton->GetJoint(Joint::ShoulderCenter).X, draw_skeleton->GetJoint(Joint::ShoulderCenter).Y, draw_skeleton->GetJoint(Joint::ShoulderCenter).Z)){
					Visualisation::Draw3DCircle(draw_skeleton->GetJoint(Joint::ShoulderCenter).X,draw_skeleton->GetJoint(Joint::ShoulderCenter).Y,draw_skeleton->GetJoint(Joint::ShoulderCenter).Z,CIRCLE_RAD);
					Visualisation::Draw3DLine(draw_skeleton->GetJoint(Joint::ShoulderCenter).X,draw_skeleton->GetJoint(Joint::ShoulderCenter).Y,draw_skeleton->GetJoint(Joint::ShoulderCenter).Z,
											  draw_skeleton->GetJoint(Joint::ShoulderLeft).X,draw_skeleton->GetJoint(Joint::ShoulderLeft).Y,draw_skeleton->GetJoint(Joint::ShoulderLeft).Z);
					Visualisation::Draw3DLine(draw_skeleton->GetJoint(Joint::ShoulderCenter).X,draw_skeleton->GetJoint(Joint::ShoulderCenter).Y,draw_skeleton->GetJoint(Joint::ShoulderCenter).Z,
											  draw_skeleton->GetJoint(Joint::ShoulderRight).X,draw_skeleton->GetJoint(Joint::ShoulderRight).Y,draw_skeleton->GetJoint(Joint::ShoulderRight).Z);
					Visualisation::Draw3DLine(draw_skeleton->GetJoint(Joint::ShoulderCenter).X,draw_skeleton->GetJoint(Joint::ShoulderCenter).Y,draw_skeleton->GetJoint(Joint::ShoulderCenter).Z,
											  draw_skeleton->GetJoint(Joint::Spine).X,draw_skeleton->GetJoint(Joint::Spine).Y,draw_skeleton->GetJoint(Joint::Spine).Z);
				}

				if(Visualisation::CheckPositionNotEmpty(draw_skeleton->GetJoint(Joint::ShoulderLeft).X, draw_skeleton->GetJoint(Joint::ShoulderLeft).Y, draw_skeleton->GetJoint(Joint::ShoulderLeft).Z)){
					Visualisation::Draw3DCircle(draw_skeleton->GetJoint(Joint::ShoulderLeft).X,draw_skeleton->GetJoint(Joint::ShoulderLeft).Y,draw_skeleton->GetJoint(Joint::ShoulderLeft).Z,CIRCLE_RAD);
					Visualisation::Draw3DLine(draw_skeleton->GetJoint(Joint::ShoulderLeft).X,draw_skeleton->GetJoint(Joint::ShoulderLeft).Y,draw_skeleton->GetJoint(Joint::ShoulderLeft).Z,
											  draw_skeleton->GetJoint(Joint::ElbowLeft).X,draw_skeleton->GetJoint(Joint::ElbowLeft).Y,draw_skeleton->GetJoint(Joint::ElbowLeft).Z);
				}

				if(Visualisation::CheckPositionNotEmpty(draw_skeleton->GetJoint(Joint::ElbowLeft).X, draw_skeleton->GetJoint(Joint::ElbowLeft).Y, draw_skeleton->GetJoint(Joint::ElbowLeft).Z)){
					Visualisation::Draw3DCircle(draw_skeleton->GetJoint(Joint::ElbowLeft).X,draw_skeleton->GetJoint(Joint::ElbowLeft).Y,draw_skeleton->GetJoint(Joint::ElbowLeft).Z,CIRCLE_RAD);
					Visualisation::Draw3DLine(draw_skeleton->GetJoint(Joint::ElbowLeft).X,draw_skeleton->GetJoint(Joint::ElbowLeft).Y,draw_skeleton->GetJoint(Joint::ElbowLeft).Z,
											  draw_skeleton->GetJoint(Joint::WristLeft).X,draw_skeleton->GetJoint(Joint::WristLeft).Y,draw_skeleton->GetJoint(Joint::WristLeft).Z);
				}

				if(Visualisation::CheckPositionNotEmpty(draw_skeleton->GetJoint(Joint::WristLeft).X, draw_skeleton->GetJoint(Joint::WristLeft).Y, draw_skeleton->GetJoint(Joint::WristLeft).Z)){
					Visualisation::Draw3DCircle(draw_skeleton->GetJoint(Joint::WristLeft).X,draw_skeleton->GetJoint(Joint::WristLeft).Y,draw_skeleton->GetJoint(Joint::WristLeft).Z,CIRCLE_RAD);
					Visualisation::Draw3DLine(draw_skeleton->GetJoint(Joint::WristLeft).X,draw_skeleton->GetJoint(Joint::WristLeft).Y,draw_skeleton->GetJoint(Joint::WristLeft).Z,
											  draw_skeleton->GetJoint(Joint::HandLeft).X,draw_skeleton->GetJoint(Joint::HandLeft).Y,draw_skeleton->GetJoint(Joint::HandLeft).Z);
				}

				if(Visualisation::CheckPositionNotEmpty(draw_skeleton->GetJoint(Joint::ShoulderRight).X, draw_skeleton->GetJoint(Joint::ShoulderRight).Y, draw_skeleton->GetJoint(Joint::ShoulderRight).Z)){
					Visualisation::Draw3DCircle(draw_skeleton->GetJoint(Joint::ShoulderRight).X,draw_skeleton->GetJoint(Joint::ShoulderRight).Y,draw_skeleton->GetJoint(Joint::ShoulderRight).Z,CIRCLE_RAD);
					Visualisation::Draw3DLine(draw_skeleton->GetJoint(Joint::ShoulderRight).X,draw_skeleton->GetJoint(Joint::ShoulderRight).Y,draw_skeleton->GetJoint(Joint::ShoulderRight).Z,
											  draw_skeleton->GetJoint(Joint::ElbowRight).X,draw_skeleton->GetJoint(Joint::ElbowRight).Y,draw_skeleton->GetJoint(Joint::ElbowRight).Z);
				}

				if(Visualisation::CheckPositionNotEmpty(draw_skeleton->GetJoint(Joint::ElbowRight).X, draw_skeleton->GetJoint(Joint::ElbowRight).Y, draw_skeleton->GetJoint(Joint::ElbowRight).Z)){
					Visualisation::Draw3DCircle(draw_skeleton->GetJoint(Joint::ElbowRight).X,draw_skeleton->GetJoint(Joint::ElbowRight).Y,draw_skeleton->GetJoint(Joint::ElbowRight).Z,CIRCLE_RAD);
					Visualisation::Draw3DLine(draw_skeleton->GetJoint(Joint::ElbowRight).X,draw_skeleton->GetJoint(Joint::ElbowRight).Y,draw_skeleton->GetJoint(Joint::ElbowRight).Z,
											  draw_skeleton->GetJoint(Joint::WristRight).X,draw_skeleton->GetJoint(Joint::WristRight).Y,draw_skeleton->GetJoint(Joint::WristRight).Z);
				}

				if(Visualisation::CheckPositionNotEmpty(draw_skeleton->GetJoint(Joint::WristRight).X, draw_skeleton->GetJoint(Joint::WristRight).Y, draw_skeleton->GetJoint(Joint::WristRight).Z)){
					Visualisation::Draw3DCircle(draw_skeleton->GetJoint(Joint::WristRight).X,draw_skeleton->GetJoint(Joint::WristRight).Y,draw_skeleton->GetJoint(Joint::WristRight).Z,CIRCLE_RAD);
					Visualisation::Draw3DLine(draw_skeleton->GetJoint(Joint::WristRight).X,draw_skeleton->GetJoint(Joint::WristRight).Y,draw_skeleton->GetJoint(Joint::WristRight).Z,
											  draw_skeleton->GetJoint(Joint::HandRight).X,draw_skeleton->GetJoint(Joint::HandRight).Y,draw_skeleton->GetJoint(Joint::HandRight).Z);
				}

				if(Visualisation::CheckPositionNotEmpty(draw_skeleton->GetJoint(Joint::Spine).X, draw_skeleton->GetJoint(Joint::Spine).Y, draw_skeleton->GetJoint(Joint::Spine).Z)){
					Visualisation::Draw3DCircle(draw_skeleton->GetJoint(Joint::Spine).X,draw_skeleton->GetJoint(Joint::Spine).Y,draw_skeleton->GetJoint(Joint::Spine).Z,CIRCLE_RAD);
					Visualisation::Draw3DLine(draw_skeleton->GetJoint(Joint::Spine).X,draw_skeleton->GetJoint(Joint::Spine).Y,draw_skeleton->GetJoint(Joint::Spine).Z,
											  draw_skeleton->GetJoint(Joint::HipCenter).X,draw_skeleton->GetJoint(Joint::HipCenter).Y,draw_skeleton->GetJoint(Joint::HipCenter).Z);
				}

				if(Visualisation::CheckPositionNotEmpty(draw_skeleton->GetJoint(Joint::HipCenter).X, draw_skeleton->GetJoint(Joint::HipCenter).Y, draw_skeleton->GetJoint(Joint::HipCenter).Z)){
					Visualisation::Draw3DCircle(draw_skeleton->GetJoint(Joint::HipCenter).X,draw_skeleton->GetJoint(Joint::HipCenter).Y,draw_skeleton->GetJoint(Joint::HipCenter).Z,CIRCLE_RAD);
					Visualisation::Draw3DLine(draw_skeleton->GetJoint(Joint::HipCenter).X,draw_skeleton->GetJoint(Joint::HipCenter).Y,draw_skeleton->GetJoint(Joint::HipCenter).Z,
											  draw_skeleton->GetJoint(Joint::HipLeft).X,draw_skeleton->GetJoint(Joint::HipLeft).Y,draw_skeleton->GetJoint(Joint::HipLeft).Z);
					Visualisation::Draw3DLine(draw_skeleton->GetJoint(Joint::HipCenter).X,draw_skeleton->GetJoint(Joint::HipCenter).Y,draw_skeleton->GetJoint(Joint::HipCenter).Z,
											  draw_skeleton->GetJoint(Joint::HipRight).X,draw_skeleton->GetJoint(Joint::HipRight).Y,draw_skeleton->GetJoint(Joint::HipRight).Z);
				}

				if(Visualisation::CheckPositionNotEmpty(draw_skeleton->GetJoint(Joint::HipLeft).X, draw_skeleton->GetJoint(Joint::HipLeft).Y, draw_skeleton->GetJoint(Joint::HipLeft).Z)){
					Visualisation::Draw3DCircle(draw_skeleton->GetJoint(Joint::HipLeft).X,draw_skeleton->GetJoint(Joint::HipLeft).Y,draw_skeleton->GetJoint(Joint::HipLeft).Z,CIRCLE_RAD);
					Visualisation::Draw3DLine(draw_skeleton->GetJoint(Joint::HipLeft).X,draw_skeleton->GetJoint(Joint::HipLeft).Y,draw_skeleton->GetJoint(Joint::HipLeft).Z,
											  draw_skeleton->GetJoint(Joint::KneeLeft).X,draw_skeleton->GetJoint(Joint::KneeLeft).Y,draw_skeleton->GetJoint(Joint::KneeLeft).Z);
				}

				if(Visualisation::CheckPositionNotEmpty(draw_skeleton->GetJoint(Joint::HipRight).X, draw_skeleton->GetJoint(Joint::HipRight).Y, draw_skeleton->GetJoint(Joint::HipRight).Z)){
					Visualisation::Draw3DCircle(draw_skeleton->GetJoint(Joint::HipRight).X,draw_skeleton->GetJoint(Joint::HipRight).Y,draw_skeleton->GetJoint(Joint::HipRight).Z,CIRCLE_RAD);
					Visualisation::Draw3DLine(draw_skeleton->GetJoint(Joint::HipRight).X,draw_skeleton->GetJoint(Joint::HipRight).Y,draw_skeleton->GetJoint(Joint::HipRight).Z,
											  draw_skeleton->GetJoint(Joint::KneeRight).X,draw_skeleton->GetJoint(Joint::KneeRight).Y,draw_skeleton->GetJoint(Joint::KneeRight).Z);
				}

				if(Visualisation::CheckPositionNotEmpty(draw_skeleton->GetJoint(Joint::KneeLeft).X, draw_skeleton->GetJoint(Joint::KneeLeft).Y, draw_skeleton->GetJoint(Joint::KneeLeft).Z)){
					Visualisation::Draw3DCircle(draw_skeleton->GetJoint(Joint::KneeLeft).X,draw_skeleton->GetJoint(Joint::KneeLeft).Y,draw_skeleton->GetJoint(Joint::KneeLeft).Z,CIRCLE_RAD);
					Visualisation::Draw3DLine(draw_skeleton->GetJoint(Joint::KneeLeft).X,draw_skeleton->GetJoint(Joint::KneeLeft).Y,draw_skeleton->GetJoint(Joint::KneeLeft).Z,
											  draw_skeleton->GetJoint(Joint::AnkleLeft).X,draw_skeleton->GetJoint(Joint::AnkleLeft).Y,draw_skeleton->GetJoint(Joint::AnkleLeft).Z);
				}

				if(Visualisation::CheckPositionNotEmpty(draw_skeleton->GetJoint(Joint::KneeRight).X, draw_skeleton->GetJoint(Joint::KneeRight).Y, draw_skeleton->GetJoint(Joint::KneeRight).Z)){
					Visualisation::Draw3DCircle(draw_skeleton->GetJoint(Joint::KneeRight).X,draw_skeleton->GetJoint(Joint::KneeRight).Y,draw_skeleton->GetJoint(Joint::KneeRight).Z,CIRCLE_RAD);
					Visualisation::Draw3DLine(draw_skeleton->GetJoint(Joint::KneeRight).X,draw_skeleton->GetJoint(Joint::KneeRight).Y,draw_skeleton->GetJoint(Joint::KneeRight).Z,
											  draw_skeleton->GetJoint(Joint::AnkleRight).X,draw_skeleton->GetJoint(Joint::AnkleRight).Y,draw_skeleton->GetJoint(Joint::AnkleRight).Z);
				}

				if(Visualisation::CheckPositionNotEmpty(draw_skeleton->GetJoint(Joint::AnkleLeft).X,draw_skeleton->GetJoint(Joint::AnkleLeft).Y,draw_skeleton->GetJoint(Joint::AnkleLeft).Z)){
					Visualisation::Draw3DCircle(draw_skeleton->GetJoint(Joint::AnkleLeft).X,draw_skeleton->GetJoint(Joint::AnkleLeft).Y,draw_skeleton->GetJoint(Joint::AnkleLeft).Z,CIRCLE_RAD);
					Visualisation::Draw3DLine(draw_skeleton->GetJoint(Joint::AnkleLeft).X,draw_skeleton->GetJoint(Joint::AnkleLeft).Y,draw_skeleton->GetJoint(Joint::AnkleLeft).Z,
											  draw_skeleton->GetJoint(Joint::FootLeft).X,draw_skeleton->GetJoint(Joint::FootLeft).Y,draw_skeleton->GetJoint(Joint::FootLeft).Z);
				}

				if(Visualisation::CheckPositionNotEmpty(draw_skeleton->GetJoint(Joint::AnkleRight).X, draw_skeleton->GetJoint(Joint::AnkleRight).Y, draw_skeleton->GetJoint(Joint::AnkleRight).Z)){
					Visualisation::Draw3DCircle(draw_skeleton->GetJoint(Joint::AnkleRight).X,draw_skeleton->GetJoint(Joint::AnkleRight).Y,draw_skeleton->GetJoint(Joint::AnkleRight).Z,CIRCLE_RAD);
					Visualisation::Draw3DLine(draw_skeleton->GetJoint(Joint::AnkleRight).X,draw_skeleton->GetJoint(Joint::AnkleRight).Y,draw_skeleton->GetJoint(Joint::AnkleRight).Z,
											  draw_skeleton->GetJoint(Joint::FootRight).X,draw_skeleton->GetJoint(Joint::FootRight).Y,draw_skeleton->GetJoint(Joint::FootRight).Z);
				}

				if(Visualisation::CheckPositionNotEmpty(draw_skeleton->GetJoint(Joint::HandLeft).X, draw_skeleton->GetJoint(Joint::HandLeft).Y, draw_skeleton->GetJoint(Joint::HandLeft).Z))
					Visualisation::Draw3DCircle(draw_skeleton->GetJoint(Joint::HandLeft).X,draw_skeleton->GetJoint(Joint::HandLeft).Y,draw_skeleton->GetJoint(Joint::HandLeft).Z,CIRCLE_RAD);

				if(Visualisation::CheckPositionNotEmpty(draw_skeleton->GetJoint(Joint::HandRight).X, draw_skeleton->GetJoint(Joint::HandRight).Y, draw_skeleton->GetJoint(Joint::HandRight).Z))
					Visualisation::Draw3DCircle(draw_skeleton->GetJoint(Joint::HandRight).X,draw_skeleton->GetJoint(Joint::HandRight).Y,draw_skeleton->GetJoint(Joint::HandRight).Z,CIRCLE_RAD);

				if(Visualisation::CheckPositionNotEmpty(draw_skeleton->GetJoint(Joint::FootLeft).X, draw_skeleton->GetJoint(Joint::FootLeft).Y, draw_skeleton->GetJoint(Joint::FootLeft).Z))
					Visualisation::Draw3DCircle(draw_skeleton->GetJoint(Joint::FootLeft).X,draw_skeleton->GetJoint(Joint::FootLeft).Y,draw_skeleton->GetJoint(Joint::FootLeft).Z,CIRCLE_RAD);

				if(Visualisation::CheckPositionNotEmpty(draw_skeleton->GetJoint(Joint::FootRight).X, draw_skeleton->GetJoint(Joint::FootRight).Y, draw_skeleton->GetJoint(Joint::FootRight).Z))
					Visualisation::Draw3DCircle(draw_skeleton->GetJoint(Joint::FootRight).X,draw_skeleton->GetJoint(Joint::FootRight).Y,draw_skeleton->GetJoint(Joint::FootRight).Z,CIRCLE_RAD);

			}
		}
	}

	bool Visualisation::CheckPositionNotEmpty(double x_joint,double y_joint, double z_joint){
		if(x_joint!=0 && y_joint!=0 && z_joint!=0)
			return true;
		else
			return false;
	}
}