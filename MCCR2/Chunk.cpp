#include "Chunk.hpp"
#include "Renderer.hpp"
#include "Biomes.hpp"
#include <algorithm>
#include "glm/ext.hpp" 

using namespace std;
using namespace std::filesystem;


//stolen from some stack overflow
/*#define BPATTERN "%c%c%c%c%c%c%c%c"
#define BTB(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') */


Chunk::Chunk(string saveFolder, int chkx, int chkz, Asset* ass)
{
	this->chkx = chkx;
	this->chkz = chkz;
	this->ass = ass;
	this->saveFolder = saveFolder;
	this->initializeBuffers();


}




void Chunk::initializeChunk(World* world)
{
	//printf("Creating chunk %i,%i\n", chkx, chkz);
	CompoundTag* ct = decompress(saveFolder, chkx, chkz);
	//printf("ct is null: %s\n", ct == 0 ? "true" : "false");
	if (ct != NULL)
	{
		//printf("%i,%i ct not null: %s\n", chkx, chkz, ct == NULL ? "false" : "true");
		//printf("initialized buffers");


		array<array<array<uint32_t, 64>, 4>, 4> biomes;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				biomes[i][j].fill(0);
			}
		}

		array<Section*, 20> sections;

		this->createChunk(ct, ass, sections, biomes);
		this->cullChunk(sections);
		delete(ct);
		this->generateVertices(sections, biomes);

		world->bufferItHere(vec2(chkx, chkz));
		//printf("finished chunk %i, %i\n", chkx, chkz);
	}
}

void Chunk::cleanUp()
{

	verts.clear();
}

Chunk::~Chunk()
{
}






//TODO: this could be done better. It's rotating a cube in 90* increments. there has to be a better way than a rotation matrix
vec4 rotateAroundCenter(const mat4& rm, const vec4& b)
{
	return vec4(vec3(0.5f), 0.0f) + (rm * (b - vec4(vec3(0.5f), 0.0f)));
}
vec3 scaleAroundCenter(float factor, vec3 b)
{
	return vec3(0.5f) + ((b - vec3(0.5f))*factor);
}

//TODO: this is what sets up the UVs, so when it's wrong, this is to blame
void addFace(vector<Vert>& verts, const vec3& ax, const vec3& bx, const vec3& cx, const vec3& dx, vec4 uv, int texRotation, int uvRotation, bool uvLock, int texture, int tintIndex, vec2 tintUV, const vec3& blockCoords)
{
	
	vec3 a = ax;
	vec3 b = bx;
	vec3 c = cx;
	vec3 d = dx;

	tintUV = vec2(0.2f, 0.2f);
	if (tintIndex == -1)
	{
		//printf("setting tintUV to -1\n");
		tintUV = vec2(-1.0f, -1.0f);
	}
	else //TODO:
	{
		float scaleFactor = 1.005f;
		//printf("tint index is %i\n", tintIndex);
		a = scaleAroundCenter(scaleFactor, a);
		b= scaleAroundCenter(scaleFactor, b);
		c = scaleAroundCenter(scaleFactor, c);
		d = scaleAroundCenter(scaleFactor, d);
	}

	a += blockCoords;
	b += blockCoords;
	c += blockCoords;
	d += blockCoords;


	vec2 uv00 = vec2(uv.x, uv.y) / 16.0f;
	vec2 uv01 = vec2(uv.x, uv.w) / 16.0f;
	vec2 uv11 = vec2(uv.z, uv.w) / 16.0f;
	vec2 uv10 = vec2(uv.z, uv.y) / 16.0f;
	//printf("\n");
	//printf("texture rotation is %i\n", texRotation);
	if (uvLock)
	{
		//printf("uvlocked\n");
		texRotation -= uvRotation;
	}
	else
	{
		//printf("not uvlocked\n");
	}
	texRotation = ((texRotation % 360) + 360) % 360;//confines to 0 to 360

	//bprintf("texture rotation is %i\n", texRotation);

	/*for (int i = 0; i < texRotation; i += 90)//maybe disable rotation? or have a look at it with a small sample size (1)?glhf
	{
		uv00 = rot90(uv00);
		uv01 = rot90(uv01);
		uv11 = rot90(uv11);
		uv10 = rot90(uv10);
	}*/


	Vert v00(a, uv00, tintUV, texture);
	Vert v01(b, uv01, tintUV, texture);
	Vert v11(c, uv11, tintUV, texture);
	Vert v10(d, uv10, tintUV, texture);

	verts.push_back(v00);
	verts.push_back(v11);
	verts.push_back(v10);

	verts.push_back(v00);
	verts.push_back(v01);
	verts.push_back(v11);
}




