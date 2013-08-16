#pragma once
#include <vector>
#include "SubLayer.h"

class Layer
{
	// Tile data
	Tile*			data;
	
	// Tile count (map size)
	unsigned		width;
	unsigned		height;

public:

	// Sub-layers
	std::vector<SubLayer>	subLayers;

	// Scrolling offset in pixels
	int				offsetX;
	int				offsetY;

	// Scrolling coefficient for parallax
	float			scrollX;
	float			scrollY;

	// Wrapping
	bool			wrapX;
	bool			wrapY;

	// Rendering settings
	bool			visible;
	float			opacity;

	/* Tileset and collision indices */;
	unsigned char	tileset;
	unsigned char	collision;

	// Tile size (pixels)
	unsigned short	tileWidth;
	unsigned short	tileHeight;

	// Constructor/destructor
	Layer() : offsetX(0), offsetY(0), scrollX(1.0), scrollY(1.0), wrapX(false), wrapY(false),
		visible(true), opacity(1.0), tileset(0), collision(-1),
		tileWidth(16), tileHeight(16), width(0), height(0), data(0)
	{
	}

	Layer(const Layer& src) : data(0)
	{
		offsetX = src.offsetX;
		offsetY = src.offsetY;
		scrollX = src.scrollX;
		scrollY = src.scrollY;
		wrapX = src.wrapX;
		wrapY = src.wrapY;
		width = src.width;
		height = src.height;
		tileWidth = src.tileWidth;
		tileHeight = src.tileHeight;
		visible = src.visible;
		opacity = src.opacity;
		tileset = src.tileset;
		collision = src.collision;

		if (src.data)
		{
			data = new Tile[width * height];
			if (!data)
			{
				width = 0;
				height = 0;
				return;
			}

			// Copy the source data
			memcpy(data, src.data, width * height * sizeof(Tile));
		}

		// Copy sub-layers...
		subLayers = src.subLayers;
	}

	~Layer()
	{
		delete[] data;
	}

	// Check if a layer is usable
	inline bool isValid() const
	{
		return width > 0 && height > 0 && tileWidth > 0 && tileHeight > 0 && data != 0;
	}

	// Check if a coordinate is valid
	inline bool isValid(unsigned x, unsigned y) const
	{
		return isValid() && x < width && y < height;
	}

	// Get a tile within the layer array
	inline Tile* getTile(unsigned x, unsigned y)
	{
		return data + x + width*y;
	}

	inline Tile* getDataPointer()
	{
		return data;
	}

	inline unsigned getWidth() const
	{
		return width;
	}

	inline unsigned getHeight() const
	{
		return height;
	}

	inline unsigned getByteSize() const
	{
		return width * height * sizeof(Tile);
	}

	// Resize layer, new tiles are empty
	void resize(unsigned newWidth, unsigned newHeight)
	{
		// Nothing to do...
		if (width == newWidth && height == newHeight)
			return;

		if (newWidth == 0 || newHeight == 0)
		{
			delete[] data;
			data = 0;
			width = 0;
			height = 0;
			return;
		}

		// Allocate a new array
		Tile* newData = new Tile[newWidth*newHeight];
		if (!newData)
		{
			width = 0;
			height = 0;
			return;
		}

		// Zero all tiles
		memset(newData, 0xff, sizeof(Tile)*newWidth*newHeight);

		// Copy old data
		if (data)
		{
			// Get the size of the rectangle to copy
			int copyWidth = min(width, newWidth);
			int copyHeight = min(height, newHeight);

			// Copy the old tiles
			for (int x = 0; x < copyWidth; ++x)
			{
				for (int y = 0; y < copyHeight; ++y)
				{
					newData[x+y*newWidth] = data[x+y*width];
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
	inline int getScreenX(int cameraX)
	{
		return (int)((offsetX - cameraX) * scrollX);
	}

	// For viewport rendering
	inline int getScreenY(int cameraY)
	{
		return (int)((offsetY - cameraY) * scrollY);
	}
};
