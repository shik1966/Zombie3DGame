#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <math.h>
#include <corecrt_math_defines.h>

int WIDTH = 1280;
int HEIGHT = 720;

GLuint tex;
char title[] = "3D Model Loader Sample";

// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 100;

class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	//================================================================================================//
	// Operator Overloading; In C++ you can override the behavior of operators for you class objects. //
	// Here we are overloading the += operator to add a given value to all vector coordinates.        //
	//================================================================================================//
	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;
	}
};

Vector Eye(30, 30, 20);
Vector At(0, 0, 0);
Vector Up(0, 2, 0);

int cameraZoom = 0;
int currentView = 0;  // Default to 0 which could be your free camera or another default view
float playerX = 0.0f, playerY = 0.0f, playerZ = 0.0f;  // Player's position
float playerYaw = 0.0f;  // Player's yaw angle in degrees

bool firstPersonMode = false;  // Initially set to false
// Global variable
double playerAngle = 0;  // Angle in degrees, initialized to 0

float wallHeight = 20.0;  // Height of the walls

// Global variables to track mouse position and player's view direction
float lastX = WIDTH / 2.0;
float lastY = HEIGHT / 2.0;
bool firstMouse = true;
float yaw = -90.0f;   // Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially want the player to look along the z-axis
float pitch = 0.0f;
const float sensitivity = 0.1f;

// Model Variables
Model_3DS model_lamp;
Model_3DS model_couch;
Model_3DS model_couch2;
Model_3DS model_naruto;
Model_3DS model_door;
Model_3DS model_table;


// Textures
GLTexture tex_ground;


//=======================================================================
// Lighting Configuration Function
//=======================================================================
void InitLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources
	glEnable(GL_LIGHT0);

	// Define Light source 0 ambient light
	GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	// Define Light source 0 diffuse light
	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	// Define Light source 0 Specular light
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	// Finally, define light source 0 position in World Space
	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	//*******************************************************************************************//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*******************************************************************************************//

	InitLightSource();

	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

//=======================================================================
// Render Ground Function
//=======================================================================
void RenderGround()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-30, 0, -30);
	glTexCoord2f(5, 0);
	glVertex3f(30, 0, -30);
	glTexCoord2f(5, 5);
	glVertex3f(30, 0, 30);
	glTexCoord2f(0, 5);
	glVertex3f(-30, 0, 30);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

//======================================================================= 
// Render Walls Function
//=======================================================================
void RenderWalls()
{
	glDisable(GL_LIGHTING);  // Disable lighting for solid color walls

	glColor3f(0.7, 0.7, 0.7);  // Set the color of the walls

	glEnable(GL_TEXTURE_2D);  // Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);  // Use the same texture as the ground

	// Draw four walls around the ground
	glBegin(GL_QUADS);

	// Wall along the +X edge
	glNormal3f(-1, 0, 0);  // Normal pointing inward
	glTexCoord2f(0, 0);
	glVertex3f(30, 0, -30);
	glTexCoord2f(1, 0);
	glVertex3f(30, 0, 30);
	glTexCoord2f(1, 1);
	glVertex3f(30, wallHeight, 30);
	glTexCoord2f(0, 1);
	glVertex3f(30, wallHeight, -30);

	// Wall along the -X edge
	glNormal3f(1, 0, 0);  // Normal pointing inward
	glTexCoord2f(0, 0);
	glVertex3f(-30, 0, 30);
	glTexCoord2f(1, 0);
	glVertex3f(-30, 0, -30);
	glTexCoord2f(1, 1);
	glVertex3f(-30, wallHeight, -30);
	glTexCoord2f(0, 1);
	glVertex3f(-30, wallHeight, 30);

	// Wall along the +Z edge
	glNormal3f(0, 0, -1);  // Normal pointing inward
	glTexCoord2f(0, 0);
	glVertex3f(-30, 0, 30);
	glTexCoord2f(1, 0);
	glVertex3f(30, 0, 30);
	glTexCoord2f(1, 1);
	glVertex3f(30, wallHeight, 30);
	glTexCoord2f(0, 1);
	glVertex3f(-30, wallHeight, 30);

	// Wall along the -Z edge
	glNormal3f(0, 0, 1);  // Normal pointing inward
	glTexCoord2f(0, 0);
	glVertex3f(30, 0, -30);
	glTexCoord2f(1, 0);
	glVertex3f(-30, 0, -30);
	glTexCoord2f(1, 1);
	glVertex3f(-30, wallHeight, -30);
	glTexCoord2f(0, 1);
	glVertex3f(30, wallHeight, -30);

	glEnd();

	glEnable(GL_LIGHTING);  // Enable lighting back after rendering walls
	glColor3f(1, 1, 1);  // Reset the color to default after rendering
}

