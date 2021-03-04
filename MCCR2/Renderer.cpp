#pragma once
#ifndef OPEN_GL_C
#define OPEN_GL_C

#define STB_IMAGE_IMPLEMENTATION

#include "Shader.h"
#include "World.hpp"
#include <vector>
#include <filesystem>
#include <algorithm>
#include <glm/glm.hpp>
#include "Renderer.hpp"


using namespace std;
using namespace std::filesystem;
using namespace glm;



float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame


////////////////////////////////////


void frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouseCursorPossCallback(GLFWwindow* window, double xpos, double ypos)
{
	//cam.mouseInput(xpos, ypos, true);
}

void processInput(GLFWwindow* window, Camera& cam)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		//printf("W");
		cam.keyboardInput(Direction::FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		//printf("S");
		cam.keyboardInput(Direction::BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		//printf("A");
		cam.keyboardInput(Direction::LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		//printf("D");
		cam.keyboardInput(Direction::RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		//printf("D");
		cam.keyboardInput(Direction::DOWN, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		//printf("D");
		cam.keyboardInput(Direction::UP, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		//printf("E");
		cam.keyboardInput(Direction::YAW_RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		//printf("Q");
		cam.keyboardInput(Direction::YAW_LEFT, deltaTime);
	}
}


Renderer::Renderer(int x, int y)
{
	layerCount = 0;
	EBO = 0;
	VBO = 0;
	VAO = 0;
	screenX = x;
	screenY = y;
	cam = Camera(vec3(0, 0, 0), vec3(0, 1, 0), 0, 0, 10, 1, 1);
	window = glfwCreateWindow(x, y, "Title Goes here", NULL, NULL);
}

Renderer::~Renderer()
{
}

/*
vec3 adjust(const float& x, const float& y, const float& z)
{
	return fvec3(( x / 16.0f), (y / 16.0f), (z / 16.0f));
}
*/float Renderer::normalizeTexture(const int& texID)
{
	return (float)std::max(0, std::min(layerCount - 1, (int)floor(texID + 0.5f)));
}

/*

vec4 rotateAroundCenter(const mat4& rm, const vec4& b)
{
	return vec4(vec3(0.5f), 0.0f) + (rm * (b - vec4(vec3(0.5f), 0.0f)));
}

//vec4 rotateUV(int degrees, const vec4& b)
//{
//	vec4 f = b;
//	for (int i = 0; i < degrees / 90; i++)
//	{
//		f = vec4(f.z, f.y, f.x, f.w);
//	}
//	return f;
//}

vec2 rot90(vec2 v)
{
	return vec2( v.y, 1.0f-v.x);
}

void OpenGL::addFace(vector<Vert>& verts, const vec3& a, const vec3& b, const vec3& c, const vec3& d, vec4 uv, int texRotation, int uvRotation, bool uvLock, int texture)
{
	vec2 uv00 = vec2(uv.x, uv.y) / 16.0f;
	vec2 uv01 = vec2(uv.x, uv.w) / 16.0f;
	vec2 uv11 = vec2(uv.z, uv.w) / 16.0f;
	vec2 uv10 = vec2(uv.z, uv.y) / 16.0f;

	if (uvLock)
	{
		texRotation -= uvRotation;
	}

	texRotation = ((texRotation % 360) + 360) % 360;//confines to 0 to 360

	for (int i = 0; i < texRotation; i += 90)
	{
		uv00 = rot90(uv00);
		uv01 = rot90(uv01);
		uv11 = rot90(uv11);
		uv10 = rot90(uv10);
	}


	Vert v00(a, uv00, texture);
	Vert v01(b, uv01, texture);
	Vert v11(c, uv11, texture);
	Vert v10(d, uv10, texture);

	verts.push_back(v00);
	verts.push_back(v11);
	verts.push_back(v10);

	verts.push_back(v00);
	verts.push_back(v01);
	verts.push_back(v11);
}

vector<Vert> OpenGL::convertWorldToVerts(const vector<culledModel>& culledWorld)
{
	printf("converting to verts\n");

	vec3 ppp;
	vec3 ppn;
	vec3 pnp;
	vec3 pnn;
	vec3 npp;
	vec3 npn;
	vec3 nnp;
	vec3 nnn;

	vector<Vert> verts;
	for (culledModel cm: culledWorld)//for each block that might exist
	{
		for (Element e : cm.m.elements)//for each element of that block that might exist
		{
			mat4 rm = mat4(1.0f);//rm stands for rotation matrix
			rm = rotate(rm, (float)radians((float)e.yRot), vec3(0, -1, 0));
			rm = rotate(rm, (float)radians((float)e.xRot), vec3(-1, 0, 0));

			ppp = rotateAroundCenter(rm, vec4(adjust(e.to.x, e.to.y, e.to.z), 0.0f)) + vec4(cm.coords, 0.0f);
			ppn = rotateAroundCenter(rm, vec4(adjust(e.to.x, e.to.y, e.from.z), 0.0f)) + vec4(cm.coords, 0.0f);
			pnp = rotateAroundCenter(rm, vec4(adjust(e.to.x, e.from.y, e.to.z), 0.0f)) + vec4(cm.coords, 0.0f);
			pnn = rotateAroundCenter(rm, vec4(adjust(e.to.x, e.from.y, e.from.z), 0.0f)) + vec4(cm.coords, 0.0f);
			npp = rotateAroundCenter(rm, vec4(adjust(e.from.x, e.to.y, e.to.z), 0.0f)) + vec4(cm.coords, 0.0f);
			npn = rotateAroundCenter(rm, vec4(adjust(e.from.x, e.to.y, e.from.z), 0.0f)) + vec4(cm.coords, 0.0f);
			nnp = rotateAroundCenter(rm, vec4(adjust(e.from.x, e.from.y, e.to.z), 0.0f)) + vec4(cm.coords, 0.0f);
			nnn = rotateAroundCenter(rm, vec4(adjust(e.from.x, e.from.y, e.from.z), 0.0f)) + vec4(cm.coords, 0.0f);

			if (cm.faces & 0b00100000 && !(e.up.cullFace & 0b11000000))//+y
			{
				addFace(verts, npn, npp, ppp, ppn, e.up.uv, e.up.rotation, e.yRot, e.uvLock, e.up.texture);
			}
			if (cm.faces & 0b00010000 && !(e.down.cullFace & 0b11000000))//-y
			{
				addFace(verts, nnp, nnn, pnn, pnp, e.down.uv, e.down.rotation, e.yRot, e.uvLock, e.down.texture);
			}
			if (cm.faces & 0b00001000 && !(e.east.cullFace & 0b11000000))//+x
			{
				addFace(verts, ppp, pnp, pnn, ppn, e.east.uv, e.east.rotation, e.yRot % 180 == 90 ? 0 :e.xRot, e.uvLock, e.east.texture);
			}
			if (cm.faces & 0b00000100 && !(e.west.cullFace & 0b11000000))//-x
			{
				addFace(verts, npn, nnn, nnp, npp, e.west.uv, e.west.rotation, e.yRot % 180 == 90 ? 0 : e.xRot, e.uvLock, e.west.texture);
			}
			if (cm.faces & 0b00000010 && !(e.south.cullFace & 0b11000000))//+z
			{
				addFace(verts, npp, nnp, pnp, ppp, e.south.uv, e.south.rotation, e.yRot % 180 == 0 ? 0 : e.xRot, e.uvLock, e.south.texture);
			}
			if (cm.faces & 0b00000001 && !(e.north.cullFace & 0b11000000))//-z
			{
				addFace(verts, ppn, pnn, nnn, npn, e.north.uv, e.north.rotation, e.yRot % 180 == 0 ? 0 : e.xRot, e.uvLock, e.north.texture);
			}
		}
	}
	printf("converted to verts\n");
	return verts;
}
*/
unordered_map<string, int> Renderer::loadTextures(string path)
{
	unordered_map<string, int> toReturn;
	directory_iterator textureDir(path + "block/");
	vector<pair<string, unsigned char*>> sixteenImages;

	for (auto textureFile : textureDir)
	{
		if (textureFile.path().extension().u8string() == ".png")
		{
			//printf("doing image %s\n", textureFile.path().filename().stem().u8string().c_str());
			int width, height, channels;

			//TODO: Come back here and figure out animated textures. For now i'm just taking the top most frame and using that statically.
			//todo maybe each one will be a mipmap in each level?
			unsigned char* initImage = stbi_load(textureFile.path().u8string().c_str(), &width, &height, &channels, 4);

			//printf("it has %i channels\n", channels);
			if (initImage == NULL)
			{
				fprintf(stderr, "image not loaded\n");
				fprintf(stderr, "%s\n", textureFile.path().u8string().c_str());
				exit(-1);
			}

			//so the number 1024 might seem like a magic number, but let me explain
			//16pixels*16pixels*4channels(bytes) = 1024, because all the textures come in vertical
			//The images are loaded in row major order, so this works
			unsigned char* cutImage = new unsigned char[1024];
			if (!cutImage)
			{
				fprintf(stderr, "memcpy error #5\n");
				exit(-1);
			}
			else
			{
				memcpy(cutImage, initImage, 1024);//this shoulllld be safe
			}
			sixteenImages.push_back({ textureFile.path().filename().stem().u8string(), cutImage });
		}
	}

	printf("all images in sixteenImages\n");
	int32_t mipLevelCount = 1;//maybe eventually fix?
	int32_t width = 16;
	int32_t height = 16;
	layerCount = sixteenImages.size();

	printf("layerCount: %i\n", layerCount);


	glGenTextures(1, &(this->largeTextureStack));
	glBindTexture(GL_TEXTURE_2D_ARRAY, this->largeTextureStack);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevelCount, GL_SRGB8_ALPHA8, width, height, layerCount);

	for (int32_t i = 0; i < sixteenImages.size(); i++)
	{
		if (i == 440)
		{
			printf("440 iS ########### %s\n", sixteenImages[i].first.c_str());
		}
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, sixteenImages[i].second);
		toReturn[sixteenImages[i].first] = i;
		stbi_image_free(sixteenImages[i].second);//todo hopefully this doesn't break shit.
	}
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	printf("all textures loaded into the largeTextureStack\n");

	return toReturn;
}


void Renderer::initializeOpenGL()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SRGB_CAPABLE, 1);
	//glfwWindowHint(GLFW_SAMPLES, 16);
	window = glfwCreateWindow(screenX, screenY, "Title Goes here", NULL, NULL);

	if (window == NULL)
	{
		cout << "Window creation failed" << endl;
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "GLAD init failed" << endl;
		exit(-1);
	}

	shader = Shader("vertShader.glsl", "fragShader.glsl");

	glEnable(GL_CULL_FACE);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	GLsizei stride = sizeof(Vert);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0); //position
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float))); //uv
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float))); //tex layer
	glEnableVertexAttribArray(2);

}


void Renderer::run(World& world)
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, largeTextureStack);
	glClearColor(0.529f, 0.808f, 0.922f, 1.0f);

	shader.setInt("layerCount", layerCount);

	while (!glfwWindowShouldClose(window))
	{
		shader.use();

		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);


		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window, cam);

		mat4 view = cam.getView();
		//mat4 view = lookAt(vec3(32 + sin(glfwGetTime()/10)*20, 85, 32+cos(glfwGetTime()/10)*20), vec3(32, 60, 32), vec3(0.0f, 1.0f, 0.0f));
		shader.setMatFour("view", view);
		mat4 projection = perspective(radians(70.0f), (float)screenX / (float)screenY, 0.1f, 256.0f);
		shader.setMatFour("projection", projection);

		world.giveLocation(cam.)

		world.draw();//TODO:
		//glDrawArrays(GL_TRIANGLES, 0, originChunk.size());
		//glDrawArrays(GL_TRIANGLES, 0, originChunk.size());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}


#endif