#ifndef WORLD_H
#define WORLD_H

#include <string>
#include "Asset.hpp"
#include <glm/glm.hpp>
#include <unordered_map>
#include "Chunk.hpp"

using namespace std;
using namespace std;


namespace std
{
	template<>
	struct hash<vec2>
	{
		size_t operator ()(const vec2& value) const
		{
			uint64_t key = ((uint64_t)value.x) << 32 | (uint64_t)value.y;
			key ^= (key >> 33);
			key *= 0xff51afd7ed558ccd;
			key ^= (key >> 33);
			key *= 0xc4ceb9fe1a85ec53;
			key ^= (key >> 33);
			return (size_t)key;
		}
	};
}







class World
{
	string saveFolder;
	int chkx;
	int chkz;
	int range;
	Asset* ass;
	unordered_map<vec2, pair<bool, Chunk*>> chunks;
	
public:

	World(string saveFolder, Asset* ass, int range, int initx, int initz);
	~World();
	void givePos(ivec3 pos);
	void adjustLoadedChunks();
	void draw();

private:

};

#endif