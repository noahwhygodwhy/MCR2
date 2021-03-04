#include "NBTTags.hpp"

using namespace std;



int32_t nextUTF8Char(size_t* index, vector<unsigned char>& decompressedData)
{
	int32_t toReturn;
	if (decompressedData.at(*index) <= 0x7f)
	{
		toReturn = decompressedData.at(*index);
		(*index) += 1;
	}
	else if (decompressedData.at(*index) <= 0x7ff)
	{
		toReturn = (decompressedData.at(*index) - 0b11000000) << 6 || (decompressedData.at(*index + 1) - 0b10000000);
		(*index) += 2;
	}
	else if (decompressedData.at(*index) <= 0xffff)
	{
		toReturn = (decompressedData.at(*index) - 0b111000000) << 12 || (decompressedData.at(*index + 1) - 0b10000000) << 6 || decompressedData.at(*index + 2) - 0b10000000;
		(*index) += 3;
	}
	else if (decompressedData.at(*index) <= 0x10ffff)
	{
		toReturn = (decompressedData.at(*index) - 0b111100000) << 18 || (decompressedData.at(*index + 1) - 0b10000000) << 12 || (decompressedData.at(*index + 2) - 0b10000000) << 6 || decompressedData.at(*index + 3) - 0b10000000;
		(*index) += 4;
	}
	else
	{
		fprintf(stderr, "bad utf decoding\n");
		exit(-1);
	}
	return toReturn;
}

//converts a TagType to a string for print debugging
string tts(TagType t)
{
	switch (t)
	{
	case TAG_COMPOUND:
		return "COMPOUND";
	case TAG_BYTE:
		return "BYTE";
	case TAG_SHORT:
		return "SHORT";
	case TAG_INT:
		return "INT";
	case TAG_LONG:
		return "LONG";
	case TAG_FLOAT:
		return "FLOAT";
	case TAG_DOUBLE:
		return "DOUBLE";
	case TAG_BYTE_ARRAY:
		return "BYTE ARRAY";
	case TAG_STRING:
		return "STRING";
	case TAG_LIST:
		return "LIST";
	case TAG_INT_ARRAY:
		return "INT ARRAY";
	case TAG_LONG_ARRAY:
		return "LONG ARRAY";
	case TAG_END:
		return "TAG END";
	default:
		return "WEIRD TAG";
	}
}


TagList* SuperTag::toList()
{
	return dynamic_cast<TagList*>(this);
}
CompoundTag* SuperTag::toCT()
{
	return dynamic_cast<CompoundTag*>(this);
}

CompoundTag* parseNBT(vector<unsigned char>& decompressedData, size_t* index)
{
	TagType type = (TagType)decompressedData.at(*index);
	(*index) += 1;

	size_t nameLength = ((size_t)decompressedData.at(*index) << 8 | (size_t)decompressedData.at((*index + 1)));
	(*index) += 2;
	string name;
	for (size_t i = 0; i < nameLength; i++)
	{
		name += decompressedData.at(*index);
		(*index) += 1;
	}
	return parseCompoundTag(decompressedData, index, type);
	//return dynamic_cast<CompoundTag*>(parseNBT(decompressedData, index, TAG_COMPOUND));
}
SuperTag* parseNBT(vector<unsigned char>& decompressedData, size_t* index, TagType type)
{
	//TagType type = (TagType)decompressedData.at(*index);
	//(*index) += 1;

	switch (type)
	{
	case TAG_END:
		return new Tag<uint8_t>(0, TAG_END);
	case TAG_COMPOUND:
		return parseCompoundTag(decompressedData, index, type);
	case TAG_BYTE:
		return parseByteTag(decompressedData, index, type);
	case TAG_SHORT:
		return parseShortTag(decompressedData, index, type);
	case TAG_INT:
		return parseIntTag(decompressedData, index, type);
	case TAG_LONG:
		return parseLongTag(decompressedData, index, type);
	case TAG_FLOAT:
		return parseFloatTag(decompressedData, index, type);
	case TAG_DOUBLE:
		return parseDoubleTag(decompressedData, index, type);
	case TAG_BYTE_ARRAY:
		return parseByteArrayTag(decompressedData, index, type);
	case TAG_STRING:
		return parseStringTag(decompressedData, index, type);
	case TAG_LIST:
		return parseListTag(decompressedData, index, type);
	case TAG_INT_ARRAY:
		return parseIntArrayTag(decompressedData, index, type);
	case TAG_LONG_ARRAY:
		return parseLongArrayTag(decompressedData, index, type);
	default:
		fprintf(stderr, "Forgot to handle nbt tab: %i", type);
		exit(-1);
	}
}

