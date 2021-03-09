/*! \file Chunk.hpp
* Contains the Chunk class and a few things that go with it.
*/

#ifndef CHUNK_H
#define CHUNK_H

#include <unordered_map>
#include "NBTTags.hpp"
#include <filesystem>
#include <fstream>
#include "gunzip.hpp"
#include <string>
#include <array>
#include "Asset.hpp"
#include <thread>
#include "glm/ext.hpp"

//#include "World.hpp"

class World;


using namespace std;
using namespace std::filesystem;


/*! \struct Section
	A structure to respresent a minecraft Section
*/
struct Section
{
	int y = -1;///<The Y level of the Section
	array<array<array<Model, 16>, 16>, 16> blocks; ///The 16^3 array of Models 
};


class Chunk
{
public:
	/*! \class Chunk
	* \param saveFolder The path to the "region" folder.
	* \param chkx The global x coord of the chunk
	* \param chkz The global z coord of the chunk
	* \param ass The Asset object for parsing models and blockstates and textures and stuff.
	*/
	Chunk(string saveFolder, int chkx, int chkz, Asset* ass);
	~Chunk();

	/*! \fn initializeBuffers
	* Sets up the VAO and VBO. MUST be called by the thread with the gl context.
	*/
	void initializeBuffers();

	/*! \fn bufferData
	* Buffers the prepared vertices. MUST be called by the thread with the gl context.
	*/
	void bufferData();

	/*! \fn draw
	* Draws the vertices in the VBO. MUST be called by the thread with the gl context.
	*/
	void draw();

	/*!
	* Parses the NBT data into an array of Models in the form of an array of Sections.
	* \param root The NBT tag root of the chunk <a href=https://minecraft.gamepedia.com/Chunk_format#NBT_structure">https://minecraft.gamepedia.com/Chunk_format#NBT_structure</a>
	* \param ass The Asset object containing all the information about blockstates, models, and textures.
	* \param[out] thisSections An array of up to 20 Section pointers. Any sections that do not exist are null.
	* \param[out] biomes An array of 4x4x4 biome indexes for the chunk;
	*/
	void createChunk(CompoundTag* root, Asset* ass, array<Section*, 20>& thisSections, array<array<array<uint32_t, 64>, 4>, 4>& biomes);
	
	/*! \fn cullChunk
	* Goes through the chunk, and sets faces that are obscured by other blocks to be culled.
	* Each element of each model has a 8 bit int, of which 6 are used to indicate which faces should be buffered.
	* If a block is next to anther block, and that block's cullforme is true, then the face on this block that is 
	* facing the other block is set to cull. If the face is on the edge of the chunk, it is not culled.
	* \param[out] sections The array of Sections to go through and cull blocks in.
	*/
	void cullChunk(const array<Section*, 20>& sections);

	/*! \fn generateVertices
	* For each block in the section, for each element of the block, for each face that is not culled, 
	generates two triangles worth of vertices. 
	* \param sections The array of Section pointers
	* \param biomes The array of biome indexes.
	*/
	void generateVertices(const array<Section*, 20>& sections, const array<array<array<uint32_t, 64>, 4>, 4>& biomes);

	/*! \fn cleanUp
	* Deletes a lot of the obects used in Chunk creation in order to save memory.
	*/
	void cleanUp();
	/*! \fn unbufferData
	* Not implemented yet.
	*/
	void unbufferData(); //idk...

	/*! \fn initializeChunk
	* A function that is basically a constructor, but isn't, allowing for it to be done by another thread.
	* The World calls initializeBuffer(), since it's in the opengl context, and then creates a thread to do this.
	* This function then does everything needed to create the chunk, and then tells the main thread's world when it's done. The world
	* then calls bufferData() since it has the opengl context. The world also sets drawable to true once buffered.
	*/
	void initializeChunk(World* w);

	bool drawable; ///< Whether or not the world is ready to be drawn.
	int chkx; ///<The global chunk x coordinate;
	int chkz; ///<The global chunk z coordinate;
	Asset* ass; ///<The Asset object for chunk creation
	string saveFolder; ///<The folder to look in for the save files.
	
private:
	/*! \fn cullForThisBlock
	* \param coord The coordinates of the block
	* \param sections The array of blocks to look for that other block in.
	* \returns True if the block should be culled for, false if not.
	*/
	bool cullForThisBlock(ivec3 coord, const array<Section*, 20>& sections);

	/*! \fn getSides
	* \param chkRelativeCoord the local coordinates of the block in a chunk;
	* \param sections The array of Sections to look in to find the block.
	* \returns a 8 bit int containing which sides should be rendered in this order: null/null/+y/-y/+x/-x/+z/-z
	*/
	uint8_t getSides(ivec3 chkRelativeCoord, const array<Section*, 20>& sections);

	/*! \fn getBiomeAttrs
	* \param biomes An array of biome indices for the chunk
	* \param coords the coordinates of the block to look up the biome attributes for.
	* \returns a vec2 containing the rainfall and temperature for the biome the block is in.
	*/
	vec2 getBiomeAttrs(const array<array<array<uint32_t, 64>, 4>, 4>& biomes, ivec3 coords);

	vector<Vert> verts; ///<The array of verts to be buffered and drawn
	unsigned int VBO, VAO; ///<The buffers to be buffered to and drawn from.
};

/*! \fn decompress
* Calculates which file the chunk is in, opens it, navigates to the proper location in the file,
* decompresses the data, and parses it to NBT data.
* \param saveFolder The folder to look for the save file in
* \param chkx The global chunk x coordinate
* \param chkz The global chunk z coordinate
* \returns An NBT tag filled with all the chunk data for the chunk. 
*/
CompoundTag* decompress(string saveFolder, int chkx, int chkz);


#endif