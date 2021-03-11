/*!file main.cpp
*/


//constexpr function (sfm, ...)printf = (void*)0;

/*! \mainpage Minecraft Renderer Attempt Number 2
 *This is a minecraft renderer using opengl 4.6. Super early development. It is starting to work. Main issue rn are texture UVs and the handling of tint overlays. Z fighting in distant grass. Just fixed block rotation and grass biome color calculation. Taking a break and working on documentation. https://noahwhygodwhy.github.io/MCR2/ Accessing certain pages doesn't work as doxygen likes to put underscores at the front of some html file names. WIP :\
 *
 *It is working a lot better than attempt #1 though. Chunk creation has not only been significantly optimized, but has also been threaded, so loading chunks is almost as seamless as real minecraft. Block rotation is also much more efficient. Memory usage has been cut by something like 80%. I was being kinda stupid before lol. Once I get UVs done right, i'll start working on other things like water, or maybe some basic entities like chests.
 *
 *I'll make it user friendly in the future, but if you reaaaallly want to try it out I'm sure you can figure it out. main.cpp has the path to the save file. The paths to the minecraft asset stuff is all relative. There's a copy of it in the repository. It needs nuget packages glm and glfw in visual studio 19. Language standard C++17. Comment out "#define TEST" for better performance (enables threading). Also have to have command line arguments to disable depreciation warnings as one of the header files it uses had a bunch of depreciated stuff. Will update when it's working better.
 */

#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#include <glm/glm.hpp>


#include "Renderer.hpp"
#include "Chunk.hpp"
#include <string>
#include "Asset.hpp"
#include "Config.h"
#include <iostream>

using namespace std;
using namespace glm;

//#define TEST






#ifdef TEST
#define INIT_POS vec3(1, 2, 4);
string saveFolder = "C:/Users/noahm/AppData/Roaming/.minecraft/saves/New World2";
int radius = 0;
#else
#define INIT_POS vec3(1, 64, 1)
string saveFolder = "D:/OneDrive/Personal/MCCR2/New World";
int radius = 5;
#endif

int main(void)
{

/*#ifndef TEST
	std::fclose(stdout);
#endif*/

	printf("hello world\n");
	
	vec3 initPos = INIT_POS;


	Renderer r(1920, 1080);

	r.initializeOpenGL();

	Asset* ass = new Asset(r.loadTextures(TEXTURE_DIR_PATH));
	World w(saveFolder + "/region", ass, radius, initPos.x, initPos.z);

	r.run(w, initPos);
	//w.close();
}