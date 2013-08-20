#pragma once
#include <iostream>
#define MINIZ_HEADER_FILE_ONLY 1
#include "miniz.c"

class TMIStream : public std::ifstream
{
	unsigned blockSize;

public:

	TMIStream(const char* fileName)
		:	std::ifstream(fileName, std::ifstream::in | std::ifstream::binary),
			blockSize(0)
	{

	}

	~TMIStream()
	{
	}

	// Reads a file block header (map, layer, tileset)
	unsigned readBlockHeader()
	{
		unsigned identifier;

		*this >> identifier;
		*this >> blockSize;

		return identifier;
	}

	// Skips the current file block
	void skipBlock()
	{
		seekg(blockSize, ios_base::cur);
	}

	// Read a binary block as big as the given buffer
	template <class T>
	TMIStream& operator >>(T& i)
	{
		read(reinterpret_cast<char*>(&i), sizeof(i));
		return *this;
	}

	// Read a string preceded by its length into a buffer (max 256 bytes)
	unsigned char readShortStr(char* buffer)
	{
		unsigned char length;
		*this >> length;
		read(buffer, length);

		return length;
	}

	// Read a string preceded by its length into a newly allocated buffer
	char* readLongStr()
	{
		unsigned char length;
		*this >> length;

		char* buffer = new char[length + 1];
		buffer[length] = 0;

		read(buffer, length);
		return buffer;
	}
};