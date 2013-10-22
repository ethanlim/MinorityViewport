#include "Visualisation.h"

namespace MultipleKinectsPlatformServer{

	Visualisation::Visualisation(int *argcp, char **argv){
		
		//Window Configurations
		this->width = 1280;
		this->height = 720;
		this->title = "OpenGL/GLUT Window";
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
		
		if(global_skeleton!=NULL){
			// Live Drawing
			Visualisation::Draw3DCircle(global_skeleton->GetJoint(Joint::Head).X,global_skeleton->GetJoint(Joint::Head).Y,global_skeleton->GetJoint(Joint::Head).Z,CIRCLE_RAD);		
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

}