static uint64_t layers = 0;

CompoundTag* parseCompoundTag(vector<unsigned char>& decompressedData, size_t* index, TagType type)
{
	CompoundTag* toReturn = new CompoundTag(type);
	//layers++;
	while (true)
	{
		TagType type = (TagType)decompressedData.at(*index);
		(*index) += 1;
		if (type == TAG_END)
		{
			break;
		}
		uint16_t nameLength = ((size_t)decompressedData.at(*index) << 8 | (size_t)decompressedData.at((*index + 1)));
		(*index) += 2;
		string name;
		for (size_t i = 0; i < nameLength; i++)
		{
			name += decompressedData.at(*index);
			(*index) += 1;
		}
		//for (size_t i = 0; i < layers; i++)
		//{
		//	printf(" ");
		//}
		//printf("parsing tag %s of type %s\n", name, tts(type));
		toReturn->addValue(name, parseNBT(decompressedData, index, type));
	}
	//layers--;
	return toReturn;
}
TagList* parseListTag(vector<unsigned char>& decompressedData, size_t* index, TagType type)
{
	TagType subType = (TagType)decompressedData.at(*index);
	(*index) += 1;
	int32_t size = ((uint32_t)decompressedData.at(*index) << 24 | (uint32_t)decompressedData.at(*index + 1) << 16 | (uint32_t)decompressedData.at(*index + 2) << 8 | (uint32_t)decompressedData.at(*index + 3));
	(*index) += 4;
	TagList* toReturn = new TagList(type, subType);
	for (size_t i = 0; i < size; i++)
	{
		toReturn->addValue(parseNBT(decompressedData, index, subType));
	}
	return toReturn;
}
Tag<int8_t>* parseByteTag(vector<unsigned char>& decompressedData, size_t* index, TagType type)
{

	int8_t inter = decompressedData.at(*index);
	(*index) += 1;
	return new Tag<int8_t>(inter, type);
}
Tag<int16_t>* parseShortTag(vector<unsigned char>& decompressedData, size_t* index, TagType type)
{
	int16_t inter = ((uint16_t)decompressedData.at(*index) << 8 | (uint16_t)decompressedData.at(*index + 1));
	(*index) += 2;
	return new Tag<int16_t>(inter, type);
}
Tag<int32_t>* parseIntTag(vector<unsigned char>& decompressedData, size_t* index, TagType type)
{
	int32_t inter = ((uint32_t)decompressedData.at(*index) << 24 | (uint32_t)decompressedData.at(*index + 1) << 16 | (uint32_t)decompressedData.at(*index + 2) << 8 | (uint32_t)decompressedData.at(*index + 3));
	(*index) += 4;
	return new Tag<int32_t>(inter, type);
}
Tag<int64_t>* parseLongTag(vector<unsigned char>& decompressedData, size_t* index, TagType type)
{
	int64_t inter =
		((uint64_t)decompressedData.at(*index) << 56 | (uint64_t)decompressedData.at(*index + 1) << 48 | (uint64_t)decompressedData.at(*index + 2) << 40 | (uint64_t)decompressedData.at(*index + 3) << 32 |
			(uint64_t)decompressedData.at(*index + 4) << 24 | (uint64_t)decompressedData.at(*index + 5) << 16 | (uint64_t)decompressedData.at(*index + 6) << 8 | (uint64_t)decompressedData.at(*index + 7));
	(*index) += 8;
	return new Tag<int64_t>(inter, type);
}
Tag<float>* parseFloatTag(vector<unsigned char>& decompressedData, size_t* index, TagType type)
{
	float inter;;
	memcpy(&inter, decompressedData.data() + (*index), sizeof(float));
	(*index) += sizeof(float);
	return new Tag<float>(inter, type);
}
Tag<double>* parseDoubleTag(vector<unsigned char>& decompressedData, size_t* index, TagType type)
{
	double inter;
	memcpy(&inter, decompressedData.data() + (*index), sizeof(double));
	(*index) += sizeof(double);
	return new Tag<double>(inter, type);
}
TagArray<int8_t>* parseByteArrayTag(vector<unsigned char>& decompressedData, size_t* index, TagType type)
{
	int sizeOfArray = ((uint32_t)decompressedData.at(*index) << 24 | (uint32_t)decompressedData.at(*index + 1) << 16 | (uint32_t)decompressedData.at(*index + 2) << 8 | (uint32_t)decompressedData.at(*index + 3));
	(*index) += 4;
	TagArray<int8_t>* inter = new TagArray<int8_t>(type);
	for (int i = 0; i < sizeOfArray; i++)
	{
		inter->addValue(decompressedData.at(*index));
		(*index) += 1;
	}
	return inter;
}
Tag<string>* parseStringTag(vector<unsigned char>& decompressedData, size_t* index, TagType type)
{
	short stringLength = ((uint16_t)decompressedData.at(*index) << 8 | (uint16_t)decompressedData.at(*index + 1));
	(*index) += 2;

	string x(decompressedData.begin() + *index, decompressedData.begin() + *index + stringLength);
	*index += stringLength;

	//string inter;

	//while(*index < end)
	//{	
	//	inter += nextUTF8Char(index, decompressedData);
	//}
	Tag<string>* result = new Tag<string>(x, type);
	return result;

	//short stringLength = ((uint16_t)decompressedData.at(*index) << 8 | (uint16_t)decompressedData.at(*index + 1));
	//(*index) += 2;
	//string inter;
	//for (int i = 0; i < stringLength; i++)
	//{
	//	inter += decompressedData.at(*index);
	//	(*index) += 1;
	//}
	//Tag<string>* result = new Tag<string>(inter, type);
	//return result;
}
TagArray<int32_t>* parseIntArrayTag(vector<unsigned char>& decompressedData, size_t* index, TagType type)
{
	int size = ((uint32_t)decompressedData.at(*index) << 24 | (uint32_t)decompressedData.at(*index + 1) << 16 | (uint32_t)decompressedData.at(*index + 2) << 8 | (uint32_t)decompressedData.at(*index + 3));
	(*index) += 4;
	TagArray<int32_t>* result = new TagArray<int32_t>(type);
	for (size_t i = 0; i < size; i++)
	{
		result->addValue(((uint32_t)decompressedData.at(*index) << 24 | (uint32_t)decompressedData.at(*index + 1) << 16 | (uint32_t)decompressedData.at(*index + 2) << 8 | (uint32_t)decompressedData.at(*index + 3)));
		(*index) += 4;
	}
	return result;
}
TagArray<int64_t>* parseLongArrayTag(vector<unsigned char>& decompressedData, size_t* index, TagType type)
{
	int size = ((uint32_t)decompressedData.at(*index) << 24 | (uint32_t)decompressedData.at(*index + 1) << 16 | (uint32_t)decompressedData.at(*index + 2) << 8 | (uint32_t)decompressedData.at(*index + 3));
	(*index) += 4;
	TagArray<int64_t>* result = new TagArray<int64_t>(type);
	for (size_t i = 0; i < size; i++)
	{
		result->addValue(((uint64_t)decompressedData.at(*index) << 56 | (uint64_t)decompressedData.at(*index + 1) << 48 | (uint64_t)decompressedData.at(*index + 2) << 40 | (uint64_t)decompressedData.at(*index + 3) << 32 |
			(uint64_t)decompressedData.at(*index + 4) << 24 | (uint64_t)decompressedData.at(*index + 5) << 16 | (uint64_t)decompressedData.at(*index + 6) << 8 | (uint64_t)decompressedData.at(*index + 7)));
		(*index) += 8;
	}
	return result;
}

