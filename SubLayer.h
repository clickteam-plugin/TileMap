#pragma once

/*
	TODO: Should probably make Layer & SubLayer inherit from a generic class
*/

class SubLayer
{
	// The power of two to use for the data cell size (0 = 1 byte).
	unsigned		cellShift;

	// Size of a single cell in bytes.
	char			cellSize;

	// The value used for empty/new tiles.
	unsigned		defaultValue;

	// Raw data (may be reinterpreted)
	unsigned char*	data;

	// Tile count, should always be the same as the parent's
	unsigned int	width;
	unsigned int	height;

public:

	SubLayer()
		:	data(0), cellShift(0), cellSize(1), defaultValue(0)
	{
	}

	// Allocate layer with custom cell settings
	SubLayer(unsigned cellSize, unsigned defaultValue)
		// cellShift calculation is accurate for cellSize in {1, 2, 4, 8} which is all we need really
		:	data(0), defaultValue(defaultValue), cellSize(cellSize), cellShift(min(cellSize / 2, 3))
	{
	}

	SubLayer(const SubLayer& src)
		:	data(0), defaultValue(src.defaultValue), cellShift(src.cellShift), cellSize(src.cellSize)
	{
		width = src.width;
		height = src.height;

		if (src.data)
		{
			if (data = new (std::nothrow) unsigned char[(width * height) << cellShift])
				memcpy(data, src.data, (width * height) << cellShift);
		}
	}

	~SubLayer()
	{
		delete[] data;
	}

	inline char getCellSize() const
	{
		return cellSize;
	}

	inline unsigned getDefaultValue() const
	{
		return defaultValue;
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
	inline unsigned char* getCell(unsigned x, unsigned int y)
	{
		return data + ((x + width * y) << cellShift);
	}

	template <class T>
	inline void getCellSafe(unsigned x, unsigned y, T* target)
	{
		unsigned maxSize = min(sizeof(*target), cellSize);

		memcpy(target, getCell(x, y), maxSize);
	}

	template <class T>
	T getCellAs(unsigned x, unsigned y)
	{
		return *reinterpret_cast<T*>(getCell(x, y));
	}

	inline void setCell(unsigned x, unsigned y, unsigned value)
	{
		memcpy(getCell(x, y), &value, cellSize);
	}

	inline unsigned int getWidth() const
	{
		return width;
	}

	inline unsigned int getHeight() const
	{
		return height;
	}

	inline unsigned char* getDataPointer()
	{
		return data;
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
		unsigned char* newData = new (std::nothrow) unsigned char[(newWidth * newHeight) << cellShift];
		if (!newData)
		{
			width = 0;
			height = 0;
			return;
		}

		// Zero all tiles
		if (cellShift)
		{
			for (unsigned i = 0; i < newWidth * newHeight; ++i)
				memcpy(newData + (i << cellShift), &defaultValue, cellSize);
		}
		else
			memset(newData, defaultValue, newWidth * newHeight);

		// Copy old data
		if (data)
		{
			// Get the size of the rectangle to copy
			int copyWidth = min(width, newWidth);
			int copyHeight = min(height, newHeight);

			// Copy the old tiles (optimized for 1 byte cell size)
			if (cellShift)
			{
				for (int x = 0; x < copyWidth; ++x)
				{
					for (int y = 0; y < copyHeight; ++y)
					{
						memcpy(newData + ((x + y * newWidth) << cellShift), data + ((x + y * width) << cellShift), cellSize);
					}
				}
			}
			else
			{
				for (int x = 0; x < copyWidth; ++x)
				{
					for (int y = 0; y < copyHeight; ++y)
					{
						newData[x + y * newWidth] = data[x + y * width];
					}
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