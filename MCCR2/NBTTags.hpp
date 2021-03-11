/*! \file NBTTags.hpp
* This file contains everything you need to parse the NBT data of a minecraft save file.
* I suppose it would work for any old NBT data, but IDK if anything uses it besides minecraft.
* I would read this before going any further <a href="https://minecraft.gamepedia.com/NBT_format">https://minecraft.gamepedia.com/NBT_format</a>
* 
* The main entry point of this file is SuperTag* ParseNBT(). This starts the process of parsing
* the giant byte array known as "decompressedData". It must be given a pointer to an interger of value 0.
* It uses this integer as a bookmark in the decompressedData, slowly moving it as it iterates through the data.
* I can't guarentee the behavior of any of the other functions if not called from parseNBT() which was called with the decompressed 
* data of a single chunk from a minecraft save file.
* 
* Each parse[sometype]Tag() function recursively parses the NBT data interitvly, with the types of tags that contain other tags calling
* parse functions to fill up their lists and maps. 
*
* Every parse function is responsible for adding the number of bytes it read to the index value.
*/


#ifndef NBTTAGS_H
#define NBTTAGS_H
#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <unordered_map>
#include "Config.h"



/*! \enum TagType
* \brief An Enum mapping tag type ids to labels for code readability. #magicNumbers
*/
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
* \brief The all encompassing tag type. Every other tag is a subclass of this tag.
* 
* The all encompassing tag type. Every other tag is a subclass of this tag.
* P O L Y M O R P H I C
* Basically abstract, shouldn't really be initialized.
*/
class SuperTag
{
	TagType type; ///<The type of this tag.
public:
	
	/** \fn SuperTag
	* \param t The TagType of this tag.
	*/
	SuperTag(TagType t)
	{
		type = t;
	}
	virtual ~SuperTag() = default;

	/*! \fn getType
	* \returns The TagType of this tag.
	*/
	TagType getType()
	{
		return type;
	}


	/*! \fn toList
	* \returns This tag dynamic cast to a TagList.
	*/
	TagList* toList();

	/*! \fn toCT
	* \returns This tag dynamic cast to a CompoundTag.
	*/
	CompoundTag* toCT();


	/*! \fn toTag
	* \returns This tag dynamic cast to a plain old Tag<T>.
	*/
	template <class T>
	Tag<T>* toTag()
	{
		return dynamic_cast<Tag<T>*>(this);
	}

	/*! \fn toTagArray
	* \returns This tag dynamic cast to a TagArray<T>.
	*/
	template <class T>
	TagArray<T>* toTagArray()
	{
		return dynamic_cast<TagArray<T>*>(this);
	}


private:

};




/*! \class Tag
* \brief A single value Tag of variable type. Basically just a container.
*/
template <class T>
class Tag : public SuperTag
{
	T value; ///<The value of the tag.
public:

	/*! \fn Tag
	* \param val The value to store in the tag.
	* \param t The TagType related to the value stored.
	*/
	Tag(T val, TagType t) : SuperTag(t)
	{
		value = val;
	}
	virtual ~Tag()
	{
	}
	/*! \fn getValue
	* \returns The value stored by this Tag.
	*/
	T getValue()
	{
		return value;
	}


private:
};

/*! \class TagArray
* \brief A multiple value Tag of a (single) variable type. Basically just a container for an array.
*/
template <class T>
class TagArray : public SuperTag
{
	vector<T> values; ///<The array of values in the array.
public:
	/*! \fn TagArray
	* \param t The TagType for this tag.
	*/
	TagArray(TagType t) : SuperTag(t)
	{
	}
	virtual ~TagArray()
	{
	}
	/*! \fn addValue
	* \param val The value to be added to this array.
	*/
	void addValue(T val)
	{
		values.push_back(val);
	}
	/*! \fn getValues
	* \returns The array of values contained by this tag.
	*/
	vector<T> getValues()
	{
		return values;
	}

private:
};


