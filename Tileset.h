#pragma once
#include "HWASurface.h"

class Tileset {
    char path[256];

  public:
    cSurface * surface;
    cSurface * texture;
    COLORREF transpCol;
	int renderMode;

    Tileset() : surface(0), texture(0), transpCol(0xFF00FF), renderMode(SD_DIB) { path[0] = 0; }

    ~Tileset()
    {
        delete surface;
#ifdef HWABETA
        delete texture;
#endif
    }

    Tileset(const Tileset & src) : surface(0), texture(0)
    {
        int n = min(254, strlen(src.path));
        strncpy(path, src.path, n);
        path[n] = 0;
        transpCol = src.transpCol;

        if (src.surface) {
            surface = new cSurface;
            surface->Clone(*src.surface);
            texture = 0;
            updateTexture();
        }
    }

    inline bool isValid() { return surface != 0 && surface->IsValid(); }

    // For HWA: Need texture and bitmap surface...
    void updateTexture()
    {
#ifdef HWABETA
        if (texture) {
            delete texture;
            texture = 0;
        }

        if (surface) {
            texture = new cSurface;
            texture->Create(surface->GetWidth(), surface->GetHeight(),
					getPrototype(surface->GetDepth(), renderMode));
            copyBlit(*surface, *texture);
            texture->SetTransparentColor(surface->GetTransparentColor());
        }
#else
        texture = surface;
#endif
    }

	void setRenderMode(int mode)
	{
		renderMode = mode;
	}

    void setPathFromRelative(const char * sourcePath, const char * relativePath)
    {
        // Try to make this path absolute
        if (!PathCombine(path, sourcePath, relativePath)) {
            // Last chance: Copy path directly
            strcpy_s(path, 256, relativePath);
        }
    }

    void setPath(const char * source) { strcpy_s(path, 256, source); }

    const char * getPath() const { return (const char *)&path[0]; }

    bool getPathRelativeTo(const char * sourcePath, char * outputBuffer, bool sourceIsFile = false) const
    {
        return outputBuffer &&
               PathRelativePathTo(outputBuffer, sourcePath,
                                  sourceIsFile ? 0 : FILE_ATTRIBUTE_DIRECTORY, path, 0);
    }
};