ivec3 adjust(fvec3 in)
{
	return ivec3(in / 16.0f);
}

vec3 adjust(const float& x, const float& y, const float& z)
{
	return fvec3((x / 16.0f), (y / 16.0f), (z / 16.0f));
}


vec2 Chunk::getBiomeAttrs(const array<array<array<uint32_t, 64>, 4>, 4>& biomes, ivec3 coords)
{
	try
	{
		//printf("about to get biome data\n");
		uint32_t biomeID = biomes[(coords.z % 16) / 4][(coords.x % 16) / 4][coords.y / 4];
		//printf("biome id = %u\n", biomeID);
		if (biomeID > 255)
		{
			biomeID = 0; //TODO: fix
		}
		Biome b = biomeData[biomeID];
		//printf("got biome data for %s\n", b.name.c_str());
		float temp = std::clamp(b.temp, 0.0f, 1.0f);
		temp -= (coords.y - SEA_LEVEL) * TEMP_LOSS_PER_METER;
		float rain = std::clamp(b.rain, 0.0f, 1.0f) * temp;
		return vec2(temp, rain);
	}
	catch (exception e)//TODO:
	{

		return vec2(0.2f, 0.2f);
	}

}

fvec3 performRotations(int xRot, int yRot, ivec3 given)
{
	fvec3 in = fvec3(given) - fvec3(0.5f);
	//in -= vec3(0.5f);
	//printf("in: %f, %f, %f\n", in.x, in.y, in.z);
	fvec3 out = in;
	
	switch (xRot)
	{
	case 90:
		//printf("90 x rot\n");
		out = vec3(out.x, -out.y, out.z);
		break;
	case 180:
		out = vec3(out.x, -out.y, -out.z);
		break;
	case 270:
		out = vec3(out.x, out.z, -out.y);
		break;
	default:
		//printf("no x rot\n");
		//out = in;
		break;
	}

	switch (yRot)
	{
	case 90:
		//printf("90 y rot\n");
		out = vec3(-out.z, out.y, out.x);
		break;
	case 180:
		out = vec3(-out.x, out.y, -out.z);
		break;
	case 270:
		out = vec3(out.z, out.y, -out.x);
		break;
	default:
		//printf("no y rot\n");
		//out = in;
		break;
	}
	//("out: %f, %f, %f\n", out.x, out.y, out.z);
	out += fvec3(0.5f);
	return out;
}


