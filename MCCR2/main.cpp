
#include <GLFW/glfw3.h>


#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include "Renderer.h"
#include <iostream>




using namespace std;
using namespace glm;

int main(void)
{
	Renderer r(1920, 1080);

	r.initializeOpenGL();
}