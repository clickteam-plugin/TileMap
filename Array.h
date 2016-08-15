#pragma once

template <class Type> class Array {
    // Value for new cells
    static char DefaultValue;

    // Type data & size
    Type * data;
    unsigned int width;
    unsigned int height;
    unsigned int cellSize;

  public:
    // Clone data from an equivalent array
    void clone(const Array<Type> & src)
    {
        if (data)
            delete[] data;

        width = src.width;
        height = src.height;
        data = new Type[width * height];

        if (data)
            memcpy(data, src.data, cellSize * width * height);
        else {
            width = 0;
            height = 0;
        }
    }

    // Check if a layer is usable
    inline bool isValid() const { return width > 0 && height > 0 && data != 0; }

    // Check if a coordinate is valid
    inline bool isValid(unsigned int x, unsigned int y) const
    {
        return isValid() && x < width && y < height;
    }

    inline unsigned int getWidth() const { return width; }

    inline unsigned int getHeight() const { return height; }

    // Get a Type within the layer array
    inline Type * get(unsigned int x = 0, unsigned int y = 0)
    {
        return data + x + width * y;
    }

    // Resize layer, new Types are empty
    void resize(unsigned int newWidth, unsigned int newHeight)
    {
        // Nothing to do...
        if (width == newWidth && height == newHeight)
            return;

        if (newWidth == 0 || newHeight == 0) {
            delete[] data;
            data = 0;
            width = 0;
            height = 0;
            return;
        }

        // Allocate a new array
        Type * newData = new Type[newWidth * newHeight];
        if (!newData) {
            width = 0;
            height = 0;
            return;
        }

        // Zero all Types
        memset(newData, V, sizeof(Type) * newWidth * newHeight);

        // Copy old data
        if (data) {
            // Get the size of the rectangle to copy
            int copyWidth = min(width, newWidth);
            int copyHeight = min(height, newHeight);

            // Copy the old Types
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
};
