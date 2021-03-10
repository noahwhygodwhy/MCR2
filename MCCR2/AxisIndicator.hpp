#include "Renderer.hpp"
#include "Shader.hpp"
#include <string>
#include "Camera.hpp"
#include <vector>
using namespace std;


class AxisIndicator
{
public:
	AxisIndicator(Camera* c);
	~AxisIndicator();
	void draw(const mat4& persp, int screenX, int screenY);
private:
	unsigned int VAO = 0, VBO = 0;
	Shader axisShader;
	Camera* cam;
	
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
	CV(vec3(20.0f, 0.0f, 0.0f), red),
	CV(vec3(-20.0f, 0.0f, 0.0f), red),
	CV(vec3(0.0f, 20.0f, 0.0f), green),
	CV(vec3(0.0f, -20.0f, 0.0f), green),
	CV(vec3(0.0f, 0.0f, 20.0f), blue),
	CV(vec3(0.0f, 0.0f, -20.0f), blue),
};

inline AxisIndicator::AxisIndicator(Camera* c)
{
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

void AxisIndicator::draw(const mat4& persp, int screenX, int screenY)
{
	this->axisShader.use();
	glLineWidth(2);
	mat4 t = mat4(1);

	t = scale(t, vec3(3));



	t = translate(t, vec3(screenX/2, screenY/2, 0));

	//t = rotate(t, radians(45.0f), vec3(0, 1, 0));
	//t = rotate(t, radians(45.0f), vec3(1, 1, 1));

	axisShader.setMatFour("transform", t);
	//axisShader.setMatFour("projection", mat4(1.0));
	axisShader.setMatFour("projection", glm::ortho(0.0f, (float)screenX, 0.0f, (float)screenY, -100.0f, 100.0f));
	axisShader.setMatFour("view", lookAt(vec3(-5.0f), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f)));
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glDrawArrays(GL_LINES, 0, 36);

}

AxisIndicator::~AxisIndicator()
{
}