/*! \class TagList
* \brief A list of tags themselves. The Tags are all the same type as defined by subType.
*/
class TagList : public SuperTag
{
public:
	vector<SuperTag*> values; ///<The array of SuperTags
	TagType subType; ///<The type of tags the SuperTags actually are.

	/*! \fn TagList
	* \param t The TagType for this tag.
	* \param st The TagType for the tags contained by this tag.
	*/
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

	/*! \fn addValue
	* \param val The tag to be added to this array.
	*/
	void addValue(SuperTag* val)
	{
		values.push_back(val);
	}
	/*! \fn getValues
	* \returns The array of tags contained by this tag.
	*/
	vector<SuperTag*> getValues()
	{
		return values;
	}
	/*! \fn getSubType
	* \returns The TagType of the tags contained by this tag.
	*/
	TagType getSubType()
	{
		return subType;
	}
private:
};

/*! \class CompoundTag
* \brief Represents a compound tag. Has a mapping of names to various differen types of tags.
*/
class CompoundTag : public SuperTag
{
public:
	unordered_map<string, SuperTag*>  values;///<The map containing all of the string:SuperTag pairs.
	
	/*! \fn CompoundTag
	* \param t The TagType of this tag.
	*/
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
	/*! \fn addValue
	* Adds a tag to this tag's map.
	* \param name The key to add the tag under.
	* \param val The SuperTag to map.
	* \returns the deferenced tag that was added.
	*/
	SuperTag addValue(string name, SuperTag* val)
	{
		values[name] = val;
		return *val;
	}
	/*! \fn getValues
	* \returns The map containing all the keys and values in this tag.
	*/
	unordered_map<string, SuperTag*> getValues()
	{
		return values;
	}
	/*! \fn getTag
	* \returns The mapped value to the key <name>. Null if it doesn't not exist.
	*/
	SuperTag* getTag(string name)
	{
		return values.at(name);
	}
private:
};

/*! \fn tts
* \param t The TagType whose name is wanted.
* \returns The string name of the TagType.
*/
string tts(TagType t);

/*! \fn nextUTF8Char
*	Reads in a character from the decompressed data. UTF8 characters can range in size,
*	and this funtion makes sure that the index is progressed correctly. Returns a 32bit int
*	containing the character. Might have a bunch of leading 0 bits.
*	\param decompressedData The chunk data.
*	\param index How far into the data the parsing process is.
*	\returns The next UTF8 character in the decompressed data.
*/
int32_t nextUTF8Char(size_t* index, vector<unsigned char>& decompressedData);

/*! \fn parseNBT
*	\param decompressedData The chunk data.
*	\param index How far into the data the parsing process is.
*	\param type The TagType that the returned tag should be.
*	\returns An NBT tag representing the data given.
*/
SuperTag* parseNBT(vector<unsigned char>& decompressedData, size_t* index, TagType type);


/*! \fn parseNBT
*	Parses a compound tag from the decompressed data.
*	\param decompressedData The chunk data.
*	\param index How far into the data the parsing process is.
*	\param type The TagType that the returned tag should be.
*	\returns An NBT tag representing the data given.
*/
CompoundTag* parseNBT(vector<unsigned char>& decompressedData, size_t* index);

/*! \fn parseByteTag
*	Parses a tag representing a byte from the decompressed data.
*	\param decompressedData The chunk data.
*	\param index How far into the data the parsing process is.
*	\param type The TagType that the returned tag should be.
*	\returns An NBT tag representing the data given.
*/
Tag<int8_t>* parseByteTag(vector<unsigned char>& decompressedData, size_t* index, TagType type);

/*! \fn parseShortTag
*	Parses a tag representing a short from the decompressed data.
*	\param decompressedData The chunk data.
*	\param index How far into the data the parsing process is.
*	\param type The TagType that the returned tag should be.
*	\returns An NBT tag representing the data given.
*/
Tag<int16_t>* parseShortTag(vector<unsigned char>& decompressedData, size_t* index, TagType type);