void Chunk::generateVertices(const array<Section*, 20>& sections, const array<array<array<uint32_t, 64>, 4>, 4>& biomes)
{
	vec3 ppp;
	vec3 ppn;
	vec3 pnp;
	vec3 pnn;
	vec3 npp;
	vec3 npn;
	vec3 nnp;
	vec3 nnn;


	for (const auto& sec : sections)
	{
		if (sec != NULL)
		{
			//chrono::milliseconds start = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
			for (const auto& yLevel : sec->blocks)
			{
				for (const auto& zSlice : yLevel)
				{
					for (const auto& block : zSlice)
					{
						if (block.model != "block/air" && block.model != "block/void_air" && block.model != "block/cave_air" && block.model != "NULL") //TODO: needs to include other types of air too
						{
							//printf("-----------------------------\nverticizing %s\n", block.model.c_str());
							for (const Element& e : block.elements)
							{

								//printf("model is from %i,%i,%i to %i,%i,%i\n", e.from.x, e.from.y, e.from.z, e.to.x, e.to.y, e.to.z);
								
								printf("xrot: %i, yrot: %i\n", e.xRot, e.yRot);

								//printf("to: %i, %i, %i\n", e.to.x, e.to.y, e.to.z);
								//printf("from: %i, %i %i\n", e.from.x, e.from.y, e.from.z);

								ivec3 adjTo = adjust(e.to);
								ivec3 adjFrom = adjust(e.from);

								//printf("to: %i, %i, %i\n", adjTo.x, adjTo.y, adjTo.z);
								//printf("from: %i, %i %i\n", adjFrom.x, adjFrom.y, adjFrom.z);

								//int eyRot = 270;

								//adjTo = performRotations(e.xRot, eyRot, adjTo);
								//adjFrom = performRotations(e.xRot, eyRot, adjFrom);

								//printf("to: %i, %i, %i\n", adjTo.x, adjTo.y, adjTo.z);
								//printf("from: %i, %i %i\n", adjFrom.x, adjFrom.y, adjFrom.z);

								ppp = performRotations(e.xRot, e.yRot, vec3(adjTo.x, adjTo.y, adjTo.z));
								ppn = performRotations(e.xRot, e.yRot, vec3(adjTo.x, adjTo.y, adjFrom.z));
								pnp = performRotations(e.xRot, e.yRot, vec3(adjTo.x, adjFrom.y, adjTo.z));
								pnn = performRotations(e.xRot, e.yRot, vec3(adjTo.x, adjFrom.y, adjFrom.z));
								npp = performRotations(e.xRot, e.yRot, vec3(adjFrom.x, adjTo.y, adjTo.z));
								npn = performRotations(e.xRot, e.yRot, vec3(adjFrom.x, adjTo.y, adjFrom.z));
								nnp = performRotations(e.xRot, e.yRot, vec3(adjFrom.x, adjFrom.y, adjTo.z));
								nnn = performRotations(e.xRot, e.yRot, vec3(adjFrom.x, adjFrom.y, adjFrom.z));

								/*ppp = vec3(adjTo.x, adjTo.y, adjTo.z);
								ppn = vec3(adjTo.x, adjTo.y, adjFrom.z);
								pnp = vec3(adjTo.x, adjFrom.y, adjTo.z);
								pnn = vec3(adjTo.x, adjFrom.y, adjFrom.z);
								npp = vec3(adjFrom.x, adjTo.y, adjTo.z);
								npn = vec3(adjFrom.x, adjTo.y, adjFrom.z);
								nnp = vec3(adjFrom.x, adjFrom.y, adjTo.z);
								nnn = vec3(adjFrom.x, adjFrom.y, adjFrom.z);*/

								/*mat4 rm = mat4(1.0f);//rm stands for rotation matrix

								rm = rotate(rm, (float)radians((float)e.yRot), vec3(0, 1, 0));
								rm = rotate(rm, (float)radians((float)e.xRot), vec3(-1, 0, 0));

								ppp = rotateAroundCenter(rm, vec4(adjust(e.to.x, e.to.y, e.to.z), 0.0f));
								ppn = rotateAroundCenter(rm, vec4(adjust(e.to.x, e.to.y, e.from.z), 0.0f));
								pnp = rotateAroundCenter(rm, vec4(adjust(e.to.x, e.from.y, e.to.z), 0.0f));
								pnn = rotateAroundCenter(rm, vec4(adjust(e.to.x, e.from.y, e.from.z), 0.0f));
								npp = rotateAroundCenter(rm, vec4(adjust(e.from.x, e.to.y, e.to.z), 0.0f));
								npn = rotateAroundCenter(rm, vec4(adjust(e.from.x, e.to.y, e.from.z), 0.0f));
								nnp = rotateAroundCenter(rm, vec4(adjust(e.from.x, e.from.y, e.to.z), 0.0f));
								nnn = rotateAroundCenter(rm, vec4(adjust(e.from.x, e.from.y, e.from.z), 0.0f));*/

								

								//printf("from: %i, %i, %i\n", e.from.x, e.from.y, e.from.z);
								//printf("to: %i, %i, %i\n", e.to.x, e.to.y, e.to.z);

								//printf("block coord in verticizing: %i, %i, %i\n", block.coords.x, block.coords.y, block.coords.z);
								//printf("ppp: %f, %f, %f\n", ppp.x, ppp.y, ppp.z);
								//printf("nnn %f, %f, %f\n", nnn.x, nnn.y, nnn.z);
								//printf("bout to crash\n");
								//vec2 tintUV = vec2(0.1, 0.1);
								vec2 tintUV = getBiomeAttrs(biomes, block.coords);
								//printf("didn't crash\n");

								

								if (block.faces & 0b00100000 && !(e.up.cullFace & 0b11000000))//+y
								{
									//printf("adding top\n");
									addFace(this->verts, npn, npp, ppp, ppn, e.up.uv, e.up.rotation, e.yRot, e.uvLock, e.up.texture, e.up.tintIndex, tintUV, block.coords);
								}
								if (block.faces & 0b00010000 && !(e.down.cullFace & 0b11000000))//-y
								{
									//printf("adding bot\n");
									addFace(this->verts, nnp, nnn, pnn, pnp, e.down.uv, e.down.rotation, e.yRot, e.uvLock, e.down.texture, e.down.tintIndex, tintUV, block.coords);
								}
								if (block.faces & 0b00001000 && !(e.east.cullFace & 0b11000000))//+x
								{
									//printf("adding west\n");
									addFace(this->verts, ppp, pnp, pnn, ppn, e.east.uv, e.east.rotation, e.yRot % 180 == 90 ? 0 : e.xRot, e.uvLock, e.east.texture, e.east.tintIndex, tintUV, block.coords);
								}
								if (block.faces & 0b00000100 && !(e.west.cullFace & 0b11000000))//-x
								{
									//printf("adding east\n");
									addFace(this->verts, npn, nnn, nnp, npp, e.west.uv, e.west.rotation, e.yRot % 180 == 90 ? 0 : e.xRot, e.uvLock, e.west.texture, e.west.tintIndex, tintUV, block.coords);
								}
								if (block.faces & 0b00000010 && !(e.south.cullFace & 0b11000000))//+z
								{
									//printf("adding south\n");
									addFace(this->verts, npp, nnp, pnp, ppp, e.south.uv, e.south.rotation, e.yRot % 180 == 0 ? 0 : e.xRot, e.uvLock, e.south.texture, e.south.tintIndex, tintUV, block.coords);
								}
								if (block.faces & 0b00000001 && !(e.north.cullFace & 0b11000000))//-z
								{
									//printf("adding north\n");
									addFace(this->verts, ppn, pnn, nnn, npn, e.north.uv, e.north.rotation, e.yRot % 180 == 0 ? 0 : e.xRot, e.uvLock, e.north.texture, e.north.tintIndex, tintUV, block.coords);
								}
							}
						}
					}
				}
			}

			//chrono::milliseconds end = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
			//long duration = end.count() - start.count();
			//printf("that section took %lims\n", duration);
		}
	}
}


