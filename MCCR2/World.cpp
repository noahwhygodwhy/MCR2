#include "World.hpp"

World::World(string saveFolder, Asset* ass, int range, int initx, int initz)
{
	printf("making world with init %i,%i\n", initx, initz); 
	this->saveFolder = saveFolder;
	this->chkx = initx;
	this->chkz = initz;
	this->range = range;
	this->ass = ass;
	this->adjustLoadedChunks();
}

World::~World()
{
}


void World::bufferItHere(ivec2 coords)
{
	queueLocker.lock();
	//printf("queueing %i,%i\n", coords.x, coords.y);
	this->bufferQueue.push(this->chunks.at(coords).second);
	queueLocker.unlock();

}
void initChunk(Chunk* c, World* w)
{
	c->initializeChunk(w);

}


//todo: I have to image this is where the parallelism would come in if I wanted to do that
//a separate thread to load the chunk, with an indicator when it's ready to be drawn
void World::adjustLoadedChunks()
{
	vector<Chunk*> chunksToGet;
	for (int x = this->chkx - this->range; x <= this->chkx + this->range; x++)
	{
		for (int z = this->chkz - this->range; z <= this->chkz + this->range; z++)
		{
			//printf("world loading chunk %i,%i\n", x, z);
			if (this->chunks.count(ivec2(x, z)) == 0)
			{
				//printf("have to load it fresh\n");

				Chunk* toAdd = new Chunk(this->saveFolder, x, z, this->ass);
				chunksToGet.push_back(toAdd);



				this->chunks.insert({ ivec2(x, z), pair(true,  toAdd)});
			}
			else
			{
				//printf("already loaded\n");
				this->chunks[ivec2(x, z)].first = true;
			}
		}
	}




	for (Chunk* c : chunksToGet)
	{
		thread th(initChunk, c, this);
		th.detach();
	}



	for (pair<vec2, pair<bool, Chunk*>> c : this->chunks)
	{
		if (abs(c.first.x) - this->chkx > 1 || abs(c.first.y) - this->chkz > 1)
		{
			c.second.first = false;
		}
		else
		{
			c.second.first = true;
		}
	}
}

void World::givePos(ivec3 pos)
{
	int newchkx = pos.x > 0 ? pos.x / 16 : ((pos.x-1) / 16) - 1;
	int newchkz = pos.z > 0 ? pos.z / 16 : ((pos.z - 1) / 16) - 1;
	if (newchkx != this->chkx || newchkz != this->chkz)
	{
		printf("moved to a new chunk from %i,%i to %i,%i\n", chkx, chkz, newchkx, newchkz);
		this->chkx = newchkx;
		this->chkz = newchkz;
		this->adjustLoadedChunks();
	}
	
}

void World::draw()
{
	queueLocker.lock();
	while (!bufferQueue.empty())
	{
		
		bufferQueue.front()->bufferData();
		bufferQueue.front()->drawable = true;

		//printf("buffering %i,%i\n", bufferQueue.front()->chkx, bufferQueue.front()->chkz);
		bufferQueue.pop();
	}
	queueLocker.unlock();
	for (pair<vec2, pair<bool, Chunk*>> c : this->chunks)
	{
		//printf("%i, %i is drawable: %s\n", c.first.x, c.first.y, c.second.second->drawable?"true":"false");
		if (c.second.first && c.second.second->drawable)
		{	
			c.second.second->draw();
		}
	}
}