#include "Asset.hpp"
#include <filesystem>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "json.hpp"


//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

using namespace std;
using namespace std::filesystem;
using namespace nlohmann;

//parses a list of string:string attributes from a json string in the format
//atr1=atr1,atr2=atr2 ect
unordered_map < string, string > Asset::parseAttributes(string src)
{
	unordered_map < string, string > toReturn;
	vector<string> bothHalves;
	string str;
	istringstream stri(src);

	while (getline(stri, str, ','))
	{
		bothHalves.push_back(str);
	}
	for (string wholes : bothHalves)
	{
		istringstream wholeStream(wholes);
		string firstHalf;
		string secondHalf;
		getline(wholeStream, firstHalf, '=');
		getline(wholeStream, secondHalf, '=');
		toReturn[firstHalf] = secondHalf;
	}
	return toReturn;
}

json Asset::fileToJson(string filepath)
{
	//printf("pasring file %s\n", filepath.c_str());
	ifstream file(filepath);
	json j;
	file >> j;
	//printf("parsed\n");
	return j;
}

void combineModels(Model& keeping, const Model& discarding)
{
	//so you'll notice that this doesn't take the name of the models
	//into acount, and that is because at this point the names aren't used.
	keeping.elements.insert(keeping.elements.end(), discarding.elements.begin(), discarding.elements.end());
	if (discarding.cullForMe)
	{
		keeping.cullForMe = true;
	}
	if (discarding.AmbOcc)
	{
		keeping.AmbOcc = true;
	}
}
//Model combineModels(const Model& keeping, const Model& discarding)
//{
//	//so you'll notice that this doesn't take the name of the models
//	//into acount, and that is because at this point the names aren't used.
//	Model m;
//	m.elements.insert(m.elements.end(), keeping.elements.begin(), keeping.elements.end());
//	m.elements.insert(m.elements.end(), discarding.elements.begin(), discarding.elements.end());
//	m.model = keeping.model;
//	if (keeping.cullForMe || discarding.cullForMe)
//	{
//		m.cullForMe = true;
//	}
//	if (keeping.AmbOcc || discarding.AmbOcc)
//	{
//		m.cullForMe = true;
//	}
//	return m;
//}

Model Asset::findModelFromAssets(string name, const unordered_map<string, string>& attributes)
{

	//if (name != "minecraft:grass_block" && name != "minecraft:bedrock" && name != "minecraft:dirt" && name != "minecraft:air")
	if (name == "minecraft:redstone_wall_torch")
	{

		Model m;
		name = name.substr(name.find(":") + 1);
		BlockState bs = assets[name];
		printf("------------------------\nfindModelFromAssets of name: %s\nattributes: \n", name.c_str());
		for (pair<string, string> val : attributes)
		{
			printf("%s=%s\n", val.first.c_str(), val.second.c_str());
		}
		printf("\n");
		//god this naming scheme was a mistake
		//printf("it has at least one variant: %s\n", bs.variants.empty() ? "false" : "true");
		for (Conditional variant : bs.variants) //for each variant
		{
			printf("testing for %s\n", variant.model.model.c_str());
			if (variant.when.size() == 0)
			{
				if (m.model == "NULL")
				{
					printf("first add\n");
					m = variant.model;
				}
				else
				{
					printf("not first add\n");
					combineModels(m, variant.model);
				}
			}
			for (Conditions variantConditions : variant.when) //for each possible set of conditions
			{

				printf("\n---if list: \n");
				//bool added = false;
				bool match = true;



				for (pair<string, string> condition : variantConditions.conditions) //for each condition
				{
					printf("%s=%s\n", condition.first.c_str(), condition.second.c_str());
					if (attributes.count(condition.first) > 0)//if it has the attribute
					{
						printf("it has the attribute\n");
						if (attributes.at(condition.first) == condition.second) //if the attribute matches
						{
							//printf("the attribute values match\n");
							//gucci
						}
						else
						{
							match = false;
						}

					}
					else
					{
						match = false;
					}
				}
				if (match)
				{
					printf("matches\n");
					if (m.model == "NULL")
					{
						printf("first add pt 2\n");
						m = variant.model;
						//printf("x %i, y %i\n", variant.model.elements[0].xRot, variant.model.elements[0].yRot);
					}
					else
					{
						printf("not first add pt2\n");
						combineModels(m, variant.model);
					}
				}
			}

		}
		Element e = m.elements[0];
		//printf("size: %i\n", m.elements.size());
		//printf("x %i, y %i\n", e.xRot, e.yRot);
		return m;
	}
	else
	{
		Model m;
		name = name.substr(name.find(":") + 1);
		BlockState bs = assets[name];
		for (Conditional variant : bs.variants) //for each variant
		{
			if (variant.when.size() == 0)
			{
				if (m.model == "NULL")
				{
					m = variant.model;
				}
				else
				{
					combineModels(m, variant.model);
				}
			}
			for (Conditions variantConditions : variant.when) //for each possible set of conditions
			{

				bool match = true;
				for (pair<string, string> condition : variantConditions.conditions) //for each condition
				{
					if (attributes.count(condition.first) > 0)//if it has the attribute
					{
						if (attributes.at(condition.first) == condition.second) //if the attribute matches
						{
							//gucci
						}
						else
						{
							match = false;
						}

					}
					else
					{
						match = false;
					}
				}
				if (match)
				{
					if (m.model == "NULL")
					{
						m = variant.model;
					}
					else
					{
						combineModels(m, variant.model);
					}
				}
			}

		}
		Element e = m.elements[0];
		return m;
	}
}
//the parses the information from the <faces> 

