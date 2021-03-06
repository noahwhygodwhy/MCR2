#pragma once
#ifndef RENDERER_H
#define RENDERER_H

//#include "Chunk.h"
#include "Asset.hpp"
//#include "RegionLoader.h"
#include "glad.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "GLSLReader.h"
#include "Shader.h"
#include "Camera.h"
#include <map>
#include <string>
#include <vector>
#include "World.hpp"

#include "stb_image.h"

using namespace std;


struct VertToBeRendered
{
	fvec3 coordinates;
	fvec2 uv;
	float texIndex;
	VertToBeRendered(vec3 c, vec2 u, float t)
	{
		coordinates = c;
		uv = u;
		texIndex = t;
	}
};

class Renderer
{
	unsigned int largeTextureStack = 0;
	unsigned int foliageColorMap = 0;
	int screenX;
	int screenY;
	Camera cam;
	Shader shader;
	GLFWwindow* window;
	int32_t layerCount;

public:
	Renderer(int screenX, int screenY);
	~Renderer();
	GLFWwindow* initializeOpenGL();
	void run(World& world, vec3 initPos);
	unordered_map<string, int> loadTextures(string path);
	//vector<Vert> convertWorldToVerts(const vector<culledModel>& culledWorld);
	//void addFace(vector<Vert>& verts, const vec3& a, const vec3& b, const vec3& c, const vec3& d, vec4 uv, int texRotation, int uvRotation, bool uvLock, int texture);

private:
	float normalizeTexture(const int& texID);
};
vec3 adjust(const float& x, const float& y, const float& z);
#endif;