bool Chunk::cullForThisBlock(ivec3 coord, const array<Section*, 20>& sections)
{
	if (coord.x >= 0 && coord.x <= 15 && coord.z >= 0 && coord.z <= 15 && coord.y >= 0)//if it's still in the chunk
	{
		//printf("chk exists\n");
		int sec = coord.y >> 4;
		if (sections[sec] != NULL)
		{
			//printf("sec exists\n");
			//printf("accesing %i, %i, %i\n", coord.y % 16, coord.z, coord.x);
			bool cfm = sections[sec]->blocks[coord.y % 16][coord.z][coord.x].cullForMe;

			//printf("cfm: %s\n", cfm ? "true" : "false");
			return cfm;

		}
	}
	return false; //nope
}

uint8_t Chunk::getSides(ivec3 chkRelativeCoord, const array<Section*, 20>& sections)
{

	ivec3 top = chkRelativeCoord + ivec3(0, 1, 0);
	ivec3 bot = chkRelativeCoord + ivec3(0, -1, 0);
	ivec3 left = chkRelativeCoord + ivec3(-1, 0, 0);
	ivec3 right = chkRelativeCoord + ivec3(1, 0, 0);
	ivec3 back = chkRelativeCoord + ivec3(0, 0, 1);
	ivec3 front = chkRelativeCoord + ivec3(0, 0, -1);

	uint8_t toReturn = 0b00000000;
	if (!cullForThisBlock(top, sections))
	{
		toReturn |= 0b00100000;
	}
	if (!cullForThisBlock(bot, sections))
	{
		//printf("not culling bot");
		toReturn |= 0b00010000;
	}
	if (!cullForThisBlock(left, sections))
	{
		//printf("not culling left");
		toReturn |= 0b00001000;
	}
	if (!cullForThisBlock(right, sections))
	{
		//printf("not culling right");
		toReturn |= 0b00000100;
	}
	if (!cullForThisBlock(back, sections))
	{
		//printf("not culling back");
		toReturn |= 0b00000010;
	}
	if (!cullForThisBlock(front, sections))
	{
		//printf("not culling front");
		toReturn |= 0b00000001;
	}
	return toReturn;
}


