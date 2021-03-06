#pragma once

#include <iostream>
#include "glad.h"
#include "DirectionEnum.h"
#include "Camera.h"
#include <glm/glm.hpp>


using namespace std;
using namespace glm;

vec3 Camera::getPos()
{
	return this->position;
}


void Camera::keyboardInput(Direction dir, float time)
{
	float vel = time * movementSpeed;
	//printf("vel: %f\n", vel);
	//printf("dir: %i\n", dir);
	//printf("id: %f\n", id);
	//printf("front: ");
	//printVecThree(front);
	//printf("position: ");
	//printVecThree(position);
	switch (dir)
	{
	case Direction::FORWARD:
		position += front * vel;
		break;
	case Direction::BACKWARD:
		position += -front * vel;
		break;
	case Direction::RIGHT:
		position += right * vel;
		break;
	case Direction::LEFT:
		position += -right * vel;
		break;
	case Direction::UP:
		position += up * vel;
		break;
	case Direction::DOWN:
		position += -up * vel;
		break;
	case Direction::YAW_LEFT:
		yaw += -1;
		break;
	case Direction::YAW_RIGHT:
		yaw += 1;
		break;
	default:
		//this block intentionally left blank
		break;
	}
	updateVectors();
	////printf("new position: ");
	//printVecThree(position);	
}
void Camera::mouseInput(float x, float y, GLboolean cp = GL_TRUE)
{
	yaw += x * mouseSensitivity;
	pitch += y * mouseSensitivity;

	if (cp)
	{
		if (pitch > 89.0f)
		{
			pitch = 89.0f;
		}
		if (pitch < -89.0f)
		{
			pitch = -89.0f;
		}
	}
	updateVectors();
}
mat4 Camera::getView()
{
	return lookAt(position, position + front, up);
}
//void Camera::moveLeft()
//{
//	position = position + (right*0.1f);
//}
void Camera::updateVectors()
{
	vec3 f;
	f.x = cos(radians(yaw)) * cos(radians(pitch));
	f.y = sin(radians(pitch));
	f.z = sin(radians(yaw)) * cos(radians(pitch));
	front = normalize(f);
	right = normalize(cross(front, worldUp));
	up = normalize(cross(right, front));
	////printf("front:\n");
	//printVecThree(front);
	////printf("right:\n");
	//printVecThree(right);
	//printf("up:\n");
	//printVecThree(up);
}
float Camera::getId()
{
	return id;
}