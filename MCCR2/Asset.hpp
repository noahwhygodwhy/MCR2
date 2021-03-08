/*! \file Asset.hpp
	\brief A file that holds everything Asset related plus some structs needed eleswhere.

	\author Noah Alvard
*/

#ifndef ASSET_H
#define ASSET_H
#include <vector>
#include <string>
#include <unordered_map>
#include <string_view>
#include "json.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

using namespace nlohmann;

using namespace glm;

static const string TEXTURE_DIR_PATH = "../../MCCR2/minecraft/textures/";
static const string MODEL_DIR_PATH = "../../MCCR2/minecraft/models/";
static const string BLOCKSTATE_DIR_PATH = "../../MCCR2/minecraft/blockstates/";

/*! \struct Vert
*	\brief A structure to represent a vertex. Can be directly buffered.
*/
struct Vert
{
	
	fvec3 coord;///<The 3D coordinates of the vertex.
	fvec2 uv;///<The 2D texture coordinates of the vertex.
	fvec2 tintUV;///<The 2D texture coordinates of the vertex on the foliage texture.
	float texIndex;///<The index of the texture that the vertex should be shaded with.
	Vert(vec3 c, vec2 u, vec2 u2, float t)
	{
		coord = c;
		uv = u;
		tintUV = u2;
		texIndex = t;
	}
};


/*! \struct Vert
*	\brief A structure to represent a face of a cuboid element.
*/
struct Face
{	
	int texture = 0;///<The index of the texture that the face should be shaded with
	uint8_t cullFace = 0b00000000;///<A binary flag to indicate which face this is. null/null/+y/-y/+x/-x/+z/-z
	ivec4 uv = ivec4(0, 0, 16, 16);///<A pair of uv coordinates in the range of 0 to 16*/
	int rotation = 0;///<The UV rotation of the face
	int tintIndex = -1;///<An int representing the tint index. -1 means it should not be tinted, Anything else represents it should be tinted. The exact number representing what type of tint.
};


/*! \struct Element
*	\brief A structure to represent a cuboid element.
* 
*/
struct Element
{
	Face down;///<The down face of the cuboid
	Face up;///<The up face of the cuboid
	Face north;///<The north face of the cuboid
	Face south;///<The south face of the cuboid
	Face west;///<The west face of the cuboid
	Face east;///<The east face of the cuboid
	ivec3 from = ivec3(0, 0, 0);///<The larger coordinate'd corner of the cuboid
	ivec3 to = ivec3(16, 16, 16);///<The smaller coordinate'd corner of the cuboid
	int xRot = 0;///<The X axis rotation of the coboid in degrees in increments of 90
	int yRot = 0;///<The Y axis rotation of the coboid in degrees in increments of 90
	bool uvLock = false;///<A boolean indication of whether textures should rotate with the block or not.
};

/*! \struct Model
*	\brief A structure to represent a block in minecraft.
*
*/
struct Model
{

	ivec3 coords = vec3(0, 0, 0);///<The 3D coordinates of the block in the minecraft world.
	vector<Element> elements;///<A list of cuboid elements that make up the block.
	string model = "NULL";///<The name of the block.
	uint8_t faces = 0b00111111;///<A binary flag to indicate which faces should be rendered. Used in culling. null/null/+y/-y/+x/-x/+z/-z
	//todo: int weight = 1;?
	bool AmbOcc = true;///<Flag indicating whether this block should be rendered with ambient occlusion
	bool cullForMe = false;///<Flag indicating whether or not this block obscures adjacent blocks.
	//todo: particle?
};



/*! \enum BlockStateType
*	\brief An enum for indicating whether a blockstate has multiple parts or has variants.
*
*/
enum BlockStateType
{
	VARIANT,
	MULTIPART
};



struct Conditions
{
	unordered_map<string, string> conditions;
};
struct Conditional
{
	vector<Conditions> when;//OR'd
	Model model;//"apply"
};
struct BlockState
{
	BlockStateType type;
	vector<Conditional> variants;
};

class Asset
{
	unsigned int largeTextureStack;
	unordered_map<string, int> textureMap;
	unordered_map<string, Model> models;
	unordered_map<string, BlockState> assets;
public:
	Asset(const unordered_map<string, int>& tm);
	~Asset();
	Model findModelFromAssets(string mame, const unordered_map<string, string>& attributes);

private:
	json fileToJson(string filepath);
	Face parseFaceJson(const json& faces, const string& faceStr, const unordered_map<string, string>& textures);
	Model parseModelJson(string name, int xRot, int yRot, int uvLock);
	Model parseModel(const json& j);
	BlockState parseBlockstateJson(string filepath);
	unordered_map < string, string > parseAttributes(string src);
	//unsigned int importTextures(const string& path);


};

//class Attribute
//{
//public:
//	Attribute();
//	~Attribute();
//
//private:
//
//};
//
//Attribute::Attribute()
//{
//}
//
//Attribute::~Attribute()
//{
//}

#endif