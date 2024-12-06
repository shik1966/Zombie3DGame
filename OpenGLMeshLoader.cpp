#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <math.h>
#include <corecrt_math_defines.h>
#include <ctime> // Include this for time 
#include <vector>
#include <iostream>

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
int currentView = 0;  
float playerX = 0.0f, playerY = 0.0f, playerZ = 0.0f;  
float weaponX = 0.0f, weaponY = 2.0f, weaponZ = 0.0f;  
bool firstPersonMode = false;  
double playerAngle = 0;  
double weaponAngle = 0;
float wallHeight = 40.0;  
float lastX = WIDTH / 2.0;
float lastY = HEIGHT / 2.0;
bool firstMouse = true;
float yaw = -90.0f;   
float pitch = 0.0f;
const float sensitivity = 0.1f;
float zombieX = -20.0f, zombieY = 0.0f, zombieZ = -20.0f; 
float playerHealth = 100.0f;
int gameTime = 80;  
bool gameActive = true;
bool doorIsOpen = false;
float doorAngle = 0.0; 
bool isRecoiling = false;
float recoilAmount = 5.0f;
float recoilSpeed = 0.5f; 
Vector doorPosition = Vector(-30.0, 0.0, -15.0); 
float doorWidth = 3.0f; 
float doorHeight = 5.0f; 
bool lampIsOn = false;
Vector lampPosition(-29.5, 1, 29.5);  
bool windowIsOpen = false;
Vector windowPosition(0.0, 10.0, 30.0);  // Initial window position
float windowTranslation = 0.0f;  // Amount to translate window
bool windowLightActive = false;  // Control activation of the window light
GLfloat windowLightPos[4] = { windowPosition.x, windowPosition.y + 2.0f, windowPosition.z, 1.0f };  // Window light position
float gravity = 0.25f;  // Gravity pulling the player down each frame
bool isJumping = false;  // State to check if the player is currently jumping
float jumpSpeed = 2.0f;  // Initial speed at which the player moves up
float playerVelocityY = 0.0f;  // Player's vertical velocity
float initialPlayerY = playerY;  // Set this at the start of the game or when the player is positioned

int countdownTime = 60;  // Start the timer from 60 seconds
bool timerActive = true;  // State to control timer activation
int playerScore = 0;
float thirdPersonDistance = 12.0f; // Distance behind the player
float thirdPersonHeight = 7.0f;

int playerMaxHealth = 100;  // Initial health cap
bool perkMachineActive = true;  // State of the perk machine
Vector perkMachinePosition = Vector(28.0, 2.5, -15.0);  // Position of the perk machine (adjust as needed)

Vector tablePosition(-26.0, 0.0, -28.0);  // Position of the table
bool tableInteracted = false;  // To ensure score is added only once
bool scene1 = true;//ne 1 is active by default
bool scene2 = true;//cene 2 is inactive by default


int currentAmmo = 30;  // Current ammunition in the weapon
int maxAmmo = 30;      // Maximum capacity of the weapon
bool isReloading = false;  // Flag to check if the weapon is currently reloading

bool isInvincible = false;
int invincibilityTimer = 0;

bool isDucking = false;  // State to check if the player is currently ducking
bool iskey= false;
bool isGun3 = false;
bool gameWin = false;
bool isTranslate= false;
float truckY = 0.0f;
bool isTranslate2 = false;
float doorY = 0.0f;


// Global definition
Vector perkMachine2Position = Vector(27.0, 4.0, -20.0);  // Adjust position as needed
bool perkMachine2Active = true;
int perkMachine2Cost = 20;  // Cost in points


// Model Variables
Model_3DS model_lamp;
Model_3DS model_couch;
Model_3DS model_couch2;
Model_3DS model_naruto;
Model_3DS model_door;
Model_3DS model_table;
Model_3DS model_cubes;
Model_3DS model_tv;
Model_3DS model_window;
Model_3DS model_gun;
Model_3DS model_perk;
Model_3DS model_zombie;
Model_3DS model_gasStation;
Model_3DS model_fence;
Model_3DS model_fuelPump;
Model_3DS model_car;
Model_3DS model_gun3;
Model_3DS model_truck;
Model_3DS model_truck2;
Model_3DS model_invincibility;
Model_3DS model_exit;
Model_3DS model_beast;
Model_3DS model_perk2;






// Textures
GLTexture tex_ground;
GLTexture tex_walls;

//=======================================================================
// Classes
//=======================================================================

class Invincibility {
public:
	Vector position;
	bool active;
	bool pickedUp;

	Invincibility() : position(0.0, 0.0, 0.0), active(false), pickedUp(false) {}

	void spawn(float x, float z) {
		position = Vector(x, 2.0, z);  // Spawns at y = 2.0 for visibility
		active = true;
		pickedUp = false;
	}

	void draw() {
		if (!active || pickedUp) return;
		glPushMatrix();
		glTranslatef(position.x, position.y, position.z);
		glScalef(0.02, 0.02, 0.02);
		glRotatef(0.0f, 1, 0, 0);
		model_invincibility.Draw();
		glPopMatrix();
	}

	bool checkCollision(float playerX, float playerZ) {
		float dx = playerX - position.x;
		float dz = playerZ - position.z;
		float distance = sqrt(dx * dx + dz * dz);
		return distance < 5.0; // Collision radius
	}
};

std::vector<Invincibility> invincibilityPowerUps;

class Zombie {
public:
	float x, y, z; // Position
	float health;
	bool active;
	time_t lastHitTime; // Time of the last hit

