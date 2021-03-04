
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
	Renderer r(1920, 1080);
	r.initializeOpenGL();

	Asset* ass = new Asset(r.loadTextures(TEXTURE_DIR_PATH));
	World w(saveFolder, ass, 1);

	Chunk c(saveFolder + "/region", 0, 0, ass);
}