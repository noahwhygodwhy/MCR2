#include "World.hpp"


World::World(string saveFolder, Asset ass, int range)
{

}

World::~World()
{
}


void World adjustLoadedChunks()
{

}

void World::givePos(ivec3 pos)
{

	//TODO: need to add some lag to unloading until it's more than one out of range
	//otherwise jumping back and forth over chunk edge will cause trouble
	int newchkx = pos.x > 0 ? pos.x / 16 : ((pos.x-1) / 16) - 1;
	int newchkz = pos.z > 0 ? pos.z / 16 : ((pos.z - 1) / 16) - 1;
	if (newchkx != this->chkx || newchkz != this->chkz)
	{
		this->chkx = newchkx;
		this->chkz = newchkz;
		this->adjustLoadedChunks();
	}
	
}