int translateCullFace(const string& face)//null/null/+y/-y/+x/-x/+z/-z
{
	if (face == "up")
	{
		return 0b00100000;
	}
	if (face == "down" || face == "bottom")
	{
		return 0b00010000;
	}
	if (face == "north")
	{
		return 0b00000001;
	}
	if (face == "south")
	{
		return 0b00000010;
	}
	if (face == "east")
	{
		return 0b00001000;
	}
	if (face == "west")
	{
		return 0b00000100;
	}
	fprintf(stderr, "error in translating cull face\n");
	fprintf(stderr, "%s\n", face.c_str());
	exit(-1);
}

Face Asset::parseFaceJson(const json& faces, const string& faceStr, const unordered_map<string, string>& textures)
{
	//printf("parsing face %s\n", faceStr.c_str());
	Face toReturn;
	if (!faces.contains(faceStr))
	{
		toReturn.cullFace = 0b11000000;
		return toReturn;
	}
	if (faces[faceStr].contains("uv"))
	{
		json uv = faces[faceStr]["uv"];
		toReturn.uv = vec4(stoi(uv[0].dump()), stoi(uv[1].dump()), stoi(uv[2].dump()), stoi(uv[3].dump()));//returns the uv in the order x1, y1, x2, y2
		//toReturn.uv = vec4(stoi(uv[0].dump()), stoi(uv[1].dump()), stoi(uv[2].dump()), stoi(uv[3].dump()));//returns the uv in the order x1, y1, x2, y2
	}
	//if (faces[faceStr].contains("tintindex"))
	//{
	//	toReturn.tintIndex = faces[faceStr].at("tintIndex");
	//}

	string referenceName = string(faces[faceStr]["texture"]).substr(1);
	string fullName = textures.at(referenceName);
	string cutName = fullName.substr(fullName.find("/") + 1);


	toReturn.texture = textureMap[cutName];
	//printf("texture %s = %i\n", cutName.c_str(), toReturn.texture);

	if (faces[faceStr].contains("rotation"))
	{
		toReturn.rotation = faces[faceStr]["rotation"];
	}

	if (faces[faceStr].contains("cullface"))
	{
		toReturn.cullFace = translateCullFace(faces[faceStr]["cullface"]);
	}
	else
	{
		toReturn.cullFace = translateCullFace(faceStr);
	}

	return toReturn;
}

//parses the model information from the model.json
//this includes all the parents' information
Model Asset::parseModelJson(string name, int xRot, int yRot, int uvLock)
{

	unordered_map<string, string> textures;//a map of texture names for when they are references (#name)

	//json modelJson = fileToJson(MODEL_DIR_PATH + name + ".json");//am i just a dumbass? this was commented out
	string parent = name;
	size_t layer = 0;
	bool hasParent = true;
	json modelJson;
	bool elementRead = false;//only the bottom most elements get read, parents get ignored
	Model m;
	m.model = name;
	do
	{
		modelJson = fileToJson(MODEL_DIR_PATH + parent + ".json");
		if (parent == "block/cube" || m.model == "block/grass_block")
		{
			m.cullForMe = true;
		}
		if (modelJson.contains("ambientocclusion"))
		{
			m.AmbOcc = modelJson["ambientocclusion"];
		}
		if (modelJson.contains("textures"))
		{
			for (auto& [key, value] : modelJson["textures"].items())//for each texture
			{
				string texName(value);
				if (texName.c_str()[0] == '#')
				{
					texName = texName.substr(1);
					textures[key] = textures[texName];
				}
				else
				{
					textures[key] = string(value);
				}
			}
		}
		if (modelJson.contains("elements") && elementRead == false)
		{
			elementRead = true;
			for (auto& [key, value] : modelJson["elements"].items())//for each element
			{
				Element e;
				e.xRot = xRot;
				e.yRot = yRot;
				e.uvLock = uvLock;

				json from = value["from"];
				e.from = vec3(stoi(from[0].dump()), stoi(from[1].dump()), stoi(from[2].dump()));


				json to = value["to"];
				e.to = vec3(stoi(to[0].dump()), stoi(to[1].dump()), stoi(to[2].dump()));


				json faces = value["faces"];

				e.down = parseFaceJson(faces, "down", textures);
				e.up = parseFaceJson(faces, "up", textures);
				e.south = parseFaceJson(faces, "south", textures);
				e.north = parseFaceJson(faces, "north", textures);
				e.east = parseFaceJson(faces, "east", textures);
				e.west = parseFaceJson(faces, "west", textures);
				m.elements.push_back(e);
			}
		}
		if (modelJson.contains("parent"))
		{
			parent = string(modelJson["parent"]);
			hasParent = true;
		}
		else
		{
			hasParent = false;
		}
	} while (hasParent);
	return m;
}