//sets the face byte for each block, determining which faces should be translated to triangles  
void Chunk::cullChunk(const array<Section*, 20>& sections)
{
	for (int s = 0; s < sections.size(); s++)
	{
		Section* section = sections[s];
		if (section != NULL)
		{
			for (int y = 0; y < 16; y++) //for each y latyer
			{
				for (int x = 0; x < 16; x++)
				{
					for (int z = 0; z < 16; z++)
					{
						if (section->y > 4)
						{
							//printf("there are %i sections", sections.size());
							//printf("index of %i\n", s);
							//printf("accessing %i,%i,%i\n", y, z, x);
							//printf("section %i\n", section->y);
							//printf("section %s\n", section->blocks[y][z][x].model.c_str());
						}
						string name = section->blocks[y][z][x].model;
						//printf("name: %s\n", name.c_str());
						if (name == "block/air" || name == "block/void_air" || name == "block/cave_air")
						{
							section->blocks[y][z][x].model = "NULL";
						}
						else
						{
							//printf("culling %s\n", name.c_str());
							ivec3 chkRelativeCoords = ivec3(x, y, z);
							chkRelativeCoords.y += (section->y * 16);

							//Model curr = worldChunks[{chunk.first.first, chunk.first.second}].sections.at(section.first).blocks[y][z][x]; //lol
							//ivec3 global = ivec3(x, y, z) + ivec3(chk->x * 16, section.second->y * 16, chk->z * 16);
							uint8_t sides = getSides(chkRelativeCoords, sections);
							//printf("at coords %i%, %i, %i\n", global.x, global.y, global.z);
							//printf("sides: %c%c%c%c%c%c%c%c\n", BTB(sides));
							if (sides > 0)
							{
								//printf("it has sides\n");
								//section.second->blocks[y][z][x].coords = ivec3(x, y, z) + ivec3(chk->x * 16, section.second->y * 16, chk->z * 16); //sets the coordinates in the global scale
								section->blocks[y][z][x].faces = sides;
							}
							else
							{
								//printf("it has no sides\n");
								section->blocks[y][z][x].model = "NULL";
							}
						}
					}
				}
			}
		}
	}
}





//generates a 64 bit bit mask with <bits> 1s starting from the least most bit
//so if you input 4, the output would be 000000...000000001111
size_t generateMask(const size_t& bits)
{
	size_t toReturn = 0;
	for (int i = 0; i < bits; i++)
	{
		toReturn++;
		toReturn = toReturn << 1;
	}
	toReturn = toReturn >> 1;
	return toReturn;
}


