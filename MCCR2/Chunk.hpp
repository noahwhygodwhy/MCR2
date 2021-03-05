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


using namespace std;
using namespace std::filesystem;

struct Section
{
	int y;
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
	void createChunk(CompoundTag* ct, Asset* ass);
	void cullChunk();
	void generateVertices();
	void unbufferData(); //idk..

	
private:
	bool cullForThisBlock(ivec3 coord);
	uint8_t getSides(ivec3 chkRelativeCoord);
	bool drawable = false;
	int chkx;
	int chkz;
	array<array<array<uint32_t, 80>, 4>, 4> biomes;
	array<Section*, 20> sections;
	vector<Vert> verts;
	unsigned int VBO, VAO, EBO;
};


CompoundTag* decompress(string saveFolder, int chkx, int chkz);


#endif