void printTags(SuperTag* root)
{
	TagType type = root->getType();
	switch (type)
	{
	case TAG_COMPOUND:
	{
		CompoundTag* toPrint = dynamic_cast<CompoundTag*>(root);
		unordered_map<string, SuperTag*> vals = toPrint->getValues();
		//printf("increasing tags with compound\n");
		for (auto k = 0; k < layers; k++)
		{
			printf(" ");
		}
		printf("BOC\n");
		layers++;
		for (auto x : vals)
		{
			for (auto k = 0; k < layers; k++)
			{
				printf(" ");
			}
			printf("%s of type %s", x.first, tts(x.second->getType()).c_str());
			if (x.second->getType() == TAG_LIST)
			{
				printf(" with subtype %s", tts(x.second->toList()->getSubType()));
			}
			printf("\n");
			printTags(x.second);
		}
		layers--;
		for (auto k = 0; k < layers; k++)
		{
			printf(" ");
		}
		printf("EOC\n");
		break;
	}
	case TAG_LIST:
	{
		for (auto k = 0; k < layers; k++)
		{
			printf(" ");
		}
		printf("BOL\n");
		layers++;
		//printf("increasing tags with list\n");
		TagList* toPrint = dynamic_cast<TagList*>(root);

		for (auto i : root->toList()->getValues())
		{
			//if(i->getType() == TAG_COMPOUND)
			//	printf("------new list section--------\n");
			printTags(i);
		}
		layers--;
		for (auto k = 0; k < layers; k++)
		{
			printf(" ");
		}
		printf("EOL\n");

	}
	break;
	default:
		break;
	}
}


