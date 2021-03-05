
#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#include <glm/glm.hpp>
#include "Renderer.hpp"
#include <iostream>
#include "Chunk.hpp"
#include <string>
#include "Asset.hpp"


string saveFolder = "D:/OneDrive/Personal/MCCR2/New World";

using namespace std;
using namespace glm;


int main(void)
{


	vec3 initPos = vec3(0, 128, 0);


	Renderer r(1920, 1080);

	r.initializeOpenGL();



	Asset* ass = new Asset(r.loadTextures(TEXTURE_DIR_PATH));
	World w(saveFolder + "/region", ass, 7, initPos.x, initPos.z);

	r.run(w);
	//w.close();
}