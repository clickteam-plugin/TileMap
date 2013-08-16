#pragma once

/*
	TODO: Should probably make Layer & SubLayer inherit from a generic class
*/

class SubLayer
{
	// Raw data (may be reinterpreted)
	unsigned char*	data;

	// The power of two to use for the data cell size (0 = 1 byte).
	unsigned int	cellShift;

	// Tile count, should always be the same as the parent's
	unsigned int	width;
	unsigned int	height;

public:

	SubLayer() : data(0), cellShift(0)
	{
	}

	SubLayer(const SubLayer& src) : data(0)
	{
		cellShift = src.cellShift;
		width = src.width;
		height = src.height;

		if (src.data)
		{
			if (data = new unsigned char[(width * height) << cellShift])
				memcpy(data, src.data, (width * height) << cellShift);
		}
	}

	~SubLayer()
	{
		delete[] data;
	}

	// Check if a layer is usable
	inline bool isValid() const
	{
		return width > 0 && height > 0 && data != 0;
	}

	// Check if a coordinate is valid
	inline bool isValid(unsigned int x, unsigned int y) const
	{
		return isValid() && x < width && y < height;
	}

	// Get a tile within the layer array
	inline unsigned char* getCell(unsigned int x = 0, unsigned int y = 0)
	{
		return data + ((x + width * y) << cellShift);
	}

	inline unsigned int getWidth() const
	{
		return width;
	}

	inline unsigned int getHeight() const
	{
		return height;
	}

	inline unsigned int getByteSize() const
	{
		return (width * height) << cellShift;
	}

	// Ensure that the sub-layer conforms with its parent's requirements
	void resize(unsigned int newWidth, unsigned int newHeight)
	{
		// Nothing to do...
		if (width == newWidth && height == newHeight)
			return;

		if (!newWidth || !newHeight)
		{
			delete[] data;
			data = 0;
			width = 0;
			height = 0;
			return;
		}

		// Allocate a new array
		unsigned char* newData = new unsigned char[newWidth * newHeight];
		if (!newData)
		{
			width = 0;
			height = 0;
			return;
		}

		// Zero all tiles
		memset(newData, 0, (newWidth * newHeight) << cellShift);

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
					newData[(x + y * newWidth) << cellShift] = data[(x + y * width) << cellShift];
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
};