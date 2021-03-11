#pragma once
#ifndef OPEN_GL_C
#define OPEN_GL_C

#define STB_IMAGE_IMPLEMENTATION

#include <vector>
#include <filesystem>
#include <algorithm>
#include <glm/glm.hpp>
#include "Renderer.hpp"
#include "AxisIndicator.hpp"


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

double previousX;
double previousY;


void processInput(GLFWwindow* window, Camera& cam)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	int width, height;
	glfwGetWindowSize(window, &width, &height);


	cam.mouseInput((xpos - previousX) , (previousY - ypos)/height*width, true);

	previousX = xpos;
	previousY = ypos;

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
	screenX = x;
	screenY = y;
	cam = Camera(vec3(0, 0, 0), vec3(0, 1, 0), 0, 0, 5, 0.1f, 1);
	window = glfwCreateWindow(x, y, "Title Goes here", NULL, NULL);
}

Renderer::~Renderer()
{
}
float Renderer::normalizeTexture(const int& texID)
{
	return (float)std::max(0, std::min(layerCount - 1, (int)floor(texID + 0.5f)));
}









unordered_map<string, int> Renderer::loadTextures(string path)
{
	printf("starting to load textures\n");
	fflush(stdout);
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &(this->foliageColorMap));
	glBindTexture(GL_TEXTURE_2D, this->foliageColorMap);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height, channels;

	unsigned char* data = stbi_load((path+string("colormap/foliage.png")).c_str(), &width, &height, &channels, 3);



	//glTexStorage2D(GL_TEXTURE_2D, 1, GL_SRGB8_ALPHA8, width, height);
	//printf("it has %i channels\n", channels);
	if (data == NULL)
	{
		fprintf(stderr, "image not loaded\n");
		fprintf(stderr, "%s\n", (path + string("colormap/foliage.png")).c_str());
		exit(-1);
	}

	printf("width: %i, height: %i, channels: %i\n", width, height, channels);
	printf("data: %p\n", data);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	printf("foliage color map is null: %s\n", foliageColorMap == 0 ? "true" : "false");

	stbi_image_free(data);

	printf("foliage image loaded\n");



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
	width = 16;
	height = 16;
	layerCount = sixteenImages.size();

	printf("layerCount: %i\n", layerCount);


	glActiveTexture(GL_TEXTURE1);

	glGenTextures(1, &(this->largeTextureStack));
	glBindTexture(GL_TEXTURE_2D_ARRAY, this->largeTextureStack);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevelCount, GL_SRGB8_ALPHA8, width, height, layerCount);


	for (int32_t i = 0; i < sixteenImages.size(); i++)
	{
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, sixteenImages[i].second);
		toReturn[sixteenImages[i].first] = i;
		stbi_image_free(sixteenImages[i].second);//todo hopefully this doesn't break shit.
	}

	printf("all textures loaded into the largeTextureStack\n");

	return toReturn;
}


GLFWwindow* Renderer::initializeOpenGL()
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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "GLAD init failed" << endl;
		exit(-1);
	}

	shader = Shader("vertShader.glsl", "fragShader.glsl");

	glEnable(GL_CULL_FACE);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_BLEND);
	glFrontFace(GL_CCW);
	return window;
}


void Renderer::run(World& world, vec3 initPos)
{

	AxisIndicator ai(&cam);


	cam.setPos(initPos);
	world.givePos(cam.getPos());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, foliageColorMap);
	printf("#######foliage color map is null: %s\n", foliageColorMap == 0 ? "true" : "false");

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_ARRAY, largeTextureStack);
	//glActiveTexture(GL_TEXTURE0);
	//glActiveTexture(GL_TEXTURE1);


	glClearColor(0.529f, 0.808f, 0.922f, 1.0f);

	printf("layer count: %i\n", layerCount);
	shader.setInt("layerCount", layerCount);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window, cam);
		shader.use();

		shader.setInt("foliageColorMap", 0);
		shader.setInt("largeTextureStack", 1);


		mat4 view = cam.getView();
		//mat4 view = lookAt(vec3(32 + sin(glfwGetTime()/10)*20, 85, 32+cos(glfwGetTime()/10)*20), vec3(32, 60, 32), vec3(0.0f, 1.0f, 0.0f));
		shader.setMatFour("view", view);
		mat4 projection = perspective(radians(70.0f), (float)screenX / (float)screenY, 0.1f, 256.0f);
		shader.setMatFour("projection", projection);

		world.givePos(cam.getPos());
		world.draw();
		
		//glDrawArrays(GL_TRIANGLES, 0, originChunk.size());
		//glDrawArrays(GL_TRIANGLES, 0, originChunk.size());
		ai.draw(projection, screenX, screenY);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}


#endif