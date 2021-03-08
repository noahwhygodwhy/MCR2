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
#

class World;


using namespace std;
using namespace std::filesystem;

struct Section
{
	int y = -1;
	vector<Model> palette;
	array<array<array<Model, 16>, 16>, 16> blocks;
};


class Chunk
{
public:
	Chunk(string saveFolder, int chkx, int chkz, Asset* ass);
	~Chunk();
	void initializeBuffers();
	void bufferData();
	void draw();
	void createChunk(CompoundTag* root, Asset* ass, array<Section*, 20>& thisSections, array<array<array<uint32_t, 64>, 4>, 4>& biomes);
	void cullChunk(const array<Section*, 20>& sections);
	void generateVertices(const array<Section*, 20>& sections, const array<array<array<uint32_t, 64>, 4>, 4>& biomes);
	void cleanUp();
	void unbufferData(); //idk...


	void initializeChunk(World* w);

	bool drawable;
	int chkx;
	int chkz;
	Asset* ass;
	string saveFolder;
	
private:
	bool cullForThisBlock(ivec3 coord, const array<Section*, 20>& sections);
	uint8_t getSides(ivec3 chkRelativeCoord, const array<Section*, 20>& sections);
	vec2 getBiomeAttrs(const array<array<array<uint32_t, 64>, 4>, 4>& biomes, ivec3 coords);
	vector<Vert> verts;
	unsigned int VBO, VAO, EBO;
};


CompoundTag* decompress(string saveFolder, int chkx, int chkz);


#endif