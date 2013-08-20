#pragma once
#include <iostream>
#define MINIZ_HEADER_FILE_ONLY 1
#include "miniz.c"

class TMIStream : public std::ifstream
{
	unsigned blockSize;

	unsigned char* compressionBuffer;
	unsigned compressionBufferSize;
public:

	TMIStream(const char* fileName)
		:	std::ifstream(fileName, std::ifstream::in | std::ifstream::binary),
			blockSize(0), compressionBuffer(0), compressionBufferSize(0)
	{

	}

	~TMIStream()
	{
		delete[] compressionBuffer;
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

	void readCompressedData(char* destination, unsigned size)
	{
		// Read the size of the compressed input data
		mz_ulong dataSize;
		*this >> dataSize;
				
		if (destination)
		{
			// Buffer already exists, but is too small...
			if (compressionBuffer && compressionBufferSize < dataSize)
			{
				delete[] compressionBuffer;
				compressionBuffer = 0;
			}

			// Need to allocate a compression buffer
			if (!compressionBuffer)
			{
				compressionBuffer = new unsigned char[dataSize];
				compressionBufferSize = dataSize;
			}

			// Read the compressed data
			read((char*)compressionBuffer, dataSize);

			// Uncompress data
			mz_ulong dataAlloc = size;
			mz_uncompress((unsigned char*)destination, &dataAlloc, compressionBuffer, dataSize);
		}
		// We can't read this...
		else
		{
			seekg(dataSize, ios_base::cur);
		}
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