//=======================================================================
// Display Function
//=======================================================================
void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1.0f };
	GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);

	// Draw Ground
	RenderGround();
	// Draw Walls
	RenderWalls();

	// Draw Tree Model
	glPushMatrix();
	glTranslatef(10, 0, 0);
	glScalef(0.7, 0.7, 0.7);
	//model_tree.Draw();
	glPopMatrix();

	// Draw house Model
	glPushMatrix();
	glRotatef(90.f, 1, 0, 0);
	//model_house.Draw();
	glPopMatrix();


	// Draw couch
	glPushMatrix();
	glTranslatef(27.0, 1.0, -28.0);
	glScalef(0.005, 0.005, 0.005);
	glRotatef(0.0f, 1, 0, 0);
	model_couch.Draw();
	glPopMatrix();

	// Draw couch2
	glPushMatrix();
	glTranslatef(10.0, 0.0, -30.0);
	glScalef(5, 5, 5);
	glRotatef(0.0f, 1, 0, 0);
	model_couch2.Draw();
	glPopMatrix();

	// Draw lamp
	glPushMatrix();
	glTranslatef(-29.5, 1, 29.5);
	glScalef(0.005, 0.005, 0.005);
	glRotatef(0.0f, 1, 0, 0);
	model_lamp.Draw();
	glPopMatrix();

	// Draw naruto
	glPushMatrix();
	glTranslatef(playerX, playerY, playerZ);
	glScalef(0.025, 0.025, 0.025);
	glRotatef(playerAngle, 0.0f, 1.0f, 0.0f);  // Rotate Naruto around the y-axis
	model_naruto.Draw();
	glPopMatrix();



	// Draw door
	glPushMatrix();
	glTranslatef(-30.0, 0.0, -15.0);
	glScalef(0.1, 0.1, 0.1);
	glRotatef(0.0f, 1, 0, 0);
	model_door.Draw();
	glPopMatrix();

	// Draw door
	glPushMatrix();
	glTranslatef(-30.0, 0.0, -15.0);
	glScalef(0.1, 0.1, 0.1);
	glRotatef(0.0f, 1, 0, 0);
	model_table.Draw();
	glPopMatrix();


	//sky box
	glPushMatrix();

	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	glTranslated(50, 0, 0);
	glRotated(90, 1, 0, 1);
	glBindTexture(GL_TEXTURE_2D, tex);
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 100, 100, 100);
	gluDeleteQuadric(qobj);


	glPopMatrix();



	glutSwapBuffers();
}

//=======================================================================
// Camera Mode Function
//=======================================================================
void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (double)WIDTH / (double)HEIGHT, 0.1, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (firstPersonMode) {
		// First-person camera setup
		gluLookAt(playerX, playerY + 7.0, playerZ,  // Camera position at player's eye level
			playerX + sin(playerAngle * M_PI / 180.0), playerY + 7.0, playerZ + cos(playerAngle * M_PI / 180.0),  // Look at point
			0.0, 1.0, 0.0);  // Up vector is always straight up
	}
	else {
		switch (currentView) {
		case 1:  // Top View
			gluLookAt(0.0, 50.0, 5.0,
				0.0, 0.0, 0.0,
				1.0, 0.0, 0.0);
			break;
		case 2:  // Side View
			gluLookAt(50.0, 50.0, 0.0,
				0.0, 0.0, 0.0,
				0.0, 1.0, 0.0);
			break;
		case 3:  // Front View
			gluLookAt(0.0, 50.0, 50.0,
				0.0, 0.0, 0.0,
				0.0, 1.0, 0.0);
			break;
		default:  // Free Camera
			gluLookAt(Eye.x, Eye.y, Eye.z,
				At.x, At.y, At.z,
				Up.x, Up.y, Up.z);
			break;
		}
	}
}


void switchCameraView(unsigned char key, int x, int y) {
	switch (key) {
	case '1':
		currentView = 1;  // Top View
		break;
	case '2':
		currentView = 2;  // Side View
		break;
	case '3':
		currentView = 3;  // Front View
		break;
	case '0':
		currentView = 0;  // Free Camera
		break;
	default:
		return; // If the key is not one of the view switchers, ignore it
	}
	setupCamera();  // Update camera setup based on new view
	glutPostRedisplay();  // Redraw the scene with the new camera settings
}

