
#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#include <glm/glm.hpp>
#include "Renderer.hpp"
#include <iostream>
#include "Chunk.hpp"
#include <string>
#include "Asset.hpp"


#define TEST



#ifdef TEST
#define INIT_POS vec3(3, 2, 1);
string saveFolder = "C:/Users/noahm/AppData/Roaming/.minecraft/saves/New World2";
int radius = 0;
#else
#define INIT_POS vec3(1, 64, 1)
string saveFolder = "D:/OneDrive/Personal/MCCR2/New World";
int radius = 5;
#endif


//string saveFolder = "D:/OneDrive/Personal/MCCR2/New World";
//string saveFolder = "C:/Users/noahm/AppData/Roaming/.minecraft/saves/New World2";

using namespace std;
using namespace glm;


int main(void)
{


	vec3 initPos = INIT_POS;


	Renderer r(1920, 1080);

	r.initializeOpenGL();



	Asset* ass = new Asset(r.loadTextures(TEXTURE_DIR_PATH));
	World w(saveFolder + "/region", ass, radius, initPos.x, initPos.z);

	r.run(w, initPos);
	//w.close();
}