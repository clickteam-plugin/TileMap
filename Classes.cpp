#include "Common.h"
#include "Helpers.h"

/*
 * Layer
 */

Layer::Layer(const Layer& src)
    : data(0), settings(src.settings), width(src.width), height(src.height)
{
    if (src.data) {
        data = new Tile[width * height];

        // Copy the source data
        memcpy(data, src.data, width * height * sizeof(Tile));
    }

    // Copy sub-layers...
    subLayers = src.subLayers;
}

// Resize layer, new tiles are empty
void Layer::resize(unsigned newWidth, unsigned newHeight)
{
    // Nothing to do...
    if (width == newWidth && height == newHeight)
        return;

    // Resize sub-layers accordingly
    std::vector<SubLayer>::iterator it = subLayers.begin();
    for (; it != subLayers.end(); ++it) {
        (*it).resize(width, height);
    }

    if (newWidth == 0 || newHeight == 0) {
        delete[] data;
        data = 0;
        width = 0;
        height = 0;
        return;
    }

    // Allocate a new array
    Tile* newData = new Tile[newWidth * newHeight];

    // Zero all tiles
    memset(newData, 0xff, newWidth * newHeight * sizeof(Tile));

    // Copy old data
    if (data) {
        // Get the size of the rectangle to copy
        int copyWidth = min(width, newWidth);
        int copyHeight = min(height, newHeight);

        // Copy the old tiles
        for (int x = 0; x < copyWidth; ++x) {
            for (int y = 0; y < copyHeight; ++y) {
                newData[x + y * newWidth] = data[x + y * width];
            }
        }

        // Delete the old array
        delete[] data;
    }

    // Assign the new array
    data = newData;

    width = newWidth;
    height = newHeight;
}

// For viewport rendering
int Layer::getScreenX(int cameraX)
{
    int pos = (int)((settings.offsetX - cameraX) * settings.scrollX);
    if (settings.wrapX) {
        if (pos > 0)
            pos %= getScreenWidth();
    }
    return pos;
}

// For viewport rendering
int Layer::getScreenY(int cameraY)
{
    int pos = (int)((settings.offsetY - cameraY) * settings.scrollY);
    if (settings.wrapY) {
        if (pos > 0)
            pos %= getScreenHeight();
    }
    return pos;
}

int Layer::getScreenWidth() { return width * settings.tileWidth; }

int Layer::getScreenHeight() { return height * settings.tileHeight; }

/*
 * SubLayer
 */

// Allocate layer with custom cell settings
SubLayer::SubLayer(unsigned cellSize, unsigned defaultValue)
    // cellShift calculation is accurate for cellSize in {1, 2, 4, 8} which is
    // all we need really
    : data(0),
      defaultValue(defaultValue),
      cellSize(cellSize),
      cellShift(min(cellSize / 2, 3))
{
}

SubLayer::SubLayer(const SubLayer& src)
    : data(0), defaultValue(src.defaultValue), cellShift(src.cellShift),
      cellSize(src.cellSize)
{
    width = src.width;
    height = src.height;

    if (src.data) {
        data = new unsigned char[(width * height) << cellShift];
        memcpy(data, src.data, (width * height) << cellShift);
    }
}

void SubLayer::resize(unsigned int newWidth, unsigned int newHeight)
{
    // Nothing to do...
    if (width == newWidth && height == newHeight)
        return;

    if (!newWidth || !newHeight) {
        delete[] data;
        data = 0;
        width = 0;
        height = 0;
        return;
    }

    unsigned char* newData = new unsigned char[(newWidth * newHeight) << cellShift];

    // Zero all tiles
    if (cellShift) {
        for (unsigned i = 0; i < newWidth * newHeight; ++i)
            memcpy(newData + (i << cellShift), &defaultValue, cellSize);
    }
    else
        memset(newData, defaultValue, newWidth * newHeight);

    // Copy old data
    if (data) {
        // Get the size of the rectangle to copy
        int copyWidth = min(width, newWidth);
        int copyHeight = min(height, newHeight);

        // Copy the old tiles (optimized for 1 byte cell size)
        if (cellShift)
            for (int x = 0; x < copyWidth; ++x)
                for (int y = 0; y < copyHeight; ++y)
                    memcpy(newData + ((x + y * newWidth) << cellShift),
                           data + ((x + y * width) << cellShift), cellSize);
        else
            for (int x = 0; x < copyWidth; ++x)
                for (int y = 0; y < copyHeight; ++y)
                    newData[x + y * newWidth] = data[x + y * width];

        // Delete the old array
        delete[] data;
    }

    // Assign the new array
    data = newData;

    width = newWidth;
    height = newHeight;
}