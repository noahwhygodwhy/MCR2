/*! \file Asset.hpp
	\brief Contains the Asset class plus some structs.

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



/*! \struct Conditions
*	\brief A struct to map blockstate names to block conditions. It doens't *need* to be in a struct, but it helps keep it straight in my mind.
*/
struct Conditions
{
	unordered_map<string, string> conditions;///<The mapping of conditions to blockstates.
};

/*! \struct Conditional
*	\brief A struct to represent a conditional model. If any of the conditions in when are met, then model is the Model to be used.
*/
struct Conditional
{
	vector<Conditions> when;///<A set of conditions, any of which if true mean that the model should be used.
	Model model;///<The model to use if any of the when conditions are met.
};


/*! \struct BlockState
*	\brief A struct to represent a blockstate. If type is MULTIPART, all Models for each conditional are used. Otherwise only the models who has a conditional that is true are used.
*
*/
struct BlockState
{
	BlockStateType type;///<The type of the blockstate. Whether it has multiple models, or one of many models.
	vector<Conditional> variants;///<The list of possible models to be used.
};


/*! \class Asset
*	\brief A singleton style class that exists to parse a bunch of json files and provide the information for chunk creation.
* 
*	A singleton style class that exists to parse a bunch of json files and provide the information for chunk creation.
*	Provides 3 primary maps:
*	A map between minecraft textures, and their index in the "Large Texture Stack"
*	A map between model names, and the parsed json for each respective model.
*	A map between blockstate names, and the parsed json for each respective blockstate.
* 
*  \sa Renderer#loadTextures()
*/
class Asset
{
	//unsigned int largeTextureStack;
	unordered_map<string, int> textureMap; ///<A mapping of texture names to the index of said texture in the Large Texture Stack
	unordered_map<string, Model> models; ///<A mapping of model names to the parsed json data for said model.
	unordered_map<string, BlockState> assets; ///<A mapping of blockstate names to the parsed json dat for said blockstate.
public:
	/*! Asset Constructor
	* Iterates through the blockstate folder json files, parses the json files to Blockstates, and maps them to the name
	* of the BlockState. In this proces, it also creates a mapping of Model names to Models in order to prevent re-doing work.
	* Asset constructor requires the texture map be provided as the Renderer is the one to create the Large Texture Stack that coresponds to the texture map.
	* \param tm Maps minecraft texture names to the index of that texture in the Large Texture Stack.
	*/
	Asset(const unordered_map<string, int>& textureMap);
	~Asset();
	/*! \fn findModelFromAssets
	*	Takes a model name and a list of attributes and selects the correct model. This is needed as minecraft has no rhyme nor reason
	*	to the way they organize the block attributes. The block attributes are parsed into a map, and this function figures out which
	*	of the versions of the model has the attributes.
	*	\param name The name of the model
	*	\param attributes A map of attributeName:attributeStatus
	*	\returns A Model that corresponds to both the name and the attributes
	*/
	Model findModelFromAssets(string name, const unordered_map<string, string>& attributes);

private:
	/*! \fn fileToJson
	* Takes a string and parses it to a json object.
	* \param filepath The complete path to the file to be read.
	* \returns A parsed version of the file in the form of a json object.
	* \sa json.hpp
	*/
	json fileToJson(string filepath);
	/*! \fn parseFaceJson
	* Parses one of the "faces" sections of a minecraft model json file, and returns a Face initialized with the data from the json.
	* \param faces The json object containing the information about the faces of the model.
	* \param faceStr A compass direction (or "up" or "down") indicating which of the 6 faces to parse into a Face.
	* \param textures A mapping between arbitrary texture labels from the json to the names (*not indidces*) of real textures.
	* \returns A Face with all the information from the json.
	* 
	*/
	Face parseFaceJson(const json& faces, const string& faceStr, const unordered_map<string, string>& textures);
	/*! \fn parseFaceJson
	* Parses the json file(s) representing a model. I would definetly recommend reading <a href="https://minecraft.gamepedia.com/Model#Block_models">https://minecraft.gamepedia.com/Model#Block_models</a>   \endlink
	* \param name The name of the model.
	* \param xRot The rotation on the x axis given by the blockstate
	* \param yRot The rotation on the y axis given by the blockstate
	* \param uvLock A boolean indicating whether or not the textures on this model are UV locked.
	* \returns A Model coresponding to the parsed json file named <name>.json
	*/
	Model parseModelJson(string name, int xRot, int yRot, bool uvLock);

	/*! \fn parseModel
	* Reads in a blockstate json file, and produces the corresponding Model. I woudl definetly recommend reading <a href="https://minecraft.gamepedia.com/Model#Block_states">https://minecraft.gamepedia.com/Model#Block_states</a>
	* \param j The parsed json object of the json pertaining to the Model in a blockstate json file.
	* \returns a Model coresponding to the given part of the blockstate json.
	*/
	Model parseModel(const json& j);
	/*! \fn parseBlockStateJson()
	* Parses all of the possible models for a given blockstate in the json file at <filepath>.
	* \param filepath The path of the blockstate's json file.
	* \returns A BlockState object coresponding to the json file.
	*/
	BlockState parseBlockstateJson(string filepath);

	/*! \fn parseAttributes
	* Minecraft gives attributes in the form attributeName=attributestatus,attributeName=attributestatus,attributeName=attributestatus,ect
	* This function takes that, and creates a mapping between each attributeName and it's corresponding attributeStatus.
	* \param src The string of attributes in the hard to read format.
	* \returns A mapping thats much more convinient to use.
	*/
	unordered_map < string, string > parseAttributes(string src);


};

#endif