//TAG_END,
//TAG_BYTE,
//TAG_SHORT,
//TAG_INT,
//TAG_LONG,
//TAG_FLOAT,
//TAG_DOUBLE,
//TAG_BYTE_ARRAY,
//TAG_STRING,
//TAG_LIST,
//TAG_COMPOUND,
//TAG_INT_ARRAY,
//TAG_LONG_ARRAY

//switch (type)
//{
//case TAG_COMPOUND:
//{
//}
//break;
//case TAG_BYTE:
//{
//}
//break;
//case TAG_SHORT:
//{
//}
//break;
//case TAG_INT:
//{
//}
//break;
//case TAG_LONG:
//{
//}
//break;
//case TAG_FLOAT:
//{
//}
//break;
//case TAG_DOUBLE:
//{
//}
//break;
//case TAG_BYTE_ARRAY:
//{
//}
//break;
//case TAG_STRING:
//{
//}
//case TAG_LIST:
//{
//}
//break;
//case TAG_INT_ARRAY:
//{
//}
//break;
//case TAG_LONG_ARRAY:
//{
//}
//break;
//default:
//{
//	fprintf(stderr, "Forgot to handle nbt tab: %i", type);
//	exit(-1);
//}
//break;
//}


/*#include "NBTTags.hpp"
#include <string>
#include <vector>

using namespace std;



int32_t nextUTF8Char(size_t * index, const vector<unsigned char>& decompressedData)
{
	int32_t toReturn;
	if (decompressedData.at(*index) <= 0x7f)
	{
		toReturn = decompressedData.at(*index);
		(*index) += 1;
	}
	else if (decompressedData.at(*index) <= 0x7ff)
	{
		toReturn = (decompressedData.at(*index) - 0b11000000) << 6 || (decompressedData.at(*index+1) - 0b10000000);
		(*index) += 2;
	}
	else if (decompressedData.at(*index) <= 0xffff)
	{
		toReturn = (decompressedData.at(*index) - 0b111000000) << 12 || (decompressedData.at(*index+1) - 0b10000000)<<6 || decompressedData.at(*index+2) - 0b10000000;
		(*index) += 3;
	}
	else if (decompressedData.at(*index) <= 0x10ffff)
	{
		toReturn = (decompressedData.at(*index) - 0b111100000) << 18 || (decompressedData.at(*index + 1) - 0b10000000) << 12 || (decompressedData.at(*index + 2) - 0b10000000) << 6 || decompressedData.at(*index + 3) - 0b10000000;
		(*index) += 4;
	}
	else
	{
		fprintf(stderr, "bad utf decoding\n");
		exit(-1);
	}
	return toReturn;
}

//converts a TagType to a string for print debugging
string tts(TagType t)
{
	switch (t)
	{
	case TAG_COMPOUND:
		return "COMPOUND";
	case TAG_BYTE:
		return "BYTE";
	case TAG_SHORT:
		return "SHORT";
	case TAG_INT:
		return "INT";
	case TAG_LONG:
		return "LONG";
	case TAG_FLOAT:
		return "FLOAT";
	case TAG_DOUBLE:
		return "DOUBLE";
	case TAG_BYTE_ARRAY:
		return "BYTE ARRAY";
	case TAG_STRING:
		return "STRING";
	case TAG_LIST:
		return "LIST";
	case TAG_INT_ARRAY:
		return "INT ARRAY";
	case TAG_LONG_ARRAY:
		return "LONG ARRAY";
	case TAG_END:
		return "TAG END";
	default:
		return "WEIRD TAG";
	}
}


TagList SuperTag::toList()
{
	return *dynamic_cast<TagList*>(this);
}
CompoundTag SuperTag::toCT()
{
	return *dynamic_cast<CompoundTag*>(this);
}

CompoundTag parseNBT(vector<unsigned char>& decompressedData, size_t* index)
{
	TagType type = (TagType)decompressedData.at(*index);
	(*index) += 1;

	size_t nameLength = ((size_t)decompressedData.at(*index) << 8 | (size_t)decompressedData.at((*index + 1)));
	(*index) += 2;
	string name;
	for (size_t i = 0; i < nameLength; i++)
	{
		name += decompressedData.at(*index);
		(*index) += 1;
	}
	return parseCompoundTag(decompressedData, index, type);
	//return dynamic_cast<CompoundTag*>(parseNBT(decompressedData, index, TAG_COMPOUND));
}
SuperTag parseNBT(vector<unsigned char>& decompressedData, size_t* index, TagType type)
{
	//TagType type = (TagType)decompressedData.at(*index);
	//(*index) += 1;

	switch (type)
	{
		case TAG_END:
			return Tag<uint8_t>(0, TAG_END);
		case TAG_COMPOUND:
			return parseCompoundTag(decompressedData, index, type);
		case TAG_BYTE:
			return parseByteTag(decompressedData, index, type);
		case TAG_SHORT:
			return parseShortTag(decompressedData, index, type);
		case TAG_INT:
			return parseIntTag(decompressedData, index, type);
		case TAG_LONG:
			return parseLongTag(decompressedData, index, type);
		case TAG_FLOAT:
			return parseFloatTag(decompressedData, index, type);
		case TAG_DOUBLE:
			return parseDoubleTag(decompressedData, index, type);
		case TAG_BYTE_ARRAY:
			return parseByteArrayTag(decompressedData, index, type);
		case TAG_STRING:
			return parseStringTag(decompressedData, index, type);
		case TAG_LIST:
			return parseListTag(decompressedData, index, type);
		case TAG_INT_ARRAY:
			return parseIntArrayTag(decompressedData, index, type);
		case TAG_LONG_ARRAY:
			return parseLongArrayTag(decompressedData, index, type);
		default:
			fprintf(stderr, "Forgot to handle nbt tab: %i", type);
			exit(-1);
	}
}

static uint64_t layers = 0;

CompoundTag parseCompoundTag(vector<unsigned char>& decompressedData, size_t* index, TagType type)
{
	CompoundTag toReturn(type);// = new CompoundTag(type);
	//layers++;
	while (true)
	{
		TagType type = (TagType)decompressedData.at(*index);
		(*index) += 1;
		if (type == TAG_END)
		{
			break;
		}
		uint16_t nameLength = ((size_t)decompressedData.at(*index) << 8 | (size_t)decompressedData.at((*index + 1)));
		(*index) += 2;
		string name;
		for (size_t i = 0; i < nameLength; i++)
		{
			name += decompressedData.at(*index);
			(*index) += 1;
		}
		//for (size_t i = 0; i < layers; i++)
		//{
		//	printf(" ");
		//}
		//printf("parsing tag %s of type %s\n", name, tts(type));
		toReturn.addValue(name, parseNBT(decompressedData, index, type));
	}
	//layers--;
	return toReturn;
}
TagList parseListTag(vector<unsigned char>& decompressedData, size_t* index, TagType type)
{
	TagType subType = (TagType)decompressedData.at(*index);
	(*index) += 1;
	int32_t size = ((uint32_t)decompressedData.at(*index) << 24 | (uint32_t)decompressedData.at(*index + 1) << 16 | (uint32_t)decompressedData.at(*index + 2) << 8 | (uint32_t)decompressedData.at(*index + 3));
	(*index) += 4;
	TagList toReturn(type, subType);
	for (size_t i = 0; i < size; i++)
	{
		toReturn.addValue(parseNBT(decompressedData, index, subType));
	}
	return toReturn;
}
Tag<int8_t> parseByteTag(const vector<unsigned char>& decompressedData, size_t* index, TagType type)
{

	int8_t inter = decompressedData.at(*index);
	(*index) += 1;
	return Tag<int8_t>(inter, type);
}
Tag<int16_t> parseShortTag(const vector<unsigned char>& decompressedData, size_t* index, TagType type)
{
	int16_t inter = ((uint16_t)decompressedData.at(*index) << 8 | (uint16_t)decompressedData.at(*index + 1));
	(*index) += 2;
	return Tag<int16_t>(inter, type);
}
Tag<int32_t> parseIntTag(const vector<unsigned char>& decompressedData, size_t* index, TagType type)
{
	int32_t inter = ((uint32_t)decompressedData.at(*index) << 24 | (uint32_t)decompressedData.at(*index + 1) << 16 | (uint32_t)decompressedData.at(*index + 2) << 8 | (uint32_t)decompressedData.at(*index + 3));
	(*index) += 4;
	return Tag<int32_t>(inter, type);
}
Tag<int64_t> parseLongTag(const vector<unsigned char>& decompressedData, size_t* index, TagType type)
{
	int64_t inter =
		((uint64_t)decompressedData.at(*index) << 56 | (uint64_t)decompressedData.at(*index + 1) << 48 | (uint64_t)decompressedData.at(*index + 2) << 40 | (uint64_t)decompressedData.at(*index + 3) << 32 |
		(uint64_t)decompressedData.at(*index + 4) << 24 | (uint64_t)decompressedData.at(*index + 5) << 16 | (uint64_t)decompressedData.at(*index + 6) << 8 | (uint64_t)decompressedData.at(*index + 7));
	(*index) += 8;
	return Tag<int64_t>(inter, type);
}
Tag<float> parseFloatTag(const vector<unsigned char>& decompressedData, size_t* index, TagType type)
{
	float inter;;
	memcpy(&inter, decompressedData.data() + (*index), sizeof(float));
	(*index) += sizeof(float);
	return Tag<float>(inter, type);
}
Tag<double> parseDoubleTag(const vector<unsigned char>& decompressedData, size_t* index, TagType type)
{
	double inter;
	memcpy(&inter, decompressedData.data() + (*index), sizeof(double));
	(*index) += sizeof(double);
	return Tag<double>(inter, type);
}
TagArray<int8_t> parseByteArrayTag(const vector<unsigned char>& decompressedData, size_t* index, TagType type)
{
	int sizeOfArray = ((uint32_t)decompressedData.at(*index) << 24 | (uint32_t)decompressedData.at(*index + 1) << 16 | (uint32_t)decompressedData.at(*index + 2) << 8 | (uint32_t)decompressedData.at(*index + 3));
	(*index) += 4;
	TagArray<int8_t> inter(type);
	for (int i = 0; i < sizeOfArray; i++)
	{
		inter.addValue(decompressedData.at(*index));
		(*index) += 1;
	}
	return inter;
}
Tag<string> parseStringTag(const vector<unsigned char>& decompressedData, size_t* index, TagType type)
{
	short stringLength = ((uint16_t)decompressedData.at(*index) << 8 | (uint16_t)decompressedData.at(*index + 1));
	(*index) += 2;

	string x(decompressedData.begin() + *index, decompressedData.begin() + *index + stringLength);
	*index += stringLength;

	//string inter;

	//while(*index < end)
	//{
	//	inter += nextUTF8Char(index, decompressedData);
	//}
	Tag<string> result(x, type);
	return result;

	//short stringLength = ((uint16_t)decompressedData.at(*index) << 8 | (uint16_t)decompressedData.at(*index + 1));
	//(*index) += 2;
	//string inter;
	//for (int i = 0; i < stringLength; i++)
	//{
	//	inter += decompressedData.at(*index);
	//	(*index) += 1;
	//}
	//Tag<string>* result = new Tag<string>(inter, type);
	//return result;
}
TagArray<int32_t> parseIntArrayTag(const vector<unsigned char>& decompressedData, size_t* index, TagType type)
{
	int size = ((uint32_t)decompressedData.at(*index) << 24 | (uint32_t)decompressedData.at(*index + 1) << 16 | (uint32_t)decompressedData.at(*index + 2) << 8 | (uint32_t)decompressedData.at(*index + 3));
	(*index) += 4;
	TagArray<int32_t> result(type);//= TagArray<int32_t>(type);
	for (size_t i = 0; i < size; i++)
	{
		result.addValue(((uint32_t)decompressedData.at(*index) << 24 | (uint32_t)decompressedData.at(*index + 1) << 16 | (uint32_t)decompressedData.at(*index + 2) << 8 | (uint32_t)decompressedData.at(*index + 3)));
		(*index) += 4;
	}
	return result;
}
TagArray<int64_t> parseLongArrayTag(const vector<unsigned char>& decompressedData, size_t* index, TagType type)
{
	int size = ((uint32_t)decompressedData.at(*index) << 24 | (uint32_t)decompressedData.at(*index + 1) << 16 | (uint32_t)decompressedData.at(*index + 2) << 8 | (uint32_t)decompressedData.at(*index + 3));
	(*index) += 4;
	TagArray<int64_t> result(type);
	for (size_t i = 0; i < size; i++)
	{
		result.addValue(((uint64_t)decompressedData.at(*index) << 56 | (uint64_t)decompressedData.at(*index + 1) << 48 | (uint64_t)decompressedData.at(*index + 2) << 40 | (uint64_t)decompressedData.at(*index + 3) << 32 |
			(uint64_t)decompressedData.at(*index + 4) << 24 | (uint64_t)decompressedData.at(*index + 5) << 16 | (uint64_t)decompressedData.at(*index + 6) << 8 | (uint64_t)decompressedData.at(*index + 7)));
		(*index) += 8;
	}
	return result;
}

/*void printTags(SuperTag* root)
{
	TagType type = root->getType();
	switch (type)
	{
	case TAG_COMPOUND:
	{
		CompoundTag* toPrint = dynamic_cast<CompoundTag*>(root);
		unordered_map<string, SuperTag*> vals = toPrint->getValues();
		//printf("increasing tags with compound\n");
		for (auto k = 0; k < layers; k++)
		{
			printf(" ");
		}
		printf("BOC\n");
		layers++;
		for (auto x : vals)
		{
			for (auto k = 0; k < layers; k++)
			{
				printf(" ");
			}
			printf("%s of type %s", x.first, tts(x.second->getType()).c_str());
			if (x.second->getType() == TAG_LIST)
			{
				printf(" with subtype %s", tts(x.second->toList()->getSubType()));
			}
			printf("\n");
			printTags(x.second);
		}
		layers--;
		for (auto k = 0; k < layers; k++)
		{
			printf(" ");
		}
		printf("EOC\n");
		break;
	}
	case TAG_LIST:
	{
		for (auto k = 0; k < layers; k++)
		{
			printf(" ");
		}
		printf("BOL\n");
		layers++;
		//printf("increasing tags with list\n");
		TagList* toPrint = dynamic_cast<TagList*>(root);

		for (auto i : root->toList()->getValues())
		{
			//if(i->getType() == TAG_COMPOUND)
			//	printf("------new list section--------\n");
			printTags(i);
		}
		layers--;
		for (auto k = 0; k < layers; k++)
		{
			printf(" ");
		}
		printf("EOL\n");

	}
		break;
	default:
		break;
	}
}*/


//TAG_END,
//TAG_BYTE,
//TAG_SHORT,
//TAG_INT,
//TAG_LONG,
//TAG_FLOAT,
//TAG_DOUBLE,
//TAG_BYTE_ARRAY,
//TAG_STRING,
//TAG_LIST,
//TAG_COMPOUND,
//TAG_INT_ARRAY,
//TAG_LONG_ARRAY

//switch (type)
//{
//case TAG_COMPOUND:
//{
//}
//break;
//case TAG_BYTE:
//{
//}
//break;
//case TAG_SHORT:
//{
//}
//break;
//case TAG_INT:
//{
//}
//break;
//case TAG_LONG:
//{
//}
//break;
//case TAG_FLOAT:
//{
//}
//break;
//case TAG_DOUBLE:
//{
//}
//break;
//case TAG_BYTE_ARRAY:
//{
//}
//break;
//case TAG_STRING:
//{
//}
//case TAG_LIST:
//{
//}
//break;
//case TAG_INT_ARRAY:
//{
//}
//break;
//case TAG_LONG_ARRAY:
//{
//}
//break;
//default:
//{
//	fprintf(stderr, "Forgot to handle nbt tab: %i", type);
//	exit(-1);
//}
//break;
//}*/