	Zombie(float posX, float posY, float posZ) : x(posX), y(posY), z(posZ), health(100.0f), active(true) {
		lastHitTime = 0; // Initialize last hit time
	}

	void draw() {
		if (!active) return;
		glPushMatrix();
		glTranslatef(x, y, z);
		glScalef(0.05, 0.05, 0.05);
		model_zombie.Draw();
		glPopMatrix();
	}

	void updatePosition(float playerX, float playerZ) {
		float speed = 0.05;
		Vector direction(playerX - x, 0, playerZ - z);
		float distance = sqrt(direction.x * direction.x + direction.z * direction.z);

		if (distance > 1.0) { // Avoid clumping exactly on the player
			direction.x /= distance;
			direction.z /= distance;

			x += direction.x * speed;
			z += direction.z * speed;
		}
	}

	bool canHit() {
		time_t currentTime = time(NULL);
		if (difftime(currentTime, lastHitTime) >= 3) { // Check if 3 seconds have passed
			lastHitTime = currentTime;
			return true;
		}
		return false;
	}

	void takeDamage(float damage) {
		health -= damage;
		if (health <= 0) {
			active = false;
			if (rand() % 2 == 0) {
				// 20% chance to spawn an invincibility power-up
				Invincibility newPowerUp;
				newPowerUp.spawn(x, z);
				invincibilityPowerUps.push_back(newPowerUp);
			}
		}
		else {
			staggerBack();
		}
	}

	// Method to make the zombie stagger back when hit
	void staggerBack() {
		float staggerDistance = 2.0; // Distance to stagger back
		x -= staggerDistance * cos(playerAngle * M_PI / 180.0); // Stagger back in the opposite direction of the player
		z -= staggerDistance * sin(playerAngle * M_PI / 180.0);
	}
};


std::vector<Zombie> zombies;

Vector calculateDirection(float yaw, float pitch) {
	float radYaw = yaw * M_PI / 180.0;
	float radPitch = pitch * M_PI / 180.0;
	return Vector(cos(radYaw) * cos(radPitch), sin(radPitch), sin(radYaw) * cos(radPitch));
}

class Bullet {
public:
	Vector position;
	Vector velocity;
	bool active;

	Bullet() : position(0.0, 0.0, 0.0), velocity(0.0, 0.0, 0.0), active(false) {}

	void fire(float startX, float startY, float startZ, float yaw, float pitch) {
		position = Vector(startX, startY, startZ);
		Vector direction = calculateDirection(yaw, pitch);
		velocity = Vector(direction.x * 20, direction.y * 20, direction.z * 20);  // Adjust speed as needed
		active = true;
		std::cout << "Bullet fired from (" << startX << ", " << startY << ", " << startZ << ") with velocity (" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")" << std::endl;
	}

	bool checkCollision(const Zombie& zombie) {
		float distance = sqrt(pow(position.x - zombie.x, 2) + pow(position.y - zombie.y, 2) + pow(position.z - zombie.z, 2));
		std::cout << "Checking collision: Bullet at (" << position.x << ", " << position.y << ", " << position.z << "), Zombie at (" << zombie.x << ", " << zombie.y << ", " << zombie.z << "), Distance: " << distance << std::endl;
		return distance < 8.0f; // Adjust collision radius as needed
	}

	void update() {
		if (!active) return;
		position.x += velocity.x * 0.1f; // Adjust speed as needed
		position.y += velocity.y * 0.1f; // Adjust speed as needed
		position.z += velocity.z * 0.1f; // Adjust speed as needed

		std::cout << "Bullet position updated to (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;

		// Check for collision with zombies
		for (Zombie& zombie : zombies) {
			if (zombie.active && checkCollision(zombie)) {
				zombie.takeDamage(50.0f); // Apply damage
				active = false; // Deactivate bullet
				playerScore += 10; // Increase score by 10
				std::cout << "Bullet hit zombie at (" << zombie.x << ", " << zombie.y << ", " << zombie.z << "). Score: " << playerScore << std::endl;
				break;
			}
		}

		// Deactivate bullet if it goes out of bounds (example condition)
		if (position.x > 100 || position.x < -100 || position.y > 100 || position.y < -100 || position.z > 100 || position.z < -100) {
			active = false;
			std::cout << "Bullet went out of bounds and is deactivated" << std::endl;
		}
	}

	void draw() {
		if (!active) return;
		glPushMatrix();
		glColor3f(1.0, 0.0, 0.0); // Red color for visibility
		glTranslatef(position.x, position.y, position.z);
		glutSolidSphere(0.3, 10, 10); // Small sphere for bullet
		glPopMatrix();
	}
};

class CubesModel {
public:
	Vector position;
	Vector rotation;
	bool active;
	bool hasCollided;  // Track if a collision has already occurred

	CubesModel(float x, float y, float z) : position(x, y, z), rotation(0.0, 0.0, 0.0), active(true), hasCollided(false) {}

	void draw() {
		if (!active) return;
		glPushMatrix();
		glTranslatef(position.x, position.y, position.z);
		glRotatef(rotation.x, 1.0, 0.0, 0.0);
		glRotatef(rotation.y, 0.0, 1.0, 0.0);
		glRotatef(rotation.z, 0.0, 0.0, 1.0);
		model_cubes.Draw();
		glPopMatrix();
	}

	void translateAndRotate() {
		if (!hasCollided) {
			position.x += 1.0;  // Translate by 1 unit on the X-axis
			position.z += 1.0;  // Translate by 1 unit on the Z-axis
			rotation.y += 45.0;  // Rotate by 45 degrees around the Y-axis
			hasCollided = true;  // Mark as collided
		}
	}
};




