



#include "Asset.hpp"
#include "Renderer.hpp"
#include <glm/glm.hpp>

using namespace std;






Shader textShader;
unsigned int unicodePage00;


void loadFontTextures()
{
	createTexture(&unicodePage00, TEXTURE_DIR_PATH + "/font/unicode_page00.png", 4);
	textShader = Shader("textVertShader.glsl", "textFragShader.glsl");
}


struct Vert2D
{
	vec2 coords;
	float id;
	Vert2D(float x, float y, float id)
	{
		this->coords = vec2(x, y);
		this->id = id;
	}
};


class Text
{
public:
	Text(vec2 position, string characters, float scaleFactor);
	~Text();
	void draw();
	void setText(string characters);

private:
	void addCharacter(float x, float y, char c);
	unsigned int VAO, VBO;
	vec2 position = vec2(0);
	vector<Vert2D> verts;
};


const float characterSize = 16;//in 
const float characterSpacing = 2;//in pixels




void Text::addCharacter(float x, float y, char c)
{
	verts.push_back(Vert2D(x, y, c));
	verts.push_back(Vert2D(x, y+characterSize, c));
	verts.push_back(Vert2D(x + characterSize, y, c));

	verts.push_back(Vert2D(x + characterSize, y, c));
	verts.push_back(Vert2D(x + characterSize, y+ characterSize, c));
	verts.push_back(Vert2D(x, y + characterSize, c));
}


void Text::setText(string characters)
{
	float x = position.x;
	float y = position.y;

	for (char c : characters)
	{
		if (c == '\n' || c == '\r')
		{
			y += characterSize + characterSpacing;
			x = position.x;
		}
		else
		{
			this->addCharacter(x, y, c);
		}
	}
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, this->verts.size() * sizeof(Vert2D), this->verts.data(), GL_STATIC_DRAW);

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

	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Vert2D), (void*)(2 * sizeof(float))); //char
	glEnableVertexAttribArray(1);

	setText(characters);


}

void Text::draw()
{
	textShader.use();
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glDrawArrays(GL_TRIANGLES, 0, this->verts.size());
}

Text::~Text()
{
}