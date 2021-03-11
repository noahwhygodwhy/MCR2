#ifndef WORLD_H
#define WORLD_H

#include <string>
#include "Asset.hpp"
#include <glm/glm.hpp>
#include <unordered_map>
#include "Chunk.hpp"
#include <queue>
#include <mutex> 
#include "Config.h"


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
	template<>
	struct hash<ivec2>
	{
		size_t operator ()(const ivec2& value) const
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





/*! \class World
* \brief A class to represent the world. Also handles the loading/deloading of chunks.
*/
class World
{
	string saveFolder; ///<The path of the folder containing the minecraft save.
	int chkx; ///<The global chunk x coordinate for which the center of the rendering is in.
	int chkz; ///<The global chunk z coordinate for which the center of the rendering is in.
	int range; ///<How many chunks in each direction should be loaded/set to be rendered.
	Asset* ass; ///<The Asset object allowing for chunk loading.
	unordered_map<ivec2, pair<bool, Chunk*>> chunks; ///<A map pairing the chunk's global coordinates, to a pair of a boolean of whether or not it should be drawn, and the chunk itself.
	queue<Chunk*> bufferQueue; ///<A buffer that chunk loading threads will add their chunk to when it's ready to be buffered.
	mutex queueLocker; ///<A mutex that is locked before bufferQueue is accessed, and unlocked after.
	
public:
	
	/*! \fn World
	*	\param saveFolder The path of the folder containing the minecraft save
	*	\param ass The Asset object allowing for chunk loading
	*	\param range How many chunks in each direction to be loaded/set to be rendered
	*	\param initx The x coordinate of the location of the chunk to load chunks in a square out from.
	*	\param initz The z coordinate of the location of the chunk to load chunks in a square out from.
	*/
	World(string saveFolder, Asset* ass, int range, int initx, int initz);
	~World();
	/*! \fn givePos
	*	\param pos The new center position.
	*/
	void givePos(ivec3 pos);
	/*! \fn adjustLoadedChunks
	*	\brief Re-evaluates which chunks should be loaded. Loads which chunks need to be loaded, and unload chunks that are more than one chunk out of range.
	*/
	void adjustLoadedChunks();
	/*! \fn draw
	*	\brief Draws each chunk that is set to be drawable.
	*/
	void draw();
	/*! \fn bufferItHere
	*	\brief Buffers the chunk at coords.
	*	
	*	This function is called by a different thread that was responsible for creating the chunk. The vertices must be buffered by the thread with the opengl context.
	*	This function adds the chunk to the bufferQueue so when the next frame hits, the main thread buffers the vertices from that chunk.
	*/
	void bufferItHere(ivec2 coords);

private:

};

#endif