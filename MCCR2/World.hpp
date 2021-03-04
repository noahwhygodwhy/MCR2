#ifndef WORLD_H
#define WORLD_H

#include <string>
#include "Asset.hpp"
#include <glm/glm.hpp>

using namespace std;
using namespace std;

class World
{
	int chkx;
	int chkz;
public:

	World(string saveFolder, Asset ass, int range);
	~World();
	void givePos(ivec3 pos);

private:

};

#endif