CubesModel cubes(-15.0, 0.0, -15.0); // Initialize cubes model at a specific position



std::vector<Bullet> bullets;



//=======================================================================
// Render text
//=======================================================================

void renderBitmapString(float x, float y, void* font, const char* string) {
	char* c;
	glRasterPos2f(x, y);
	for (c = (char*)string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}

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

	// Initialize window light
	glEnable(GL_LIGHT1);
	GLfloat ambientLightWindow[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat diffuseLightWindow[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat specularLightWindow[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLightWindow);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLightWindow);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specularLightWindow);
	glLightfv(GL_LIGHT1, GL_POSITION, windowLightPos);

	glDisable(GL_LIGHT1);  // Start with the window light turned off
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

	doorIsOpen = false; // Door starts closed
	doorPosition = Vector(-30.0, 0.0, -15.0); // Initial position of the door

	GLfloat light_position[] = { lampPosition.x, lampPosition.y + 5.0f, lampPosition.z, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);



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

	glBindTexture(GL_TEXTURE_2D, tex_walls.texture[0]);	// Bind the ground texture

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

void updateBullets() {
	for (auto& bullet : bullets) {
		if (bullet.active) {
			bullet.position.x += bullet.velocity.x * 0.2f; // Adjust speed as needed
			bullet.position.z += bullet.velocity.z * 0.2f; // Adjust speed as needed

			// Deactivate bullet if it goes out of bounds (example condition)
			if (bullet.position.x > 100 || bullet.position.x < -100 || bullet.position.z > 100 || bullet.position.z < -100) {
				bullet.active = false;
			}
		}
	}
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
	if (gameActive) {
		// Draw Ground
		RenderGround();
		// Draw naruto
		glPushMatrix();
		//handle jump case
		if (isJumping) {
			playerY += playerVelocityY;  // Move the player up or down
			playerVelocityY -= gravity;  // Apply gravity to the player
			if (playerY <= initialPlayerY) {  // Check if the player has landed
				playerY = initialPlayerY;  // Reset the player to the initial position
				playerVelocityY = 0.0f;  // Reset the player's velocity
				isJumping = false;  // Set jumping state to false
			}
		}
		glTranslatef(playerX, playerY, playerZ);
		glScalef(0.03, 0.03, 0.03);
		glRotatef(playerAngle, 0.0f, 1.0f, 0.0f);
		model_naruto.Draw();
		glPopMatrix();

		if (scene1) {
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

			if (lampIsOn) {
				lampPosition.x = playerX;  // Set lamp's X to player's X
				lampPosition.z = playerZ;  // Set lamp's Z to player's Z

				// Set light properties for moving lamp
				GLfloat light_position[] = { lampPosition.x, lampPosition.y + 5.0f, lampPosition.z, 1.0f };
				glLightfv(GL_LIGHT0, GL_POSITION, light_position);
			}

			glPushMatrix();
			glTranslatef(lampPosition.x, lampPosition.y, lampPosition.z);
			glScalef(0.005, 0.005, 0.005);
			model_lamp.Draw();
			glPopMatrix();


			glPushMatrix();
			glTranslatef(doorPosition.x, doorPosition.y, doorPosition.z);
			glScalef(0.1, 0.1, 0.1);
			if (doorIsOpen) {
				glRotatef(doorAngle, 0, 1, 0); // Rotate the door around the Y-axis
				glTranslatef(-1, 0, 0); // Translate to simulate door swinging open; adjust as needed
			}
			model_door.Draw();
			glPopMatrix();


			// Draw table
			if (!tableInteracted) {
				glPushMatrix();
				glTranslatef(tablePosition.x, 0.0, tablePosition.z);
				glScalef(0.05, 0.05, 0.05);
				glRotatef(0.0f, 1, 0, 0);
				model_table.Draw();
				glPopMatrix();
			}



			cubes.draw(); // Draw the cubes model

			glPushMatrix();
			glTranslatef(28.0, 0.0, 0.0);
			glScalef(0.005, 0.005, 0.005);
			glRotatef(0.0f, 1, 0, 0);
			model_tv.Draw();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(0.0, 5.0, 30.0);
			glScalef(0.005, 0.005, 0.005);
			glRotatef(0.0f, 1, 0, 0);
			model_window.Draw();
			glPopMatrix();

			// Setup light positions and intensity...
			if (windowLightActive) {
				glEnable(GL_LIGHT1);
				GLfloat light_position[] = { windowPosition.x, windowPosition.y, windowPosition.z, 1.0f };
				glLightfv(GL_LIGHT1, GL_POSITION, light_position);
			}
			else {
				glDisable(GL_LIGHT1);
			}

			// Draw the window with updated Y position
			glPushMatrix();
			glTranslatef(windowPosition.x, windowPosition.y, windowPosition.z);
			glScalef(0.005, 0.005, 0.005);
			model_window.Draw();
			glPopMatrix();

			glPushMatrix();
			//handle jump case weapon jumps with player
			if (isJumping) {
				weaponY += playerVelocityY;  // Move the weapon up or down
			}
			//reset to poistion before jumpiing after jumping
			else {
				if (isDucking)
				{
					weaponY = 1.0f;
				}
				else
				{
					weaponY = 2.0f;
				}
			}

			float currentWeaponY = weaponY;
			if (isRecoiling) {
				currentWeaponY += recoilAmount;  // Move the weapon up
				recoilAmount -= recoilSpeed;  // Reduce the recoil amount to come back to the original position
				if (recoilAmount <= 0) {
					isRecoiling = false;
					recoilAmount = 0;  // Ensure the recoil amount doesn't go negative
				}
			}
			glTranslatef(weaponX, currentWeaponY, weaponZ);  // Use the modified Y-coordinate
			glScalef(0.0035, 0.0035, 0.0035);  // Scaling to adjust the weapon size
			glRotatef(weaponAngle, 0, 1, 0);  // Rotate according to the current weapon angle
			model_gun.Draw();
			glPopMatrix();

			//glScalef(0.0035, 0.0035, 0.0035);
			//glRotatef(weaponAngle, 0, 1, 0);
			//model_gun.Draw();
			//glPopMatrix();

			
			for (auto& zombie : zombies) {
				zombie.draw();
			}
			glPushMatrix();
			glTranslatef(3.0, 0.0, 18.0);
			glScalef(0.05, 0.05, 0.05);
			glRotatef(0.0f, 1, 0, 0);
			model_truck.Draw();
			glPopMatrix();

		glPushMatrix();
		glTranslatef(0.0, 0.0, 0.0);
		glScalef(0.05, 0.05, 0.05);
		glRotatef(0.0f, 1, 0, 0);
		//model_exit.Draw();
		glPopMatrix();
		

		if (perkMachine2Active) {
			glPushMatrix();
			glTranslatef(perkMachine2Position.x, perkMachine2Position.y, perkMachine2Position.z);
			glScalef(0.15, 0.15, 0.15);  // Scale as needed
			glRotatef(-90, 0, 1, 0);  // Adjust orientation as needed
			model_perk2.Draw();  // Assuming you have a model for this machine
			glPopMatrix();
		}

		}

		else if (scene2) {
			// Draw fence
			for (int i = 0; i < 9; ++i) {
				glPushMatrix();
				glTranslatef(-30.0, 3.0, -20.0 + i * 5.0); // Adjust the translation for each fence
				glScalef(0.001, 0.01, 0.001);
				model_fence.Draw();
				glPopMatrix();
			}
			// Loop to draw multiple fences on the second side
			for (int i = 0; i < 9; ++i) {
				glPushMatrix();
				glTranslatef(-20.0 + i * 5.0, 3.0, -30.0); // Adjust the translation for each fence
				glScalef(0.001, 0.01, 0.001);
				glRotatef(90.0f, 0, 1, 0);
				model_fence.Draw();
				glPopMatrix();
			}

			// Loop to draw multiple fences on the third side
			for (int i = 0; i < 9; ++i) {
				glPushMatrix();
				glTranslatef(20.0 - i * 5.0, 3.0, 30.0); // Adjust the translation for each fence
				glScalef(0.001, 0.01, 0.001);
				glRotatef(90.0f, 0, 1, 0);
				model_fence.Draw();
				glPopMatrix();
			}
			for (int i = 0; i < 9; ++i) {
				glPushMatrix();
				glTranslatef(30.0 - i * 5.0, 3.0, -30.0); // Adjust the translation for each fence
				glScalef(0.001, 0.01, 0.001);
				glRotatef(90.0f, 0, 1, 0);
				model_fence.Draw();
				glPopMatrix();
			}


			//glPushMatrix();
			//glTranslatef(-15.0, 0.0, -15.0);
			//glScalef(0.005, 0.005, 0.005);
			//glRotatef(0.0f, 1, 0, 0);
			//model_truck2.Draw();
			//glPopMatrix();



			glPushMatrix();
			glTranslatef(3.0, 0.0, 3.0);
			glScalef(0.02, 0.02, 0.02);
			glRotatef(0.0f, 1, 0, 0);
			model_fuelPump.Draw();
			glPopMatrix();


			glPushMatrix();
			glTranslatef(3.0, 0.0, 18.0);
			glScalef(0.04, 0.04, 0.04);
			glRotatef(0.0f, 1, 0, 0);
			model_gasStation.Draw();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(-12.0, 0.0, 5.0);
			glScalef(2, 2, 2);
			glRotatef(0.0f, 1, 0, 0);
			model_car.Draw();
			glPopMatrix();

			glPushMatrix();
			if (!isGun3) {
				//put gun3 on ground
				glTranslatef(0.0, 0.0, -10.0);
				glScalef(0.3, 0.3, 0.3);
				glRotatef(0.0f, 1, 0, 0);
				model_gun3.Draw();
				glPopMatrix();


			}
			else
			{

				if (isJumping) {
					weaponY += playerVelocityY;  // Move the weapon up or down
				}
				//reset to poistion before jumpiing after jumping
				else {
					if (isDucking)
					{
						weaponY = 1.0f;
					}
					else
					{
						weaponY = 2.0f;
					}
				}

				float currentWeaponY = weaponY;
				if (isRecoiling) {
					currentWeaponY += recoilAmount;  // Move the weapon up
					recoilAmount -= recoilSpeed;  // Reduce the recoil amount to come back to the original position
					if (recoilAmount <= 0) {
						isRecoiling = false;
						recoilAmount = 0;  // Ensure the recoil amount doesn't go negative
					}
				}
				glTranslatef(weaponX, currentWeaponY, weaponZ);  // Use the modified Y-coordinate
				glScalef(0.3, 0.3, 0.3);  // Scaling to adjust the weapon size
				glRotatef(weaponAngle, 0, 1, 0);  // Rotate according to the current weapon angle
				model_gun3.Draw();
				glPopMatrix();
			}

			if (isTranslate) {
				//move truck2 upwards and keep moving till it disappears
				glPushMatrix();
				glTranslatef(-15.0, truckY, -15.0);
				glScalef(0.05, 0.05, 0.05);
				glRotatef(0.0f, 1, 0, 0);
				model_truck2.Draw();
				glPopMatrix();
				truckY += 0.1;
				if (truckY > 10.0) {
					isTranslate = false;
				}
				}
			if (!iskey) {
				glPushMatrix();
				glTranslatef(13.0, 0.0, -5.0);
				glScalef(0.05, 0.05, 0.05);
				glRotatef(0.0f, 1, 0, 0);
				model_truck2.Draw();
				glPopMatrix();
			}
			if (isTranslate2) {
				//move truck2 upwards and keep moving till it disappears
				glPushMatrix();
				glTranslatef(doorPosition.x, doorPosition.y, doorPosition.z);
				glScalef(0.005, 0.005, 0.005);
				glRotatef(90.0f, 0, 1, 0);
				model_exit.Draw();
				glPopMatrix();
				doorPosition.y += 0.1;
				if (doorPosition.y > 10.0) {
					isTranslate2 = false;
					gameActive = false;
					gameWin = true;
				}
			}
			else {
				glPushMatrix();
				glTranslatef(doorPosition.x, doorPosition.y, doorPosition.z);
				glScalef(0.005, 0.005, 0.005);
				glRotatef(90.0f, 0, 1, 0);
				model_exit.Draw();
				glPopMatrix();
			}
			if (perkMachineActive) {
				glPushMatrix();
				glTranslatef(perkMachinePosition.x, perkMachinePosition.y, perkMachinePosition.z);
				glScalef(0.5, 0.5, 0.5);  // Scale as needed
				glRotatef(180, 0, 1, 0);  // Adjust orientation as needed
				model_perk.Draw();
				glPopMatrix();
			}

		}
	}
	else {
		if (gameWin) {
						// Display game win text
			char text[50];
			sprintf(text, "Congratulations! You Win! Final Score: %d", playerScore);
			renderBitmapString(10, 10, GLUT_BITMAP_HELVETICA_18, text);
		}
		else {
			// Display game over text
			char text[50];
			sprintf(text, "Game Over! Final Score: %d", playerScore);
			renderBitmapString(10, 10, GLUT_BITMAP_HELVETICA_18, text);
		}
	}

	



	// Set up 2D orthographic projection to draw text
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, WIDTH, 0, HEIGHT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// Disable lighting and depth test for 2D rendering
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	// Set text color
	glColor3f(1.0, 1.0, 1.0); // White color

	char healthText[50];
	sprintf(healthText, "Health: %.2f", playerHealth);
	renderBitmapString(10, HEIGHT - 20, GLUT_BITMAP_HELVETICA_18, healthText);

	char timerText[50];
	sprintf(timerText, "Timer: %d", countdownTime);
	renderBitmapString(WIDTH - 120, HEIGHT - 20, GLUT_BITMAP_HELVETICA_18, timerText);

	char scoreText[50];
	sprintf(scoreText, "Score: %d", playerScore);
	renderBitmapString(10, 40, GLUT_BITMAP_HELVETICA_18, scoreText); // Adjust position as needed

	// Render bullet count
	char ammoText[50];
	sprintf(ammoText, "Ammo: %d/%d", currentAmmo, maxAmmo);
	renderBitmapString(WIDTH - 150, 20, GLUT_BITMAP_HELVETICA_18, ammoText);

	// Restore matrices, lighting, and depth test
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	

	updateBullets();

	// Draw bullets
	for (auto& bullet : bullets) {
		bullet.draw();
	}

	for (auto& powerUp : invincibilityPowerUps) {
		powerUp.draw();
	}


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
		gluLookAt(playerX, playerY + 5, playerZ,  // Camera position at player's eye level
			playerX + sin(playerAngle * M_PI / 180.0), playerY + 5, playerZ + cos(playerAngle * M_PI / 180.0),  // Look at point
			0.0, 1.0, 0.0);  // Up vector is always straight up

		// Draw weapon in first person view
		glPushMatrix();
		// Move to the player's position
		glTranslatef(playerX, playerY + 5, playerZ);
		// Rotate the weapon around the player's position
		glRotatef(playerAngle, 0.0f, 1.0f, 0.0f);
		// Move the weapon out in front of the player
		glTranslatef(0.5, -0.2, 0.2); // These values control the position relative to the player
		glScalef(0.05, 0.05, 0.05);  // Scale the weapon
		model_gun.Draw();
		glPopMatrix();

	}
	else if (currentView == 4) { // Third-person view
		// Calculate the camera position based on the player's position and angle
		float camX = playerX - thirdPersonDistance * sin(playerAngle * M_PI / 180.0);
		float camY = playerY + thirdPersonHeight;
		float camZ = playerZ - thirdPersonDistance * cos(playerAngle * M_PI / 180.0);

		// Set the camera to look at the player
		gluLookAt(camX, camY, camZ, playerX, playerY + 2, playerZ, 0.0, 1.0, 0.0);
	}

	else {
		// Other camera views
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

	// Draw Naruto (if visible in first-person mode)
	if (!firstPersonMode) {
		glPushMatrix();
		glTranslatef(playerX, playerY, playerZ);
		glScalef(0.025, 0.025, 0.025);
		glRotatef(playerAngle, 0.0f, 1.0f, 0.0f);
		model_naruto.Draw();
		glPopMatrix();
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
	case 't':
		currentView = 4; // Third-Person View
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
	model_door.Load("Models/door/door.3ds");
	model_table.Load("Models/table/table.3ds");
	model_cubes.Load("Models/childCubes/cubes.3ds");
	model_tv.Load("Models/tv/tv.3ds");
	model_window.Load("Models/window/window.3ds");
	model_gun.Load("Models/gun2/gun2.3ds");
	model_zombie.Load("Models/zombie/zombie.3ds");
	model_perk.Load("Models/perkMachine/perkmachine2.3ds");
	model_fuelPump.Load("Models/fuelPump/fuelPump.3ds");
	model_fence.Load("Models/fence/fence.3ds");
	model_gasStation.Load("Models/gasStation/gasStation.3ds");
	model_car.Load("Models/car/car.3ds");
	model_gun3.Load("Models/gun3/gun3.3ds");
	model_truck.Load("Models/truck/JeepRenegade.3ds");
	model_truck2.Load("Models/truck2/truck2.3DS");
	model_invincibility.Load("Models/invincibility/invincible.3ds");
	model_exit.Load("Models/exit/exit.3ds");
	//model_beast.Load("Models/beast/beast.3ds");
	model_perk2.Load("Models/perkMachine2/untitled.3ds");



	// Loading texture files
	tex_ground.Load("Textures/floor4.bmp");
	tex_walls.Load("Textures/floor2.bmp");
	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);


}

//=======================================================================
// Misc
//=======================================================================

void spawnZombie(int value) {
	// Spawn a new zombie at a random location around the edges of the playable area
	float spawnX = (rand() % 60) - 30;
	float spawnZ = (rand() % 60) - 30;
	zombies.push_back(Zombie(spawnX, 0.0f, spawnZ));

	if (gameActive) {
		glutTimerFunc(10000, spawnZombie, 0); // Spawn another zombie in 10 seconds
	}
}

void updateZombiePosition(int value) {
	if (!gameActive) return;

	const float collisionDistance = 2.0; // Define how close zombies must be to harm the player
	const float damage = 25.0; // Amount of health reduced on collision

	for (auto& zombie : zombies) {
		zombie.updatePosition(playerX, playerZ);

		// Calculate distance from the zombie to the player
		float dx = playerX - zombie.x;
		float dz = playerZ - zombie.z;
		float distance = sqrt(dx * dx + dz * dz);

		// Check for collision and whether the zombie can hit again
		if (distance < collisionDistance && zombie.canHit() && !isInvincible) {
			playerHealth -= damage;
			if (playerHealth <= 0) {
				gameActive = false; // End the game if the player's health is depleted
			}
			printf("Player hit! Health: %f\n", playerHealth); // Output the health to console (or handle as needed)
		}
	}

	glutPostRedisplay();
	glutTimerFunc(100, updateZombiePosition, 0);
}

void updateScore() {
	playerScore += 50;  // Increase score by 50
}

bool checkBulletZombieCollision() {
	for (auto& bullet : bullets) {
		if (!bullet.active) continue;
		for (auto& zombie : zombies) {
			if (!zombie.active) continue;
			float dx = bullet.position.x - zombie.x;
			float dy = bullet.position.y - (zombie.y + 3); // Adjust y for zombie height
			float dz = bullet.position.z - zombie.z;
			float distance = sqrt(dx * dx + dy * dy + dz * dz);
			if (distance < 1.0f) { // Collision threshold
				bullet.active = false; // Deactivate bullet
				zombie.health -= 50; // Damage the zombie
				if (zombie.health <= 0) {
					zombie.active = false; // Deactivate zombie if health is depleted
					updateScore();  // Update score when a zombie is hit
				}
				return true; // Collision occurred
			}
		}
	}
	return false; // No collision occurred
}

//void fireBullet(float angle) {
//	Bullet newBullet;
//	newBullet.fire(playerX, playerY , playerZ, angle); // Fire from player position
//	bullets.push_back(newBullet);
//}

void drawBullets() {
	for (auto& bullet : bullets) {
		bullet.draw();
	}
}

void updatePlayerDirection(float angle) {
	playerAngle = angle;  // Set player angle directly
	if (playerAngle >= 360.0) playerAngle -= 360.0;  // Ensure the angle remains within 0-360 degrees
	if (playerAngle < 0) playerAngle += 360.0;
}

//void Bullet::update() {
//	if (!active) return;
//	position.x += velocity.x * 0.1f; // Adjust speed as needed
//	position.y += velocity.y * 0.1f; // Adjust speed as needed
//	position.z += velocity.z * 0.1f; // Adjust speed as needed
//
//	// Check for collision with zombies
//	for (Zombie& zombie : zombies) {
//		if (zombie.active && checkCollision(zombie)) {
//			zombie.takeDamage(20.0f); // Apply damage
//			active = false; // Deactivate bullet
//			std::cout << "Bullet hit zombie at (" << zombie.x << ", " << zombie.y << ", " << zombie.z << ")" << std::endl;
//			break;
//		}
//	}
//
//	// Deactivate bullet if it goes out of bounds (example condition)
//	if (position.x > 100 || position.x < -100 || position.y > 100 || position.y < -100 || position.z > 100 || position.z < -100) {
//		active = false;
//	}
//}


// In your game loop or relevant function
void updatePlayer() {
	if (isJumping) {
		playerVelocityY -= gravity; // Apply gravity
		playerY += playerVelocityY; // Update player Y position based on current velocity

		// Check if the player has returned to the ground
		if (playerY <= initialPlayerY) {
			playerY = initialPlayerY; // Reset to initial Y position
			isJumping = false; // Stop jumping
			playerVelocityY = 0.0f; // Reset velocity
		}
	}
}

void updateCountdown(int value) {
	if (timerActive && countdownTime > 0) {
		countdownTime--;
		glutTimerFunc(1000, updateCountdown, 0); // Call this function again after 1000 milliseconds
	}
	glutPostRedisplay(); // Make sure the display is updated to reflect the countdown
}


void checkCubesCollision() {
	float collisionThreshold = 2.0f; // Distance within which a collision is considered

	float dx = playerX - cubes.position.x;
	float dz = playerZ - cubes.position.z;
	float distance = sqrt(dx * dx + dz * dz);

	if (distance < collisionThreshold && !cubes.hasCollided) {
		cubes.translateAndRotate();  // Perform the transformation
	}
}

void regenerateHealth(int value) {
	if (playerHealth < playerMaxHealth) {
		playerHealth += 25;
		if (playerHealth > playerMaxHealth) {
			playerHealth = playerMaxHealth;  // Cap health at the maximum
		}
	}
	glutTimerFunc(5000, regenerateHealth, 0);  // Re-register timer every 5 seconds
}


void finishReload(int value) {
	currentAmmo = maxAmmo;
	isReloading = false;
	std::cout << "Reload complete. Ammo restored to " << maxAmmo << std::endl;
	glutPostRedisplay();
}

void reloadWeapon() {
	if (currentAmmo < maxAmmo) {
		std::cout << "Reloading weapon..." << std::endl;
		isReloading = true;
		// Simulate a reload delay
		glutTimerFunc(2000, finishReload, 0);  // 2 seconds reload time
	}
}

void fireBullet() {
	if (currentAmmo > 0 && !isReloading) {
		Bullet newBullet;
		newBullet.fire(playerX, playerY + 5, playerZ, yaw, pitch);
		bullets.push_back(newBullet);
		currentAmmo--;
		std::cout << "Fired bullet. Remaining ammo: " << currentAmmo << std::endl;
	}
	else {
		std::cout << "Cannot fire: Out of ammo or reloading!" << std::endl;
	}
}


void checkInvincibilityCollisions() {
	for (auto& powerUp : invincibilityPowerUps) {
		if (powerUp.active && !powerUp.pickedUp && powerUp.checkCollision(playerX, playerZ)) {
			powerUp.pickedUp = true;
			isInvincible = true;
			invincibilityTimer = 10; // 10 seconds of invincibility
			std::cout << "Invincibility activated!" << std::endl;
		}
	}
}

void updateGame(int value) {
	if (!gameActive) return;
	// Update each bullet
	for (auto& bullet : bullets) {
		bullet.update();
	}

	// Handle other game updates such as timer or game state
	if (gameTime > 0) {
		gameTime--;
		printf("Time left: %d seconds\n", gameTime);
	}
	else {
		gameActive = false;
		printf("Game Over!\n");
	}
	checkCubesCollision(); // Check for collisions between the player and cubes model
	// Re-register the update function
	checkInvincibilityCollisions();  // Call this function to check for collisions
	
	if (isInvincible) {
		invincibilityTimer--;
		if (invincibilityTimer <= 0) {
			isInvincible = false;
			std::cout << "Invincibility ended." << std::endl;
		}
	}
	glutTimerFunc(1000, updateGame, 0);
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

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// Shoot bullet when left mouse button is pressed
		fireBullet();  // Make sure this function correctly handles shooting logic
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		// Move player forward when right mouse button is pressed
		float stepSize = 0.5;  // Adjust step size to control speed
		playerX += stepSize * sin(playerAngle * M_PI / 180.0);
		playerZ += stepSize * cos(playerAngle * M_PI / 180.0);
		weaponX += stepSize * sin(playerAngle * M_PI / 180.0);
		weaponZ += stepSize * cos(playerAngle * M_PI / 180.0);
		glutPostRedisplay();  // Redraw the scene with updated player position
	}
}



