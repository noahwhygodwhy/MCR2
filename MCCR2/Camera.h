#pragma once

#include "glad.h"
#include "Asset.hpp"
#include "glad.h"
#include <GLFW/glfw3.h>
#include "DirectionEnum.h"

using namespace std;
using namespace glm;


class Camera
{
	vec3 position;
	vec3 front;
	vec3 right;
	vec3 up;
	vec3 worldUp;

	// Euler Angles
	float yaw;
	float pitch;
	// Camera options
	float movementSpeed;
	float mouseSensitivity;
	float zoom;
	float id;
public:
	Camera(vec3 initPos, vec3 initUp, float y, float p, float mvspd, float mouseSens, float zooooooom)
	{
		position = initPos;
		worldUp = initUp;
		yaw = y;
		pitch = p;
		movementSpeed = mvspd;
		mouseSensitivity = mouseSens;
		zoom = zooooooom;
		id = glfwGetTime();
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
	void keyboardInput(Direction dir, float time);
	void mouseInput(float x, float y, GLboolean cp);
	void updateVectors();
	mat4 getView();
	float getId();
	vec3 getPos();
	void setPos(vec3 pos);

};