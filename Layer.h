#pragma once

#include <map>
#include <new>
#include <string>
#include <vector>

using std::vector;
using std::map;
using std::string;

#include "Property.h"
#include "SubLayer.h"
#include "Tile.h"

struct SubLayerLink {
    unsigned char tileset;
    unsigned char animation;
    unsigned char animationFrame;

    SubLayerLink() : tileset(0xff), animation(0xff), animationFrame(0xff) {}
};

struct LayerSettings {
    // Scrolling offset in pixels
    int offsetX;
    int offsetY;

    // Scrolling coefficient for parallax
    float scrollX;
    float scrollY;

    // Wrapping
    bool wrapX;
    bool wrapY;

    // Rendering settings
    bool visible;
    float opacity;

    /* Tileset and collision indices */
    unsigned char tileset;
    unsigned char collision;

    // Tile size (pixels)
    unsigned short tileWidth;
    unsigned short tileHeight;

    // Sub-layer link indices
    SubLayerLink subLayerLink;

    LayerSettings()
        : offsetX(0), offsetY(0), scrollX(1.0), scrollY(1.0), wrapX(false), wrapY(false),
          visible(true), opacity(1.0), tileset(0), collision(-1), tileWidth(16), tileHeight(16)
    {
    }
};

class Layer {
    // Tile data
    Tile * data;

    // Tile count (map size)
    unsigned width;
    unsigned height;

  public:
    // Modiyable layer settings
    LayerSettings settings;

    // User properties
    map<string, Property> properties;

    // Sub-layers
    vector<SubLayer> subLayers;

    // Constructor/destructor
    Layer() : width(0), height(0), data(0) {}

    Layer(const Layer & src);

    ~Layer() { delete[] data; }

    // Check if a layer is usable
    bool isValid() const { return width > 0 && height > 0 && data != 0; }

    // Check if a coordinate is valid
    bool isValid(unsigned x, unsigned y) const
    {
        return isValid() && x < width && y < height;
    }

    // Get a tile within the layer array
    Tile * getTile(unsigned x, unsigned y) { return data + x + width * y; }

    Tile * getDataPointer() { return data; }

    unsigned getWidth() const { return width; }

    unsigned getHeight() const { return height; }

    unsigned getByteSize() const { return width * height * sizeof(Tile); }

    // Resize layer, new tiles are empty
    void resize(unsigned newWidth, unsigned newHeight);

    // For viewport rendering
    float getScreenX(float cameraX);
    float getScreenY(float cameraY);
    int getScreenWidth();
    int getScreenHeight();
};