//a function to get a specific block's palette index based on the given
//bitWidth. There's definetly a more efficient way of doing this if it were to be done
//one block after another, but like...it works for now. so i'll put this here 
//todo: make it better ..?
size_t getPaletteID(const vector<int64_t>& blockStates, const size_t& blockIndex, const size_t& bitWidth)
{
	size_t bitIndex = bitWidth * blockIndex;
	size_t firstLongIndex = bitIndex / 64;
	size_t secondLongIndex = (bitIndex + (bitWidth - 1)) / 64;
	if (firstLongIndex == secondLongIndex)//it's all in one long
	{
		size_t toReturn = (blockStates[firstLongIndex] >> bitIndex % 64) & generateMask(bitWidth);
		return toReturn;
	}
	else //it's in two longs
	{
		size_t bitWidthOfFirstHalf = 0;
		while ((bitIndex / 64) < secondLongIndex)
		{
			bitWidthOfFirstHalf++;
			bitIndex++;
		}
		size_t firstHalfValue = (blockStates[firstLongIndex] >> (64 - bitWidthOfFirstHalf)) & generateMask(bitWidthOfFirstHalf);
		size_t secondHalfValue = (blockStates[secondLongIndex] & generateMask(bitWidth - bitWidthOfFirstHalf)) << bitWidthOfFirstHalf;
		size_t toReturn = firstHalfValue | secondHalfValue;
		return toReturn;
	}
}








void Chunk::createChunk(CompoundTag* root, Asset* ass, array<Section*, 20>& thisSections, array<array<array<uint32_t, 64>, 4>, 4>& biomes)
{
	size_t initZero = 0;//IMPORTANT
	//printf("NBT1 Parsed\n");
	CompoundTag* level = root->getTag("Level")->toCT();


	//read in biome information
	



	if (level->getValues().count("Biomes") > 0)
	{
		//printf("it has biomes\n");
		vector<int32_t> biomeVector = level->getTag("Biomes")->toTagArray<int32_t>()->getValues();
		size_t i = 0;
		for (size_t z = 0; z < 4; z++)
		{
			for (size_t x = 0; x < 4; x++)
			{
				for (size_t y = 0; y < 64; y++)
				{
					if (biomeVector[i] > 255)
					{
						printf("x,y,z %i,%i,%i is biome %i\n", x, y, z, biomeVector[i]);
					}
					biomes[z][x][y] = biomeVector[i++];
				}
			}
		}
	}
	else
	{
		//printf("biomes does not exist\n");
		size_t i = 0;
		for (size_t z = 0; z < 4; z++)
		{
			for (size_t x = 0; x < 4; x++)
			{
				for (size_t y = 0; y < 64; y++)
				{
					biomes[z][x][y] = 0;
				}
			}
		}
	}

	TagList* sections = level->getTag("Sections")->toList();

	thisSections.fill(0);
	//printf("There are %i sections\n", sections->getValues().size());
	int a = 0;
	for (auto sec : sections->getValues())//for each section
	{
		CompoundTag* section = sec->toCT();

		if (section->values.count("BlockStates") > 0)//if it is a real section
		{
			//printf("real section %i\n", a++);
			Section* toAdd = new Section();
			toAdd->y = section->getTag("Y")->toTag<int8_t>()->getValue();
			//printf("y is %i\n", section->getTag("Y")->toTag<int8_t>()->getValue());
			TagArray<int64_t>* blockStates = section->getTag("BlockStates")->toTagArray<int64_t>();
			TagList* p = section->getTag("Palette")->toList();

			for (auto pc : p->getValues())//for each item in the palette
			{
				CompoundTag* paletteCompound = pc->toCT();
				string name;
				unordered_map<string, string> attributes;
				for (const auto& [tagName, tag] : paletteCompound->getValues())//for each element of the item
				{
					if (tagName == "Properties")
					{
						//printf("----\n");
						CompoundTag* properties = tag->toCT();
						for (const auto& [subTagName, subTag] : properties->getValues())//for each property
						{
							//printf("subtagName/subtag %s, %s\n", subTagName.c_str(), subTag->toTag<string>()->getValue().c_str());
							attributes[subTagName] = subTag->toTag<string>()->getValue();
						}
					}
					if (tagName == "Name")
					{
						name = tag->toTag<string>()->getValue();
						//printf("name: %s\n", name.c_str());
					}
				}
				toAdd->palette.push_back(ass->findModelFromAssets(name, attributes));
			}
			//printf("blockstates: %i\n", blockStates->getValues().size());

			size_t bitWidth = blockStates->getValues().size() / 64;//number of longs/64 is how many bits each one takes

			bitWidth = 4; //TODO: REMOVE/FIX

			//printf("bitWidth: %lu\n", bitWidth);
			for (size_t y = 0; y < 16; y++)
			{
				for (size_t z = 0; z < 16; z++)
				{
					for (size_t x = 0; x < 16; x++)
					{
						size_t blockIndex = (y * 256) + (z * 16) + x;
						int palleteID = getPaletteID(blockStates->getValues(), blockIndex, bitWidth);
						toAdd->blocks[y][z][x] = toAdd->palette[palleteID];
						toAdd-> blocks[y][z][x].coords = ivec3(x, y, z) + ivec3(chkx * 16, toAdd->y * 16, chkz * 16);

					}
				}
			}
			//printf("adding section at y %i\n", toAdd -> y);
			thisSections[toAdd->y] = toAdd;
		}
	}

	/*for (int i = 0; i < thisSections.size(); i++)
	{
		printf("section %i is null: %s\n", i, thisSections[i] == NULL ? "true" : "false");
	}*/


}