//=======================================================================
// Keyboard Function
//==========================================================
void updatePlayerDirection(float angle) {
	playerAngle = angle;  // Set player angle directly
	if (playerAngle >= 360.0) playerAngle -= 360.0;  // Ensure the angle remains within 0-360 degrees
	if (playerAngle < 0) playerAngle += 360.0;
}

void myKeyboard(unsigned char key, int x, int y) {
	float stepSize = 0.25;  // Movement step size
	float newX, newZ;  // Variables to hold potential new positions

	if (currentView == 0) {  // Only allow movement in free camera mode
		switch (key) {
		case 'u':
			// Move forward
			Eye.x += stepSize * sin(playerAngle * M_PI / 180.0);
			Eye.z += stepSize * cos(playerAngle * M_PI / 180.0);
			break;
		case 'h':
			// Move left
			Eye.x -= stepSize * cos(playerAngle * M_PI / 180.0);
			Eye.z += stepSize * sin(playerAngle * M_PI / 180.0);
			break;
		case 'j':
			// Move backward
			Eye.x -= stepSize * sin(playerAngle * M_PI / 180.0);
			Eye.z -= stepSize * cos(playerAngle * M_PI / 180.0);
			break;
		case 'k':
			// Move right
			Eye.x += stepSize * cos(playerAngle * M_PI / 180.0);
			Eye.z -= stepSize * sin(playerAngle * M_PI / 180.0);
			break;
		}
	}

		// Calculate potential new positions for the player based on key presses
		switch (key) {
		case 'w':
			playerAngle = 270;  // North
			break;
		case 's':
			playerAngle = 90;  // South
			break;
		case 'a':
			playerAngle = 0;  // West
			break;
		case 'd':
			playerAngle = 180;  // East
			break;
		}

		// Compute new positions based on angle
		if (key == 'w' || key == 's' || key == 'a' || key == 'd') {
			newX = playerX + stepSize * sin(playerAngle * M_PI / 180.0);
			newZ = playerZ + stepSize * cos(playerAngle * M_PI / 180.0);

			// Check for collision with the boundary walls (-30 to +30)
			if (newX >= -30.0f && newX <= 30.0f && newZ >= -30.0f && newZ <= 30.0f) {
				playerX = newX;
				playerZ = newZ;
			}
		}

		switch (key) {
		case 'p':
			// Toggle first-person mode
			firstPersonMode = !firstPersonMode;
			break;
		case '1':
		case '2':
		case '3':
		case '0':
			if (!firstPersonMode) {  // Only switch views if not in first-person mode
				switchCameraView(key, x, y);
			}
			break;
		case 27:  // ESC key to quit
			exit(0);
			break;
		}

		setupCamera();  // Update the camera setup
		glutPostRedisplay();  // Redraw the scene with the new settings
}

//=======================================================================
// Motion Function
//=======================================================================
void myMotion(int x, int y)
{
	y = HEIGHT - y;

	if (cameraZoom - y > 0)
	{
		Eye.x += -0.1;
		Eye.z += -0.1;
	}
	else
	{
		Eye.x += 0.1;
		Eye.z += 0.1;
	}

	cameraZoom = y;

	glLoadIdentity();	//Clear Model_View Matrix

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters

	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glutPostRedisplay();	//Re-draw scene 
}

//=======================================================================
// Mouse Function
//=======================================================================
void myMouse(int button, int state, int x, int y)
{
	y = HEIGHT - y;

	if (state == GLUT_DOWN)
	{
		cameraZoom = y;
	}
}

//=======================================================================
// Reshape Function
//=======================================================================
void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}

//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	// Loading Model files
	model_couch.Load("Models/couch/couch.3ds");
	model_couch2.Load("Models/couch2/couch2.3ds");
	model_naruto.Load("Models/naruto/naruto.3ds");
	model_lamp.Load("Models/lamp/lamp.3ds");
	model_table.Load("Models/table/table.3ds");

	


	// Loading texture files
	tex_ground.Load("Textures/floor2.bmp");
	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);


	
}

//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(0, 0);

	glutCreateWindow(title);

	setupCamera();

	glutDisplayFunc(myDisplay);

	glutKeyboardFunc(myKeyboard);

	glutMotionFunc(myMotion);

	glutMouseFunc(myMouse);

	glutReshapeFunc(myReshape);

	myInit();

	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}