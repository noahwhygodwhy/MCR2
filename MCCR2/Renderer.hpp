/*! \file Renderer.hpp
	\brief Contains the Renderer class.

	\author Noah Alvard
*/


#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include "Asset.hpp"
#include "glad.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include <map>
#include <string>
#include <vector>
#include "World.hpp"

#include "stb_image.h"

using namespace std;


/*! \class Renderer
*	\brief A class that handles the openGL context and render loop.
*/
class Renderer
{
	unsigned int largeTextureStack = 0; ///<The pointer to the Large Texture Stack^tm texture
	unsigned int foliageColorMap = 0;///<The pointer to the foliage color map texture
	int screenX;///<The width of the screen in pixels
	int screenY;///<The height of the screen in pixels
	Camera cam;///<The "Camera" allowing for a more comfortable way of controlling the view matrix.
	Shader shader;///<The shader object that controls the opengl shaders.
	GLFWwindow* window;///<The opengl context
	int32_t layerCount;///<The number of layers in the Large Texture Stack.

public:
	/*! \fn Renderer
	* \param screenX The width of the screen in pixels
	* \param screenY The height of the screen in pixels.
	*/
	Renderer(int screenX, int screenY);
	~Renderer();
	/*! \fn initializeOpenGL
	* Creates the window and OpenGL context, and sets up many of the options.\
	* \returns The OpenGL context.
	*/

	GLFWwindow* initializeOpenGL();
	/*! \fn run
	* Sets up the camera, ensures the world has the correct location, binds textures, and starts the rendering loop.
	* \param world The minecraft world to render.
	* \param initPos The initial position of the camera's view.
	*/
	void run(World& world, vec3 initPos);

	/*! \fn loadTextures
	* Loads the folage texture map.
	* Loads every texture in the minecraft texture folder to a layer in Large Texture Stack
	* \param path The path to the minecraft asset folder.
	* \returns A map between texture names, and indices in the Large Texture Stack.
	*/
	unordered_map<string, int> loadTextures(string path);

private:
	/*! \fn normalizeTexture
	* Ensures that the texture ID is cleanly in a layer of the LargeTextureMap to prevent bad texture sampling.
	* \param texID Yhe index in the large texture stack.
	* \returns A float that is guaranteed to index into the LargeTextureMap to the right texture, and avoid bad texture sampling.
	*/
	float normalizeTexture(const int& texID);
};
#endif;