CompoundTag* decompress(string saveFolder, int chkx, int chkz)
{
	int regx;
	int regz;
	if (chkx >= 0)
	{
		regx = chkx / 32;
	}
	else
	{
		regx = ((chkx + 1) / 32) - 1;
	}
	if (chkz >= 0)
	{
		regz = chkz / 32;
	}
	else
	{
		regz = ((chkz + 1) / 32) - 1;
	}

	string filePath = saveFolder + "/r." + to_string(regx) + "." + to_string(regz) + ".mca";
	
	//printf("Save path: %s\n", filePath.c_str());

	basic_ifstream<unsigned char> file;
	try
	{
		file.exceptions(ios::failbit | ios::badbit);
		file.open(filePath, ios_base::in | ios_base::binary);
	}
	catch (exception e)
	{
		return NULL;
	}

	//get the size of the file
	file.seekg(0, ios::end);
	unsigned int length = file.tellg();
	file.seekg(ios::beg);

	unsigned char* buffer = new unsigned char[length]; //TODO unallocate this memory 
	file.read(buffer, length);

	int ol = 4 * ((chkx & 31) + (chkz & 31) * 32);

	int offset = (buffer[ol] << 16 | buffer[ol + 1] << 8 | buffer[ol + 2]); //ignore this warning, it's a safe read
	int sectorCount = buffer[ol + 3];

	if (offset != 0)//if the chunk exsists
	{
		int sos = offset * 4096;
		int sectorLength = (buffer[sos + 0] << 24 | buffer[sos + 1] << 16 | buffer[sos + 2] << 8 | buffer[sos + 3]);
		int compressionType = buffer[sos + 4];//if i cared about older versions of minecraft world this would matter = false
		if (compressionType > 2)//check for chunk that is waaaay too big and is in a whole separate file. We're not handling those yet :/ //TODO
		{
			printf("##########DECOMPRESSION WARNING##############\n");
			exit(-1);
		}
		sos += 5;//this is a magical number, and i don't remember exactly what it means. I think it's the offset into the offset section of the file?

		vector<unsigned char> chunkData;
		int status = Deflate(&buffer[sos], sectorLength, [&](unsigned char c) {chunkData.push_back(c); });//decompress
		size_t initZero = 0;
		CompoundTag* chunkNBT = parseNBT(chunkData, &initZero);//convert decompressed byte data to NBT format

		return chunkNBT;
	}
	return NULL;
}


//TODO: should make it use the EBO, even if it's just for the coords that each cubeoid shares, not between cuboids
//shouldn't be too hard...lol
void Chunk::initializeBuffers()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	GLsizei stride = sizeof(Vert);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0); //position
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float))); //uv
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float))); //tint uv
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, (void*)(7 * sizeof(float))); //tex layer
	glEnableVertexAttribArray(3);
}

void Chunk::bufferData()
{

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, this->verts.size()*sizeof(Vert), this->verts.data(), GL_STATIC_DRAW);


	//this->verts.clear();
}

void Chunk::draw()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glDrawArrays(GL_TRIANGLES, 0, this->verts.size());
}