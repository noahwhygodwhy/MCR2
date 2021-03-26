
#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include "Asset.hpp"
//#include "Renderer.hpp"
//#include "Shader.hpp"
#include <glm/glm.hpp>

using namespace std;






Shader textShader;
unsigned int unicodePage00;


void loadFontTextures()
{
	printf("loading font textures\n");
	int width, height, channels;
	glActiveTexture(GL_TEXTURE2);

	glGenTextures(1, &unicodePage00);
	glBindTexture(GL_TEXTURE_2D, unicodePage00);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	unsigned char* data = stbi_load((TEXTURE_DIR_PATH + "font/unicode_page_00.png").c_str(), &width, &height, &channels, 4);
	if (data == NULL)
	{
		fprintf(stderr, "image not loaded\n");
		fprintf(stderr, "%s\n", (TEXTURE_DIR_PATH + "font/unicode_page_00.png").c_str());
		exit(-1);
	}
	
	printf("width: %i, height: %i, channels: %i\n", width, height, channels);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);

	textShader = Shader("textVertShader.glsl", "textFragShader.glsl");
}


struct Vert2D
{
	vec2 coords;
	vec2 uv;
	Vert2D(float x, float y, float u, float v)
	{
		this->coords = vec2(x, y);
		this->uv = vec2(u, v);
	}
};


class Text
{
public:
	Text(vec2 position, string characters, float scaleFactor);
	~Text();
	void draw(int screenX, int screenY);
	void setText(string characters);

private:
	void addCharacter(float x, float y, char c);
	unsigned int VAO, VBO;
	vec2 position = vec2(0);
	vector<Vert2D> verts;
};


const float characterSize = 96;//in 
const float characterSpacing = 2;//in pixels
const float keming = 7.0f / 16.0f;



void Text::addCharacter(float x, float y, char c)
{
	//printf("adding character %c at %f,%f\n", c, x, y);

	float width = 1.0 / 16.0;//each character's u and v will be 1/16th of the total cause 256x256 images made up of 16x16 characters

	//printf("c=%i\n", (int)c);

	int x0 = ((int)c) % 16;
	int y0 = ((int)c) / 16;

	//printf("%i, %i\n", x0, y0);


	float u0 = x0 * width;
	float v0 = 1.0f-(y0 * width)-width;
	float u1 = u0 + width;
	float v1 = v0 + width;

	//printf("uv: %f,%f to %f,%f\n", u0, v0, u1, v1);



	verts.push_back(Vert2D(x, y, u0, v0));
	verts.push_back(Vert2D(x + characterSize, y, u1, v0));
	verts.push_back(Vert2D(x, y+characterSize, u0, v1));

	verts.push_back(Vert2D(x + characterSize, y, u1, v0));
	verts.push_back(Vert2D(x + characterSize, y+ characterSize, u1, v1));
	verts.push_back(Vert2D(x, y + characterSize, u0, v1));
}


void Text::setText(string characters)
{
	float x = position.x;
	float y = position.y;
	verts.clear();
	for (char c : characters)
	{
		if (c == '\n' || c == '\r')
		{
			y -= characterSize + characterSpacing;
			x = position.x;
		}
		else if (c == ' ')
		{
			x += (characterSize*keming) + characterSpacing;
		}
		else
		{
			this->addCharacter(x, y, c);
			x += (characterSize*keming) + characterSpacing;
		}
	}
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glClear(GL_ARRAY_BUFFER);
	//glClearBufferData(GL_ARRAY_BUFFER, );
	//glBufferData(GL_ARRAY_BUFFER, this->verts.size() * sizeof(Vert2D), NULL, GL_STREAM_DRAW);
	glBufferData(GL_ARRAY_BUFFER, this->verts.size() * sizeof(Vert2D), this->verts.data(), GL_STREAM_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}


Text::Text(vec2 position, string characters, float scaleFactor)
{

	this->position = position;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vert2D), (void*)0); //position
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vert2D), (void*)(2 * sizeof(float))); //uv
	glEnableVertexAttribArray(1);

	this->setText(characters);


}

void Text::draw(int screenX, int screenY)
{

	//printf("drawing te");
	textShader.use();
	textShader.setMatFour("projection", glm::ortho(0.0f, (float)screenX, 0.0f, (float)screenY, -5.0f, 5.0f));

	//glActiveTexture(GL_TEXTURE0); //might break;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, unicodePage00);
	textShader.setInt("fontTexture", 2);
	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glDrawArrays(GL_TRIANGLES, 0, this->verts.size());
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Text::~Text()
{
}

#endif TEXT_RENDERER_H