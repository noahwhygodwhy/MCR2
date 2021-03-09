/*! \file Camera.h
* Contains the Camera class.
*/

#ifndef CAMERA_H
#define CAMERA_H

#include "glad.h"
#include "Asset.hpp"
#include "glad.h"
#include <GLFW/glfw3.h>
#include "DirectionEnum.h"

using namespace std;
using namespace glm;


enum class Direction {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	YAW_LEFT,
	YAW_RIGHT
};

/*! \class Camera
* A class to represent a "Camera" the best way you can in opengl.
* It's basically a movable point with an forward and up vector that can provide
* a view matrix on demand.
*/
class Camera
{
	vec3 position; ///<The position of the camera
	vec3 front; ///<The front vector of the camera
	vec3 right; ///<The right vector of the camera
	vec3 up; ///<The up vector of the camera
	vec3 worldUp; ///<the worldUp given at creation. Typically {0,1,0}

	// Euler Angles
	float yaw; ///<The yaw angle from 0 in degrees 
	float pitch;///<The pitch angle from 0 in degrees.
	// Camera options
	float movementSpeed; ///<How fast the camera moves
	float mouseSensitivity; ///<How fast the camera rotates by the mouse (not yet implemented)
	float zoom; ///<How zoomed in the camera is

public:
	/*! \fn Camera
	* \param initPos The initial position
	* \param initUp The initial up vector of the camera + the worldUp.
	* \param y The initial yaw value.
	* \param p The initial pitch value.
	* \param mvspd How fast the camera moves around the world.
	* \param zooooooom The initial zoom value.
	*/
	Camera(vec3 initPos, vec3 initUp, float y, float p, float mvspd, float mouseSens, float zooooooom)
	{
		position = initPos;
		worldUp = initUp;
		yaw = y;
		pitch = p;
		movementSpeed = mvspd;
		mouseSensitivity = mouseSens;
		zoom = zooooooom;
		updateVectors();
	}
	Camera()
	{
		position = vec3(0, 0, 0);
		worldUp = vec3(0, 1, 0);
		yaw = 0;
		pitch = 0;
		movementSpeed = 1;
		mouseSensitivity = 1;
		zoom = 1;
		updateVectors();
	}
	/*! \fn keyboardInput
	* Moves the camera based on keyboard input. WASD and space/shift for up/down. Q/E for yaw.
	* \param dir The direction of the movement (one of the 6 axis in 3D)
	* \param time The amount of time that this frame took, coresponding to how much movement should happen.
	*/
	void keyboardInput(Direction dir, float time);
	/*! \fn mouseInput
	* Not yet implemented
	*/
	void mouseInput(float x, float y, GLboolean cp);
	/*! \fn updateVectors
	* Using the givens, yaw, pitch, and world up, generates an updated front, right and up vector.
	*/
	void updateVectors();
	/*! \fn getView
	* \returns A mat4 view vector based on the camera's position and forward/up vectors.
	*/
	mat4 getView();
	/*! \fn getPos
	* \returns The 3D position of the camera.
	*/
	vec3 getPos();

	/*! \fn setPos
	* Sets the new position of the camera.
	* \param pos The new 3D position of the camera.
	*/
	void setPos(vec3 pos);

};
#endif