//=======================================================================
// Keyboard Function
//==========================================================

void myKeyboard(unsigned char key, int x, int y) {
	float stepSize = 0.3;  // Movement step size
	float newX, newZ;  // Variables to hold potential new positions
	float newWeaponX, newWeaponZ;

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
	case ' ': // Spacebar for jump
		if (!isJumping) { // Only jump if not already jumping
			isJumping = true;
			playerVelocityY = jumpSpeed; // Set initial jump velocity
		}
		break;
	case 'w':
		playerAngle = 270;  // North
		weaponAngle = 270;
		yaw = 180;
		break;
	case 's':
		playerAngle = 90;  // South
		weaponAngle = 90;
		yaw = 0;
		break;
	case 'a':
		playerAngle = 0;  // West
		weaponAngle = 0;
		yaw = 90;
		break;
	case 'd':
		playerAngle = 180;  // East
		weaponAngle = 180;
		yaw = -90;
		break;
	case 'c':
		isDucking = !isDucking;  // Toggle ducking state
		if (isDucking) {
			playerY -= 2.0f;  // Lower the player's position
		}
		else {
			playerY += 2.0f;  // Raise the player's position
		}
		break;
	}
	

	// Compute new positions based on angle
	if (key == 'w' || key == 's' || key == 'a' || key == 'd') {
		newX = playerX + stepSize * sin(playerAngle * M_PI / 180.0);
		newZ = playerZ + stepSize * cos(playerAngle * M_PI / 180.0);
		newWeaponX = weaponX + stepSize * sin(weaponAngle * M_PI / 180.0);
		newWeaponZ = weaponZ + stepSize * cos(weaponAngle * M_PI / 180.0);

		// Check for collision with the boundary walls (-30 to +30)
		if (newX >= -30.0f && newX <= 30.0f && newZ >= -30.0f && newZ <= 30.0f) {
			playerX = newX;
			playerZ = newZ;
			weaponX = newWeaponX;
			weaponZ = newWeaponZ;
		}
	}

	if (key == 'e' || key == 'E' ) {
		float distance = sqrt(pow(playerX - doorPosition.x, 2) + pow(playerZ - doorPosition.z, 2));
		if (scene1)
		{
			if (distance < 5.0 && countdownTime <= 0) { // Check if within interaction distance, adjust as necessary

				doorIsOpen = true;
				doorAngle = 90; // Rotate the door by 90 degrees
				scene1 = false;
				scene2 = true;
			}
		}
			else if (scene2)
			{
				if (iskey)
				{
					isTranslate2 = true;
					

			    }
			}
		float distanceToLamp = sqrt(pow(playerX - lampPosition.x, 2) + pow(playerZ - lampPosition.z, 2));
		if (distanceToLamp < 5.0) { // Interaction distance check
			lampIsOn = !lampIsOn;  // Toggle lamp state
		}
		float distanceToWindow = sqrt(pow(playerX - windowPosition.x, 2) + pow(playerZ - windowPosition.z, 2));
		if (distanceToWindow < 5.0) {  // Check proximity to window
			windowIsOpen = !windowIsOpen;  // Toggle window state
			if (windowIsOpen) {
				windowPosition.y += 5.0f;  // Move window upwards
				windowLightActive = true;  // Turn on the light
			}
			else {
				windowPosition.y -= 5.0f;  // Return window to original position
				windowLightActive = false;  // Turn off the light
			}
		}

		float distanceToPerkMachine = sqrt(pow(playerX - perkMachinePosition.x, 2) + pow(playerZ - perkMachinePosition.z, 2));
		if (distanceToPerkMachine < 5.0 && perkMachineActive && playerScore >= 50) {
			playerMaxHealth = 150;  // Increase health cap
			perkMachineActive = false;  // Disable further interaction
			playerScore = playerScore - 50;
		}

		float distanceToTable = sqrt(pow(playerX - tablePosition.x, 2) + pow(playerZ - tablePosition.z, 2));
		if (key == 'e' && distanceToTable < 5.0 && !tableInteracted) {
			playerScore += 500;  // Increase score by 500
			tableInteracted = true;  // Prevent further interaction
			printf("Score: %d\n", playerScore);  // Optional: Output score to console
		}
		//check for collision with the car
		// Truck's position
		float truckX = 13.0f;
		float truckZ = -5.0f;
		float distanceToCar = sqrt(pow(playerX - truckX, 2) + pow(playerZ - truckZ, 2));

		// Check if the player is within a certain distance to the truck (e.g., 5.0 units)
		if (distanceToCar < 5.0) {
			iskey = true;  // Player can interact with the truck
			isTranslate = true;
		}
		//check for collision with the Gun 3
		float distanceToGun3 = sqrt(pow(playerX, 2) + pow(playerZ + 10.0, 2)); // Gun3 assumed to be at (0, 0, -10)

		// Interaction with Gun3
		if (distanceToGun3 < 2.0 && playerScore>10) {  // Replace 2.0 with the pickup radius
			isGun3 = true;  // Equip the gun
			playerScore -= 10;  // Increase score
		}
		float distanceToPerkMachine2 = sqrt(pow(playerX - perkMachine2Position.x, 2) + pow(playerZ - perkMachine2Position.z, 2));
		if (distanceToPerkMachine2 < 5.0 && perkMachine2Active && playerScore >= perkMachine2Cost) {
			playerScore -= perkMachine2Cost;  // Deduct points
			maxAmmo += 30;  // Increase ammo count, adjust amount as needed
			std::cout << "Ammo increased. Current Ammo: " << currentAmmo << std::endl;
			std::cout << "Remaining Score: " << playerScore << std::endl;
			perkMachine2Active = false;  // Optional: deactivate after use
		}


		//if (distanceToWindow < 5.0) { // Check proximity to window
		//	windowLightActive = true; // Activate window light
		//	windowTranslation = 5.0f; // Translate window when activated
		//}
	}

	switch (key) {
	case 'f':
		if (currentAmmo > 0) {  // Only allow shooting if there is ammo
			fireBullet();
			isRecoiling = true;  // Start the recoil effect
			recoilAmount = 0.5f;  // Set the initial recoil amount
			currentAmmo--;  // Decrement the bullet count
			printf("Shot fired. Bullets remaining: %d\n", currentAmmo);
		}
		else {
			printf("Out of ammo!\n");
		}
		break;
	case 'p':
		// Toggle first-person mode
		firstPersonMode = !firstPersonMode;
		break;
	case 'r':
		reloadWeapon();
		break;
	case '1':
	case '2':
	case '3':
	case 't':
	case '0':
		if (!firstPersonMode) {  // Only switch views if not in first-person mode
			switchCameraView(key, x, y);
		}
		break;

	case 27:  // ESC key to quit
		exit(0);
		break;
	}

	//if (key == 'f') {  // Assuming 'f' key is for firing
	//	fireBullet();
	//	recoilAmount = 2.0f;  // Set recoil amount
	//	printf("Recoil started\n");  // Debug output

	//}

	setupCamera();  // Update the camera setup
	glutPostRedisplay();  // Redraw the scene with the new settings
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

	glutTimerFunc(10000, regenerateHealth, 0);  // Start the health regeneration timer

	glutTimerFunc(1000, updateCountdown, 0); // Start the countdown timer
	glutTimerFunc(100, updateZombiePosition, 0);
	glutTimerFunc(10000, updateGame, 0);

	glutTimerFunc(10000, spawnZombie, 0); // Start spawning zombies
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