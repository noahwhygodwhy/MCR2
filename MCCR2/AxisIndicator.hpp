/*! \file AxisIndicator.hpp
* \brief To draw an axis indicator. Only used for development.
*/

#ifndef AXIS_INIDCATOR_H
#define AXIS_INIDCATOR_H

#include "Renderer.hpp"
#include "Shader.hpp"
#include <string>
#include "Camera.hpp"
#include <vector>
#include "Config.h"
#include "Asset.hpp"

using namespace std;




/*! \class AxisIndicator.hpp
* \brief To draw an axis indicator. Only used for development.
*/
class AxisIndicator
{
public:
	AxisIndicator(Camera* c);
	~AxisIndicator();
	void draw(int screenX, int screenY);
private:
	unsigned int VAO = 0, VBO = 0;
	Shader axisShader;
	Camera* cam;
	unsigned int fontTexture;
	
};





struct CV //color vert
{
	vec3 coord;
	vec3 color;
	CV(vec3 a, vec3 b)
	{
		this->coord = a;
		this->color = b;
	}
};


vec3 red = vec3(255.0f, 0.0f, 0.0f);
vec3 green = vec3(0.0f, 255.0f, 0.0f);
vec3 blue = vec3(0.0f, 0.0f, 255.0f);



vector<CV> axisVerts = {
	//x line
	CV(vec3(20.0f, 0.0f, 0.0f), red),
	CV(vec3(-20.0f, 0.0f, 0.0f), red),
	//x arrow head
	CV(vec3(20.0f, 0.0f, 0.0f), red),
	CV(vec3(17.0f, 2.0f, 0.0f), red),
	CV(vec3(20.0f, 0.0f, 0.0f), red),
	CV(vec3(17.0f, -2.0f, 0.0f), red),
	CV(vec3(20.0f, 0.0f, 0.0f), red),
	CV(vec3(17.0f, 0.0f, 2.0f), red),
	CV(vec3(20.0f, 0.0f, 0.0f), red),
	CV(vec3(17.0f, 0.0f, -2.0f), red),
	//x character
	CV(vec3(20.0f, 3.0f, 0.0f), red),
	CV(vec3(26.0f, -3.0f, 0.0f), red),
	CV(vec3(20.0f, -3.0f, 0.0f), red),
	CV(vec3(26.0f, 3.0f, 0.0f), red),
	//y line
	CV(vec3(0.0f, 20.0f, 0.0f), green),
	CV(vec3(0.0f, -20.0f, 0.0f), green),
	//y arrow head
	CV(vec3(0.0f, 20.0f, 0.0f), green),
	CV(vec3(2.0f, 17.0f, 0.0f), green),
	CV(vec3(0.0f, 20.0f, 0.0f), green),
	CV(vec3(-2.0f, 17.0f, 0.0f), green),
	CV(vec3(0.0f, 20.0f, 0.0f), green),
	CV(vec3(0.0f, 17.0f, 2.0f), green),
	CV(vec3(0.0f, 20.0f, 0.0f), green),
	CV(vec3(0.0f, 17.0f, -2.0f), green),
	//y character
	CV(vec3(3.0f, 26.0f, 0.0f), green),
	CV(vec3(-3.0f, 20.0f, 0.0f), green),
	CV(vec3(-3.0f, 26.0f, 0.0f), green),
	CV(vec3(0.0f, 23.0f, 0.0f), green),
	//z line
	CV(vec3(0.0f, 0.0f, 20.0f), blue),
	CV(vec3(0.0f, 0.0f, -20.0f), blue),
	//z arrow head
	CV(vec3(0.0f, 0.0f, 20.0f), blue),
	CV(vec3(2.0f, 0.0f, 17.0f), blue),
	CV(vec3(0.0f, 0.0f, 20.0f), blue),
	CV(vec3(-2.0f, 0.0f, 17.0f), blue),
	CV(vec3(0.0f, 0.0f, 20.0f), blue),
	CV(vec3(0.0f, 2.0f, 17.0f), blue),
	CV(vec3(0.0f, 0.0f, 20.0f), blue),
	CV(vec3(0.0f, -2.0f, 17.0f), blue),
	//z character
	CV(vec3(0.0f, 3.0f, 26.0f), blue),
	CV(vec3(0.0f, 3.0f, 20.0f), blue),
	CV(vec3(0.0f, -3.0f, 20.0f), blue),
	CV(vec3(0.0f, 3.0f, 26.0f), blue),
	CV(vec3(0.0f, -3.0f, 20.0f), blue),
	CV(vec3(0.0f, -3.0f, 26.0f), blue),
};

inline AxisIndicator::AxisIndicator(Camera* c)
{

	//createTexture(FONT_DIR_PATH);

	this->cam = c;
	this->axisShader = Shader("axisVertShader.glsl", "axisFragShader.glsl");
	
	this->axisShader.use();
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(CV), (void*)0); //position
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(CV), (void*)(3 * sizeof(float))); //color
	glEnableVertexAttribArray(1);


	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, axisVerts.size() * sizeof(CV), axisVerts.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void AxisIndicator::draw(int screenX, int screenY)
{
	this->axisShader.use();
	glLineWidth(5);
	mat4 t = mat4(1);

	t = translate(t, vec3(screenX/2, screenY/2, 0));
	t = rotate(t, radians(cam->yaw-90), vec3(0, 1, 0));
	t = rotate(t, radians(cam->pitch), vec3(-cos(radians(cam->yaw-90)), 0, -sin(radians(cam->yaw - 90))));
	t = scale(t, vec3(5.0f));

	axisShader.setMatFour("transform", t);
	axisShader.setMatFour("projection", glm::ortho(0.0f, (float)screenX, 0.0f, (float)screenY, -1000000.0f, 1000000.0f));
	
	/*mat4 viewMat = mat4(cam->right.x, cam->right.y, cam->right.z, cam->front.x*20.0f,
		cam->up.x, cam->up.y, cam->up.z, cam->front.y * 20.0f,
		cam->front.x, cam->front.y, cam->front.z, cam->front.z * 20.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	viewMat = transpose(viewMat);
	
	
	axisShader.setMatFour("view", viewMat);*/
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glDrawArrays(GL_LINES, 0, axisVerts.size());

}

AxisIndicator::~AxisIndicator()
{
}


#endif AXIS_INIDCATOR_H