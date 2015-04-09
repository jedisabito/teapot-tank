/**********************************
 *File: teapot_tank.c
 *Name: Joe DiSabito
 *Date: 11/17/13
 *Class: CSCI343
 *Assignment: FINAL
 *Purpose: teapot_tank.c draws
 *in a 3D environment a tank
 *that can be controlled by
 *the user through keyboard input.
 *The camera can be manipulated
 *by the user and Turret Vision
 *can be toggled, where the camera
 *is moved next to the turret.
 *With spacebar, the user can
 *fire the teapot tank.
 *********************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>

#define PI 3.14159
#define GRASS "grass.ppm"

typedef GLfloat point3[3];
GLfloat spin = 0.0;

GLfloat tank_move = 0.0;
GLfloat turret_rotate = 0.0;
GLfloat turret_elevate = 0.0;

GLfloat camera_x = 0.0;
GLfloat camera_y = 80.0;
GLfloat camera_z = -300.0;

GLfloat teapot_x, teapot_y, teapot_z;

GLuint imageID[2];

point3 turret_tip;

int fire = 0;
int turretvision = 0;

GLfloat t = 1.0;
GLfloat t_increment = 0.17;

int width, height;
GLubyte *imageData;

void teapot(void);

void myinit(void)
{
        GLfloat mat_specular[] = {0.0, 0.3, 0.0, 1.0};
        GLfloat mat_diffuse[] = {0.0, 0.3, 0.0, 1.0};
        GLfloat mat_ambient[] = {0.0, 0.3, 0.0, 1.0};
	GLfloat mat_shininess[] = {200.0};

        GLfloat light_ambient[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light0_pos[] = {1.0, 2.0, 4.0, 0.0};
	
	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);

	glEnable(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glColor3f(0.0, 0.5, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(75, 1, 1, 2000);
	glMatrixMode(GL_MODELVIEW);

	//prints out instructions
	printf("Instructions:\n");
	printf("Move tank forward/backward: F and J\n");
	printf("Rotate turret clk/ctrclkwise: X and Z\n");
	printf("Raise/lower turret: U and I\n");
	printf("Move camera closer/farther: B and N\n");
	printf("Change camera elevation: A and S\n");
	printf("Toggle Turret Vision (TM): P\n");
	printf("Increase/decrease teapot velocity: 1 and 2\n");
	printf("Fire teapot: SPACEBAR\n");
}

//pre: three 3D points to find the cross product, one to store the normal
//post: none
//takes the cross product of the three points a, b, and c,
//storing the result in d, then normalizing d to length one
void cross(point3 a, point3 b, point3 c, point3 d){
        //finds the cross product, storing each component in d
	d[0] = (b[1] - a[1])*(c[2] - a[2]) - (b[2] - a[2]) * (c[1] - a[1]);
	d[1] = (b[2] - a[2])*(c[0] - a[0]) - (b[0] - a[0]) * (c[2] - a[2]);
	d[2] = (b[0] - a[0])*(c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0]);
	
	//normalizes d
	float length = sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
	d[0] = d[0] / length;
	d[1] = d[1] / length;
	d[2] = d[2] / length;
}

//pre: three 3D vectors to find the average of, one to store the average
//post: none
//average3 finds the average normal, then normalizes its length to 1 
void average3(point3 a, point3 b, point3 c, point3 avg){
  //stores the sums of the xyz components in avg
  avg[0] = a[0] + b[0] + c[0];
  avg[1] = a[1] + b[1] + c[1];
  avg[2] = a[2] + b[2] + c[2];

  //divides by magnitude to find average
  float length = sqrt(avg[0] * avg[0] + avg[1] * avg[1] + avg[2] * avg[2]);
  avg[0] = avg[0] / length;
  avg[1] = avg[1] / length;
  avg[2] = avg[2] / length;
}

//pre: four 3D vectors to find the average of, one to store the average
//post: none
//average4 finds the average normal, then normalizes its length to 1 
void average4(point3 a, point3 b, point3 c, point3 d, point3 avg){
  //stores the sums of the xyz components in avg
  avg[0] = a[0] + b[0] + c[0] + d[0];
  avg[1] = a[1] + b[1] + c[1] + d[1];
  avg[2] = a[2] + b[2] + c[2] + d[2];

  //divides by the magnitude to find average
  float length = sqrt(avg[0] * avg[0] + avg[1] * avg[1] + avg[2] * avg[2]);
  avg[0] = avg[0] / length;
  avg[1] = avg[1] / length;
  avg[2] = avg[2] / length;
}

//display function displays smooth shaded 3D pyramid
void display(void){

	
	/*Note use of Depth buffer bit for hidden surface removal*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	glLoadIdentity();

	//if Turret Vision is not toggled, camera is placed looking at tank
	//if toggled, looking in direction of turret
	if (turretvision == 0){
	  gluLookAt(camera_x, camera_y, camera_z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	}else{
	  gluLookAt(turret_tip[0] / 1.2, turret_tip[1] / 1.2 + 11.0, turret_tip[2] / 1.2, 
		    turret_tip[0], turret_tip[1], turret_tip[2], 
		    0, 1, 0);
	}

	//shifts scene by tank_move (minus tank) to create illusion of movement
	glPushMatrix();
	glTranslatef(0, 0, -tank_move);

	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_LIGHTING);

	glColor3f(0.0, 0.0, 0.0);

	//draws road and nearby grass
	glBegin(GL_QUADS);
	glVertex3f(-75.0, -90.0, -1000.0);
	glVertex3f(-5.0, -90.0, -1000.0);
	glVertex3f(-5.0, -90.0, 1000.0);
	glVertex3f(-75.0, -90.0, 1000.0);

	glVertex3f(75.0, -90.0, -1000.0);
	glVertex3f(5.0, -90.0, -1000.0);
	glVertex3f(5.0, -90.0, 1000.0);
	glVertex3f(75.0, -90.0, 1000.0);

	//draws shorter target posts
	glColor3f(0.2, 0.0, 0.0);

	glVertex3f(400.0, -90.0, -708.0);
	glVertex3f(400.0, 30.0, -708.0);
	glVertex3f(400.0, 30.0, -658.0);
	glVertex3f(400.0, -90.0, -658.0);

	glVertex3f(400.0, -90.0, -366.0);
	glVertex3f(400.0, 30.0, -366.0);
	glVertex3f(400.0, 30.0, -316.0);
	glVertex3f(400.0, -90.0, -316.0);

	glVertex3f(400.0, -90.0, -25.0);
	glVertex3f(400.0, 30.0, -25.0);
	glVertex3f(400.0, 30.0, 25.0);
	glVertex3f(400.0, -90.0, 25.0);

	glVertex3f(400.0, -90.0, 317.0);
	glVertex3f(400.0, 30.0, 317.0);
	glVertex3f(400.0, 30.0, 367.0);
	glVertex3f(400.0, -90.0, 367.0);

	glVertex3f(400.0, -90.0, 658.0);
	glVertex3f(400.0, 30.0, 658.0);
	glVertex3f(400.0, 30.0, 708.0);
	glVertex3f(400.0, -90.0, 708.0);

	glColor3f(1.0, 0.0, 0.0);

        glVertex3f(400.0, 30.0, 633.0);
	glVertex3f(400.0, 100.0, 633.0);
	glVertex3f(400.0, 100.0, 733.0);
	glVertex3f(400.0, 30.0, 733.0);

        glVertex3f(400.0, 30.0, 292.0);
	glVertex3f(400.0, 100.0, 292.0);
	glVertex3f(400.0, 100.0, 392.0);
	glVertex3f(400.0, 30.0, 392.0);

        glVertex3f(400.0, 30.0, -50.0);
	glVertex3f(400.0, 100.0, -50.0);
	glVertex3f(400.0, 100.0, 50.0);
	glVertex3f(400.0, 30.0, 50.0);

        glVertex3f(400.0, 30.0, -292.0);
	glVertex3f(400.0, 100.0, -292.0);
	glVertex3f(400.0, 100.0, -392.0);
	glVertex3f(400.0, 30.0, -392.0);

        glVertex3f(400.0, 30.0, -633.0);
	glVertex3f(400.0, 100.0, -633.0);
	glVertex3f(400.0, 100.0, -733.0);
	glVertex3f(400.0, 30.0, -733.0);

	//draws taller target posts
	glColor3f(0.2, 0.0, 0.0);

	glVertex3f(-400.0, -90.0, -708.0);
	glVertex3f(-400.0, 80.0, -708.0);
	glVertex3f(-400.0, 80.0, -658.0);
	glVertex3f(-400.0, -90.0, -658.0);

	glVertex3f(-400.0, -90.0, -366.0);
	glVertex3f(-400.0, 80.0, -366.0);
	glVertex3f(-400.0, 80.0, -316.0);
	glVertex3f(-400.0, -90.0, -316.0);

	glVertex3f(-400.0, -90.0, -25.0);
	glVertex3f(-400.0, 80.0, -25.0);
	glVertex3f(-400.0, 80.0, 25.0);
	glVertex3f(-400.0, -90.0, 25.0);

	glVertex3f(-400.0, -90.0, 317.0);
	glVertex3f(-400.0, 80.0, 317.0);
	glVertex3f(-400.0, 80.0, 367.0);
	glVertex3f(-400.0, -90.0, 367.0);

	glVertex3f(-400.0, -90.0, 658.0);
	glVertex3f(-400.0, 80.0, 658.0);
	glVertex3f(-400.0, 80.0, 708.0);
	glVertex3f(-400.0, -90.0, 708.0);

	glColor3f(1.0, 0.0, 0.0);

        glVertex3f(-400.0, 80.0, 633.0);
	glVertex3f(-400.0, 150.0, 633.0);
	glVertex3f(-400.0, 150.0, 733.0);
	glVertex3f(-400.0, 80.0, 733.0);

        glVertex3f(-400.0, 80.0, 292.0);
	glVertex3f(-400.0, 150.0, 292.0);
	glVertex3f(-400.0, 150.0, 392.0);
	glVertex3f(-400.0, 80.0, 392.0);

        glVertex3f(-400.0, 80.0, -50.0);
	glVertex3f(-400.0, 150.0, -50.0);
	glVertex3f(-400.0, 150.0, 50.0);
	glVertex3f(-400.0, 80.0, 50.0);

        glVertex3f(-400.0, 80.0, -292.0);
	glVertex3f(-400.0, 150.0, -292.0);
	glVertex3f(-400.0, 150.0, -392.0);
	glVertex3f(-400.0, 80.0, -392.0);

        glVertex3f(-400.0, 80.0, -633.0);
	glVertex3f(-400.0, 150.0, -633.0);
	glVertex3f(-400.0, 150.0, -733.0);
	glVertex3f(-400.0, 80.0, -733.0);      

	glColor3f(1.0, 1.0, 0.0);

	glVertex3f(-5.0, -90.0, -1000.0);
	glVertex3f(5.0, -90.0, -1000.0);
	glVertex3f(5.0, -90.0, 1000.0);
	glVertex3f(-5.0, -90.0, 1000.0);

	glColor3f(0.0, 0.5, 0.0);

	glVertex3f(400.0, -90.0, -1000.0);
        glVertex3f(75.0, -90.0, -1000.0);
	glVertex3f(75.0, -90.0, 1000.0);
	glVertex3f(400.0, -90.0, 1000.0);
       
	glVertex3f(-400.0, -90.0, -1000.0);
        glVertex3f(-75.0, -90.0, -1000.0);
	glVertex3f(-75.0, -90.0, 1000.0);
	glVertex3f(-400.0, -90.0, 1000.0);  
	

	glEnd();
	
	glPopMatrix();
	
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glColor3f(0.0, 0.3, 0.0);

	//shifts tank to tank_move
	//glPushMatrix();
	//glTranslatef(0.0, 0.0, tank_move);

	/*Note 3D points here.  Set up array of 3D vertices*/
	typedef GLfloat point3[3];

	point3 vertices[8] = { {75.0, 50.0, -100.0},//0
			       {-75.0, 50.0, -100.0},//1
			       {-75.0, -50.0, -100.0},//2
			       {75.0, -50.0, -100.0},//3
			       {75.0, -50.0, 100.0}, //4
			       {75.0, 50.0, 100.0},//5
			       {-75.0, 50.0, 100.0}, //6
			       {-75.0, -50.0, 100.0}};//7

	//array to store the normal vectors of each face
	point3 face_normals[6];
	//calls cross to compute normals of each face
	cross(vertices[2], vertices[3], vertices[1], face_normals[0]);
	cross(vertices[3], vertices[4], vertices[0], face_normals[1]);
	cross(vertices[4], vertices[7], vertices[5], face_normals[2]);	
	cross(vertices[7], vertices[2], vertices[6], face_normals[3]);
	cross(vertices[3], vertices[2], vertices[4], face_normals[4]);
	cross(vertices[1], vertices[0], vertices[6], face_normals[5]);

	point3 normAvg[8];
	//calls average3 function to find the normal for
	//each vertex by averaging the face normals of each connecting
	//side of the vertex

	average3(face_normals[0], face_normals[1], face_normals[5], normAvg[0]);
	average3(face_normals[0], face_normals[3], face_normals[5], normAvg[1]);
	average3(face_normals[0], face_normals[3], face_normals[4], normAvg[2]);
	average3(face_normals[0], face_normals[1], face_normals[4], normAvg[3]);
	average3(face_normals[1], face_normals[2], face_normals[4], normAvg[4]);
	average3(face_normals[1], face_normals[2], face_normals[5], normAvg[5]);
	average3(face_normals[2], face_normals[3], face_normals[5], normAvg[6]);
	average3(face_normals[2], face_normals[3], face_normals[4], normAvg[7]);

	/*Draws tank body*/
	glBegin(GL_QUADS);
	        glNormal3fv(normAvg[0]);
		glVertex3fv(vertices[0]);
		glNormal3fv(normAvg[1]);  
		glVertex3fv(vertices[1]);
		glNormal3fv(normAvg[2]);
		glVertex3fv(vertices[2]);
		glNormal3fv(normAvg[3]);
		glVertex3fv(vertices[3]);
	glEnd();	
	
	glBegin(GL_QUADS);
	        glNormal3fv(normAvg[5]);
		glVertex3fv(vertices[5]); 
		glNormal3fv(normAvg[0]);
		glVertex3fv(vertices[0]);
	       	glNormal3fv(normAvg[3]);
		glVertex3fv(vertices[3]);
		glNormal3fv(normAvg[4]);
		glVertex3fv(vertices[4]);
	glEnd();
	
	glBegin(GL_QUADS);
	        glNormal3fv(normAvg[6]);
		glVertex3fv(vertices[6]); 
		glNormal3fv(normAvg[5]);
		glVertex3fv(vertices[5]);
		glNormal3fv(normAvg[4]);
		glVertex3fv(vertices[4]);
		glNormal3fv(normAvg[7]);
		glVertex3fv(vertices[7]);
	glEnd();
	
	glBegin(GL_QUADS);
	        glNormal3fv(normAvg[1]);
		glVertex3fv(vertices[1]);
		glNormal3fv(normAvg[6]);
		glVertex3fv(vertices[6]);
		glNormal3fv(normAvg[7]);
		glVertex3fv(vertices[7]);
		glNormal3fv(normAvg[2]);
		glVertex3fv(vertices[2]);
	glEnd();

	glBegin(GL_QUADS);
	        glNormal3fv(normAvg[3]);
		glVertex3fv(vertices[3]); 
		glNormal3fv(normAvg[2]);
		glVertex3fv(vertices[2]);
		glNormal3fv(normAvg[7]);
		glVertex3fv(vertices[7]);
		glNormal3fv(normAvg[4]);
		glVertex3fv(vertices[4]);
	glEnd();

	glBegin(GL_QUADS);
		glNormal3fv(normAvg[5]);
		glVertex3fv(vertices[5]); 
		glNormal3fv(normAvg[6]);
		glVertex3fv(vertices[6]);
		glNormal3fv(normAvg[1]);
		glVertex3fv(vertices[1]);
		glNormal3fv(normAvg[0]);
		glVertex3fv(vertices[0]);
	glEnd();
	
	//end tank body

	//sets light properties to gray

	glColor3f(0.3, 0.3, 0.3);

	/*tank treads 1*/

	//tread 1 vertices
        point3 vertices1[8] = { {-35.0, -50.0, -100.0},//0
			{-75.0, -50.0, -100.0},//1
			{-75.0, -90.0, -75.0},//2
			{-35.0, -90.0, -75.0},//3
			{-35.0, -90.0, 75.0}, //4
			{-35.0, -50.0, 100.0},//5
			{-75.0, -50.0, 100.0}, //6
			{-75.0, -90.0, 75.0}};//7

	//array to store the normal vectors of each face
	//calls cross to compute normals of each face
	cross(vertices1[2], vertices1[3], vertices1[1], face_normals[0]);
	cross(vertices1[3], vertices1[4], vertices1[0], face_normals[1]);
	cross(vertices1[4], vertices1[7], vertices1[5], face_normals[2]);	
	cross(vertices1[7], vertices1[2], vertices1[6], face_normals[3]);
	cross(vertices1[3], vertices1[2], vertices1[4], face_normals[4]);
	cross(vertices1[1], vertices1[0], vertices1[6], face_normals[5]);

	//calls average3 and average4 functions to find the normal for
	//each vertex by averaging the face normals of each connecting
	//side of the vertex

	average3(face_normals[0], face_normals[1], face_normals[5], normAvg[0]);
	average3(face_normals[0], face_normals[3], face_normals[5], normAvg[1]);
	average3(face_normals[0], face_normals[3], face_normals[4], normAvg[2]);
	average3(face_normals[0], face_normals[1], face_normals[4], normAvg[3]);
	average3(face_normals[1], face_normals[2], face_normals[4], normAvg[4]);
	average3(face_normals[1], face_normals[2], face_normals[5], normAvg[5]);
	average3(face_normals[2], face_normals[3], face_normals[5], normAvg[6]);
	average3(face_normals[2], face_normals[3], face_normals[4], normAvg[7]);

	/*Draw tread 1*/
	glBegin(GL_QUADS);
	        glNormal3fv(normAvg[0]);
		glVertex3fv(vertices1[0]);
		glNormal3fv(normAvg[1]);  
		glVertex3fv(vertices1[1]);
		glNormal3fv(normAvg[2]);
		glVertex3fv(vertices1[2]);
		glNormal3fv(normAvg[3]);
		glVertex3fv(vertices1[3]);
	glEnd();	
	
	glBegin(GL_QUADS);
	        glNormal3fv(normAvg[5]);
		glVertex3fv(vertices1[5]); 
		glNormal3fv(normAvg[0]);
		glVertex3fv(vertices1[0]);
	       	glNormal3fv(normAvg[3]);
		glVertex3fv(vertices1[3]);
		glNormal3fv(normAvg[4]);
		glVertex3fv(vertices1[4]);
	glEnd();
	
	glBegin(GL_QUADS);
	        glNormal3fv(normAvg[6]);
		glVertex3fv(vertices1[6]); 
		glNormal3fv(normAvg[5]);
		glVertex3fv(vertices1[5]);
		glNormal3fv(normAvg[4]);
		glVertex3fv(vertices1[4]);
		glNormal3fv(normAvg[7]);
		glVertex3fv(vertices1[7]);
	glEnd();
	
	glBegin(GL_QUADS);
	        glNormal3fv(normAvg[1]);
		glVertex3fv(vertices1[1]);
		glNormal3fv(normAvg[6]);
		glVertex3fv(vertices1[6]);
		glNormal3fv(normAvg[7]);
		glVertex3fv(vertices1[7]);
		glNormal3fv(normAvg[2]);
		glVertex3fv(vertices1[2]);
	glEnd();

	glBegin(GL_QUADS);
	        glNormal3fv(normAvg[3]);
		glVertex3fv(vertices1[3]); 
		glNormal3fv(normAvg[2]);
		glVertex3fv(vertices1[2]);
		glNormal3fv(normAvg[7]);
		glVertex3fv(vertices1[7]);
		glNormal3fv(normAvg[4]);
		glVertex3fv(vertices1[4]);
	glEnd();

	glBegin(GL_QUADS);
	        glNormal3fv(normAvg[5]);
		glVertex3fv(vertices1[5]); 
		glNormal3fv(normAvg[6]);
		glVertex3fv(vertices1[6]);
		glNormal3fv(normAvg[1]);
		glVertex3fv(vertices1[1]);
		glNormal3fv(normAvg[0]);
		glVertex3fv(vertices1[0]);
	glEnd();

	//END TREAD 1

	//tread 2 vertices
        point3 vertices2[8] = { {75.0, -50.0, -100.0},//0
			{35.0, -50.0, -100.0},//1
			{35.0, -90.0, -75.0},//2
			{75.0, -90.0, -75.0},//3
			{75.0, -90.0, 75.0}, //4
			{75.0, -50.0, 100.0},//5
			{35.0, -50.0, 100.0}, //6
			{35.0, -90.0, 75.0}};//7

	//array to store the normal vectors of each face
	//calls cross to compute normals of each face
	cross(vertices2[2], vertices2[3], vertices2[1], face_normals[0]);
	cross(vertices2[3], vertices2[4], vertices2[0], face_normals[1]);
	cross(vertices2[4], vertices2[7], vertices2[5], face_normals[2]);	
	cross(vertices2[7], vertices2[2], vertices2[6], face_normals[3]);
	cross(vertices2[3], vertices2[2], vertices2[4], face_normals[4]);
	cross(vertices2[1], vertices2[0], vertices2[6], face_normals[5]);

	//calls average3 and average4 functions to find the normal for
	//each vertex by averaging the face normals of each connecting
	//side of the vertex

	average3(face_normals[0], face_normals[1], face_normals[5], normAvg[0]);
	average3(face_normals[0], face_normals[3], face_normals[5], normAvg[1]);
	average3(face_normals[0], face_normals[3], face_normals[4], normAvg[2]);
	average3(face_normals[0], face_normals[1], face_normals[4], normAvg[3]);
	average3(face_normals[1], face_normals[2], face_normals[4], normAvg[4]);
	average3(face_normals[1], face_normals[2], face_normals[5], normAvg[5]);
	average3(face_normals[2], face_normals[3], face_normals[5], normAvg[6]);
	average3(face_normals[2], face_normals[3], face_normals[4], normAvg[7]);

	/*Draw tread 2*/
	glBegin(GL_QUADS);
	        glNormal3fv(normAvg[0]);
		glVertex3fv(vertices2[0]);
		glNormal3fv(normAvg[1]);  
		glVertex3fv(vertices2[1]);
		glNormal3fv(normAvg[2]);
		glVertex3fv(vertices2[2]);
		glNormal3fv(normAvg[3]);
		glVertex3fv(vertices2[3]);
	glEnd();	
	
	glBegin(GL_QUADS);
	        glNormal3fv(normAvg[5]);
		glVertex3fv(vertices2[5]); 
		glNormal3fv(normAvg[0]);
		glVertex3fv(vertices2[0]);
	       	glNormal3fv(normAvg[3]);
		glVertex3fv(vertices2[3]);
		glNormal3fv(normAvg[4]);
		glVertex3fv(vertices2[4]);
	glEnd();
	
	glBegin(GL_QUADS);
	        glNormal3fv(normAvg[6]);
		glVertex3fv(vertices2[6]); 
		glNormal3fv(normAvg[5]);
		glVertex3fv(vertices2[5]);
		glNormal3fv(normAvg[4]);
		glVertex3fv(vertices2[4]);
		glNormal3fv(normAvg[7]);
		glVertex3fv(vertices2[7]);
	glEnd();
	
	glBegin(GL_QUADS);
	        glNormal3fv(normAvg[1]);
		glVertex3fv(vertices2[1]);
		glNormal3fv(normAvg[6]);
		glVertex3fv(vertices2[6]);
		glNormal3fv(normAvg[7]);
		glVertex3fv(vertices2[7]);
		glNormal3fv(normAvg[2]);
		glVertex3fv(vertices2[2]);
	glEnd();

	glBegin(GL_QUADS);
	        glNormal3fv(normAvg[3]);
		glVertex3fv(vertices2[3]); 
		glNormal3fv(normAvg[2]);
		glVertex3fv(vertices2[2]);
		glNormal3fv(normAvg[7]);
		glVertex3fv(vertices2[7]);
		glNormal3fv(normAvg[4]);
		glVertex3fv(vertices2[4]);
	glEnd();

	glBegin(GL_QUADS);
	        glNormal3fv(normAvg[5]);
		glVertex3fv(vertices2[5]); 
		glNormal3fv(normAvg[6]);
		glVertex3fv(vertices2[6]);
		glNormal3fv(normAvg[1]);
		glVertex3fv(vertices2[1]);
		glNormal3fv(normAvg[0]);
		glVertex3fv(vertices2[0]);
	glEnd();

	//END TREAD 2

	glColor3f(0.0, 0.3, 0.0);
	glDisable(GL_COLOR_MATERIAL);

	//rotates turret corresponding to turret_rotate
	glPushMatrix();
	glRotatef(turret_rotate, 0.0, 1.0, 0.0);

	//draws turret base
	glPushMatrix();
	glTranslatef(0.0, 50.0, 0.0);
	glutSolidSphere(50.0, 15, 15);
	glPopMatrix();

	GLUquadricObj* obj = gluNewQuadric();

	//draws turret itself
	glPushMatrix();
	glTranslatef(0.0, 70.0, 0.0);
	glRotatef(turret_elevate, 1.0, 0.0, 0.0);
	gluCylinder(obj, 5, 5, 100, 15, 15);
	glPopMatrix();

	glPopMatrix();

	//if fire flag is raised, display will render teapot at
	//calculated coordinates
	if (fire == 1){

	      glEnable(GL_COLOR_MATERIAL);

	      glColor3f(0.2, 0.2, 0.2);
	      
	      glPushMatrix();
	      
	      glTranslatef(teapot_x, teapot_y, teapot_z);
	      
	      glutSolidTeapot(7.0);
	      
	      glPopMatrix();
	      
	      glColor3f(0.0, 0.3, 0.0);
	      
	      glDisable(GL_COLOR_MATERIAL);
	}

	//glPopMatrix();

	glFlush();
	glutSwapBuffers(); /*Display next buffer*/
}