/*! \fn parseIntTag
*	Parses a tag representing a 32 bit int from the decompressed data.
*	\param decompressedData The chunk data.
*	\param index How far into the data the parsing process is.
*	\param type The TagType that the returned tag should be.
*	\returns An NBT tag representing the data given.
*/
Tag<int32_t>* parseIntTag(vector<unsigned char>& decompressedData, size_t* index, TagType type);

/*! \fn parseLongTag
*	Parses a tag representing a 64 bit int from the decompressed data.
*	\param decompressedData The chunk data.
*	\param index How far into the data the parsing process is.
*	\param type The TagType that the returned tag should be.
*	\returns An NBT tag representing the data given.
*/
Tag<int64_t>* parseLongTag(vector<unsigned char>& decompressedData, size_t* index, TagType type);

/*! \fn parseFloatTag
*	Parses a tag representing a float from the decompressed data.
*	\param decompressedData The chunk data.
*	\param index How far into the data the parsing process is.
*	\param type The TagType that the returned tag should be.
*	\returns An NBT tag representing the data given.
*/
Tag<float>* parseFloatTag(vector<unsigned char>& decompressedData, size_t* index, TagType type);

/*! \fn parseDoubleTag
*	Parses a tag representing a double from the decompressed data.
*	\param decompressedData The chunk data.
*	\param index How far into the data the parsing process is.
*	\param type The TagType that the returned tag should be.
*	\returns An NBT tag representing the data given.
*/
Tag<double>* parseDoubleTag(vector<unsigned char>& decompressedData, size_t* index, TagType type);

/*! \fn parseByteTag
*	Parses a tag representing a byte from the decompressed data.
*	\param decompressedData The chunk data.
*	\param index How far into the data the parsing process is.
*	\param type The TagType that the returned tag should be.
*	\returns An NBT tag representing the data given.
*/
TagArray<int8_t>* parseByteArrayTag(vector<unsigned char>& decompressedData, size_t* index, TagType type);

/*! \fn parseStringTag
*	Parses a tag representing a string from the decompressed data.
*	\param decompressedData The chunk data.
*	\param index How far into the data the parsing process is.
*	\param type The TagType that the returned tag should be.
*	\returns An NBT tag representing the data given.
*/
Tag<string>* parseStringTag(vector<unsigned char>& decompressedData, size_t* index, TagType type);

/*! \fn parseIntArrayTag
*	Parses a tag representing an array of 32 bit integers from the decompressed data.
*	\param decompressedData The chunk data.
*	\param index How far into the data the parsing process is.
*	\param type The TagType that the returned tag should be.
*	\returns An NBT tag representing the data given.
*/
TagArray<int32_t>* parseIntArrayTag(vector<unsigned char>& decompressedData, size_t* index, TagType type);

/*! \fn parseLongArrayTag
*	Parses a tag representing an array of 64 bit integers from the decompressed data.
*	\param decompressedData The chunk data.
*	\param index How far into the data the parsing process is.
*	\param type The TagType that the returned tag should be.
*	\returns An NBT tag representing the data given.
*/
TagArray<int64_t>* parseLongArrayTag(vector<unsigned char>& decompressedData, size_t* index, TagType type);

/*! \fn parseCompoundTag
*	Parses a tag representing a string:SuperTag map from the decompressed data.
*	\param decompressedData The chunk data.
*	\param index How far into the data the parsing process is.
*	\param type The TagType that the returned tag should be.
*	\returns An NBT tag representing the data given.
*/
CompoundTag* parseCompoundTag(vector<unsigned char>& decompressedData, size_t* index, TagType type);

/*! \fn parseListTag
*	Parses a tag representing a list of SuperTags from the decompressed data.
*	\param decompressedData The chunk data.
*	\param index How far into the data the parsing process is.
*	\param type The TagType that the returned tag should be.
*	\returns An NBT tag representing the data given.
*/
TagList* parseListTag(vector<unsigned char>& decompressedData, size_t* index, TagType type);

/*! \fn printTags
*	Prints out the tag and all subtags. Only used in troubleshooting.
*	\param root The tag to recursivly print out.
*/
void printTags(SuperTag* root);

#endif