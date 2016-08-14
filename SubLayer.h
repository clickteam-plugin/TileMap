#pragma once

/*
        TODO: Should probably make Layer & SubLayer inherit from a generic class
*/

class SubLayer {
    // The power of two to use for the data cell size (0 = 1 byte).
    unsigned cellShift;

    // Size of a single cell in bytes.
    char cellSize;

    // The value used for empty/new tiles.
    unsigned defaultValue;

    // Raw data (may be reinterpreted)
    unsigned char* data;

    // Tile count, should always be the same as the parent's
    unsigned int width;
    unsigned int height;

  public:
    SubLayer() : data(0), cellShift(0), cellSize(1), defaultValue(0) {}

    SubLayer(unsigned cellSize, unsigned defaultValue);
    SubLayer(const SubLayer& src);

    ~SubLayer() { delete[] data; }

    char getCellSize() const { return cellSize; }

    unsigned getDefaultValue() const { return defaultValue; }

    // Check if a layer is usable
    bool isValid() const { return width > 0 && height > 0 && data != 0; }

    // Check if a coordinate is valid
    bool isValid(unsigned int x, unsigned int y) const
    {
        return isValid() && x < width && y < height;
    }

    // Get a tile within the layer array
    __forceinline unsigned char* getCell(unsigned x, unsigned int y) const
    {
        return data + ((x + width * y) << cellShift);
    }

    template <class T>
    __forceinline void getCellSafe(unsigned x, unsigned y, T* target) const
    {
        unsigned maxSize = min(sizeof(*target), cellSize);

        memcpy(target, getCell(x, y), maxSize);
    }

    template <class T> __forceinline T getCellAs(unsigned x, unsigned y)
    {
        return *reinterpret_cast<T*>(getCell(x, y));
    }

    __forceinline void setCell(unsigned x, unsigned y, unsigned value)
    {
        memcpy(getCell(x, y), &value, cellSize);
    }

    unsigned int getWidth() const { return width; }

    unsigned int getHeight() const { return height; }

    unsigned char* getDataPointer() { return data; }

    unsigned int getByteSize() const { return (width * height) << cellShift; }

    // Ensure that the sub-layer conforms with its parent's requirements
    void resize(unsigned int newWidth, unsigned int newHeight);
};