//teapot function
//calculates coordinates to render the fired teapot, also calculates turret_tip for turret vision
void teapot(void){

  //tip of turret, calculated by using known angles (turret_elevate and turret_rotate) and
  //converting from spherical to Cartesian coordinates
  point3 p1;
  p1[0] = 100.0 * sin(PI / 2 + turret_elevate * PI / 180.0) * sin(turret_rotate * PI / 180.0);
  
  p1[1] = 100.0 * cos(PI / 2 + turret_elevate * PI / 180.0) + 70.0;
  
  p1[2] = 100.0 * sin(PI / 2 + turret_elevate * PI / 180.0) * cos(turret_rotate * PI / 180.0);
  
  turret_tip[0] = p1[0]; 
  turret_tip[1] = p1[1]; 
  turret_tip[2] = p1[2];           

  //if fire flag has been raised by hitting spacebar key
  if (fire == 1){

    //origin of line (center of turret)
    point3 p0 = {0.0, 70.0, 0.0};
    
    //components paramaterized and calculated
    teapot_x = p0[0] + t * (p1[0] - p0[0]);
    teapot_y = p0[1] + t * (p1[1] - p0[1]);
    teapot_z = p0[2] + t * (p1[2] - p0[2]);

    //t is incremented to calculate new coordinates with next call to idle function
    t += t_increment;

    //if teapot is out of bounds, end teapot firing animation
    if (teapot_x <= -400.0 || teapot_x >= 400.0
	|| teapot_y <= -250.0 || teapot_y >= 250.0
        || teapot_z <= -1000.0 || teapot_z >= 1000.0){
      fire = 0; //flag to determine if teapot should be rendered
      t = 1;    //counter is reset
    }
  }

  glutPostRedisplay();
}


