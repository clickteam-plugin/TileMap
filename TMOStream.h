#pragma once
#include <iostream>
#define MINIZ_HEADER_FILE_ONLY 1
#include "miniz.c"

class TMOStream : public std::ofstream {
    int compressionLevel;

    unsigned blockBeginPos;

    unsigned char* compressionBuffer;
    unsigned compressionBufferSize;

  public:
    TMOStream(const char* fileName, int compressionLevel)
        : std::ofstream(fileName, std::ofstream::out | std::ofstream::binary),
          compressionLevel(compressionLevel), blockBeginPos(0),
          compressionBuffer(0), compressionBufferSize(0)
    {
    }

    ~TMOStream() { delete[] compressionBuffer; }

    // Begins a block (Map, Layer, Tileset)
    void beginBlock(unsigned id)
    {
        // 4-byte identifier
        *this << id;
        // Write the current position so we can go back and update the block
        // size
        blockBeginPos = tellp();
        // Dummy block size
        *this << 0;
    }

    // Ends a block by writing its size to the block header
    void endBlock()
    {
        unsigned currentPos = tellp();
        // Write the block size to the header
        seekp(blockBeginPos);
        *this << (unsigned int)(currentPos - blockBeginPos - sizeof(unsigned));
        // Go back to current position (after the block)
        seekp(currentPos);
    }

    // Writes z-lib compressed data preceded by the size
    void writeCompressedData(unsigned char* data, unsigned size)
    {
        // Invalid data - write length 0 and quit
        if (!data || !size) {
            *this << 0;
            return;
        }

        // Buffer size needed to compress the data
        mz_ulong allocSize = mz_compressBound(size);

        // Buffer already exists, but is too small...
        if (compressionBuffer && compressionBufferSize < allocSize) {
            delete[] compressionBuffer;
            compressionBuffer = 0;
        }

        // Need to allocate a compression buffer
        if (!compressionBuffer) {
            compressionBuffer = new (std::nothrow) unsigned char[allocSize];
            compressionBufferSize = compressionBuffer ? allocSize : 0;
        }

        // Successfully allocated buffer
        if (compressionBuffer) {
            // Perform compression and store actual size in allocSize
            mz_compress2(compressionBuffer, &allocSize, data, size, compressionLevel);

            // Write compressed size
            *this << allocSize;

            // Write compressed data
            write((char*)compressionBuffer, allocSize);
        }
        // Failed - write an empty buffer...
        else {
            *this << 0;
        }
    }

    // Write a binary block
    template <class T> TMOStream& operator<<(T i)
    {
        write(reinterpret_cast<const char*>(&i), sizeof(T));
        return *this;
    }

    // Write a character sequence
    inline TMOStream& operator<<(const char* str)
    {
        write(str, strlen(str));
        return *this;
    }

    // Write a string preceded by its length (char)
    void writeShortStr(const char* str)
    {
        put(strlen(str));
        *this << str;
    }

    // Write a string preceded by its length (int)
    void writeLongStr(const char* str)
    {
        *this << strlen(str);
        *this << str;
    }
};