//parses the model's information from the blockstate.json
Model Asset::parseModel(const json& j)
{
	Model toReturn;

	if (j.is_array())
	{
		toReturn = parseModel(j.front());
	}
	else
	{
		int xRot = 0;
		int yRot = 0;
		int uvLock = 0;
		if (j.contains("x"))
		{
			xRot = j.find("x").value();
		}

		if (j.contains("y"))
		{
			yRot = j.find("y").value();
		}

		if (j.contains("uvlock"))
		{
			uvLock = j.find("uvlock").value();
		}

		string name = j.find("model").value();
		toReturn = parseModelJson(name, xRot, yRot, uvLock);
	}

	return toReturn;
}


//parses a blockstate from both the blockstate.json and model.json
BlockState Asset::parseBlockstateJson(string filepath)
{
	ifstream file(filepath);
	json blockstateJson;
	file >> blockstateJson;  //file into json object

	BlockState toAdd;
	if (blockstateJson.contains("variants"))
	{
		toAdd.type = VARIANT;
		for (auto var : blockstateJson["variants"].items())//for each variant of blockstate
		{
			Conditional onlyCondition;
			Conditions attributes;
			attributes.conditions = parseAttributes(var.key());
			onlyCondition.model = parseModel(var.value());
			onlyCondition.when.push_back(attributes);
			//if (var.key() == "facing=south,half=bottom,shape=straight")
			//{
			//	printf("x %i, y %i\n", onlyCondition.model.elements[0].xRot, onlyCondition.model.elements[0].yRot);
			//}
			toAdd.variants.push_back(onlyCondition);
		}
	}
	else if (blockstateJson.contains("multipart"))
	{
		toAdd.type = MULTIPART;

		for (auto parts : blockstateJson["multipart"].items())//for each when/apply pair
		{
			Conditional toAddCondition;
			json apply = parts.value()["apply"];
			if (parts.value().contains("when"))
			{
				json when = parts.value()["when"];
				if (when.contains("OR"))
				{
					json whore = when["OR"];//because "or" is defined as "||" in iso646
					for (auto cond : whore.items())//for each set of conditions
					{
						Conditions toAddSetOfConditions;
						for (auto& [key, value] : cond.value().items())//for each condition
						{
							string keyStr(key.c_str());
							string valueStr;
							if (cond.value().find(key).value().is_boolean())
							{
								valueStr = cond.value().find(key).value() ? "true" : "false";
							}
							else
							{
								valueStr = cond.value().find(key).value();
							}

							toAddSetOfConditions.conditions[keyStr] = valueStr;
							toAddCondition.when.push_back(toAddSetOfConditions);
						}
					}
				}
				else
				{
					Conditions toAddSetOfConditions;
					for (auto& [key, value] : when.items())//for each condition
					{
						string keyStr(key.c_str());

						string valueStr;
						if (when.find(key).value().is_boolean())
						{
							valueStr = when.find(key).value() ? "true" : "false";
						}
						else
						{
							valueStr = when.find(key).value();
						}
						toAddSetOfConditions.conditions[keyStr] = valueStr;
						toAddCondition.when.push_back(toAddSetOfConditions);
					}
				}
			}
			toAddCondition.model = parseModel(apply);
			toAdd.variants.push_back(toAddCondition);
		}
	}
	else
	{
		fprintf(stderr, "bad json blockstate, not variant or multipart\n");
		exit(-1);
	}
	return toAdd;
}

pair<long long int, long long int> addModelToVBO(directory_entry modelFile)
{

}

Asset::Asset(const unordered_map<string, int>& tm)
{
	printf("starting to create Asset\n");
	textureMap = tm;

	printf("loading blockstate jsons\n");
	directory_iterator blockstateDir(BLOCKSTATE_DIR_PATH);
	for (auto blockstateFile : blockstateDir)//for each blockstate
	{
		//if (blockstateFile.path().stem().u8string() == "black_glazed_terracotta")
		//{
			//printf("parsing: %s\n", blockstateFile.path().filename().stem().u8string().c_str());
		assets[blockstateFile.path().stem().u8string()] = parseBlockstateJson(blockstateFile.path().u8string());

		//}
	}

	printf("loaded all jsons\n");
}

Asset::~Asset()
{
}