//keys function
//allows user to move parts of tank, move the camera, and fire teapot turret
void keys(unsigned char key, int x, int y){

  if (fire == 0){
     //user can move tank in positive or negative z direction (limited)
     if ((key == 'f' || key == 'F') && tank_move < 500) tank_move += 3.0;
     if ((key == 'j' || key == 'J') && tank_move > -500) tank_move -= 3.0;

     //user can rotate turret clockwise or ctrclkwise
     if (key == 'z' || key == 'Z') turret_rotate += 3.0;
     if (key == 'x' || key == 'X') turret_rotate -= 3.0;

     //user can raise or lower the turret (to a certain extent)
     if ((key == 'u' || key == 'U') && turret_elevate >= -30.0)
       turret_elevate -= 3.0;
     if ((key == 'i' || key == 'I') && turret_elevate <= 0.0)
       turret_elevate += 3.0;
  }

  //user can manipulate camera when outside of Turret Vision (TM)
  if (turretvision == 0){

     //user can move camera closer or farther from the tank
     if ((key == 'n' || key == 'N') && camera_z < -200.0) camera_z += 3.0;
     if ((key == 'b' || key == 'B') && camera_z > -600.0) camera_z -= 3.0;
     
     //user can change elevation of camera
     if ((key == 'a' || key == 'A') && camera_y < 200.0) camera_y += 3.0;
     if ((key == 's' || key == 'S') && camera_y > 0.0) camera_y -= 3.0;

  }

  //user can toggle Turret Vision (TM)
  if (key == 'p' || key == 'P'){
    if (turretvision == 0){ 
      turretvision = 1;
    }else{
      turretvision = 0;
    }
  } 

  //user can fire teapot from the tank, flag raised to trigger idle animation
  if (key == ' ') fire = 1;
 
  //increase or decrease firing speed of teapot
  if (key == '1') t_increment += 0.02;
  if (key == '2') t_increment -= 0.02;

  display();
}

int main(int argc, char** argv)
{	
	glutInit(&argc, argv);
	/*Note the GLUT_DOUBLE and GLUT_DEPTH here*/
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500,500);
	glutInitWindowPosition(50,50);
	glutCreateWindow("Teapot Tank"); 

	glEnable(GL_DEPTH_TEST); /*For hidden surface removal*/
        

	myinit();

	glutDisplayFunc(display);
	glutKeyboardFunc(keys);
	glutIdleFunc(teapot);

	glutMainLoop();
	
	return 0;
}
