/*! \file NBTTags.hpp
* This file contains everything you need to parse the NBT data of a minecraft save file.
* I suppose it would work for any old NBT data, but IDK if anything uses it besides minecraft.
* I would read this before going any further <a href="https://minecraft.gamepedia.com/NBT_format">https://minecraft.gamepedia.com/NBT_format</a>
*/


#ifndef NBTTAGS_H
#define NBTTAGS_H
#pragma once
#include <vector>
#include <string>
#include <cstdint>
//#include "Chunk.hpp"
//#include "RegionLoader.h"
#include <unordered_map>



enum TagType
{
	TAG_END = 0,
	TAG_BYTE = 1,
	TAG_SHORT = 2,
	TAG_INT = 3,
	TAG_LONG = 4,
	TAG_FLOAT = 5,
	TAG_DOUBLE = 6,
	TAG_BYTE_ARRAY = 7,
	TAG_STRING = 8,
	TAG_LIST = 9,
	TAG_COMPOUND = 10,
	TAG_INT_ARRAY = 11,
	TAG_LONG_ARRAY = 12
};


using namespace std;

//static size_t layers = 0;

template <class T>
class TagArray;

template <class T>
class Tag;

class TagList;
class CompoundTag;




/*! \class SuperTag
* The all encompassing tag type. Every other tag is a subclass of this tag.
* P O L Y M O R P H I C
* Basically abstract, shouldn't really be initialized.
*/
class SuperTag
{
	TagType type;
public:
	
	SuperTag(TagType t)
	{
		type = t;
	}
	virtual ~SuperTag() = default;

	TagType getType()
	{
		return type;
	}


	TagList* toList();
	CompoundTag* toCT();

	template <class T>
	Tag<T>* toTag()
	{
		return dynamic_cast<Tag<T>*>(this);
	}

	template <class T>
	TagArray<T>* toTagArray()
	{
		return dynamic_cast<TagArray<T>*>(this);
	}


private:

};

template <class T>
class Tag : public SuperTag
{
	T value;
public:

	Tag(T val, TagType t) : SuperTag(t)
	{
		value = val;
	}
	virtual ~Tag()
	{
	}
	T getValue()
	{
		return value;
	}


private:
};

template <class T>
class TagArray : public SuperTag
{
	vector<T> values;
public:
	TagArray(TagType t) : SuperTag(t)
	{
	}
	virtual ~TagArray()
	{
	}
	void addValue(T val)
	{
		values.push_back(val);
	}
	vector<T> getValues()
	{
		return values;
	}

private:
};

class TagList : public SuperTag
{
public:
	vector<SuperTag*> values;
	TagType subType;
	TagList(TagType t, TagType st) : SuperTag(t)
	{
		subType = st;
	}
	virtual ~TagList()
	{
		for (auto v : values)
		{
			delete(v);
		}
	}
	void addValue(SuperTag* val)
	{
		values.push_back(val);
	}
	vector<SuperTag*> getValues()
	{
		return values;
	}
	TagType getSubType()
	{
		return subType;
	}
private:
};

class CompoundTag : public SuperTag
{
public:
	unordered_map<string, SuperTag*>  values;
	CompoundTag(TagType t) : SuperTag(t)
	{
	}
	~CompoundTag()
	{
		for (auto t : values)
		{
			delete(t.second);
		}
	}
	SuperTag addValue(string name, SuperTag* val)
	{
		values[name] = val;
		return *val;
	}
	unordered_map<string, SuperTag*> getValues()
	{
		return values;
	}
	SuperTag* getTag(string name)
	{
		//printf("finding %s\n", name);
		//for (auto& [key, value] : values)
		//{
		//	printf("possible values");
		//	printf("%s\n", key);
		//}
		return values.at(name);
	}


private:
};

string tts(TagType t);
int32_t nextUTF8Char(size_t* index, vector<unsigned char>& decompressedData);
CompoundTag* parseNBT(vector<unsigned char>& decompressedData, size_t* index);
SuperTag* parseNBT(vector<unsigned char>& decompressedData, size_t* index, TagType type);
Tag<int8_t>* parseByteTag(vector<unsigned char>& decompressedData, size_t* index, TagType type);
Tag<int16_t>* parseShortTag(vector<unsigned char>& decompressedData, size_t* index, TagType type);
Tag<int32_t>* parseIntTag(vector<unsigned char>& decompressedData, size_t* index, TagType type);
Tag<int64_t>* parseLongTag(vector<unsigned char>& decompressedData, size_t* index, TagType type);
Tag<float>* parseFloatTag(vector<unsigned char>& decompressedData, size_t* index, TagType type);
Tag<double>* parseDoubleTag(vector<unsigned char>& decompressedData, size_t* index, TagType type);
TagArray<int8_t>* parseByteArrayTag(vector<unsigned char>& decompressedData, size_t* index, TagType type);
Tag<string>* parseStringTag(vector<unsigned char>& decompressedData, size_t* index, TagType type);
TagArray<int32_t>* parseIntArrayTag(vector<unsigned char>& decompressedData, size_t* index, TagType type);
TagArray<int64_t>* parseLongArrayTag(vector<unsigned char>& decompressedData, size_t* index, TagType type);
CompoundTag* parseCompoundTag(vector<unsigned char>& decompressedData, size_t* index, TagType type);
TagList* parseListTag(vector<unsigned char>& decompressedData, size_t* index, TagType type);
void printTags(SuperTag* root);

#endif