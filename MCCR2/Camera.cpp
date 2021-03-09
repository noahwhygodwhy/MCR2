#pragma once

#include <iostream>
#include "glad.h"
#include "DirectionEnum.h"
#include "Camera.hpp"
#include <glm/glm.hpp>


using namespace std;
using namespace glm;

vec3 Camera::getPos()
{
	return this->position;
}


void Camera::setPos(vec3 pos)
{
	this->position = pos;
	updateVectors();
}

void Camera::keyboardInput(Direction dir, float time)
{
	float vel = time * movementSpeed;
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
void Camera::updateVectors()
{
	vec3 f;
	f.x = cos(radians(yaw)) * cos(radians(pitch));
	f.y = sin(radians(pitch));
	f.z = sin(radians(yaw)) * cos(radians(pitch));
	front = normalize(f);
	right = normalize(cross(front, worldUp));
	up = normalize(cross(right, front));
}