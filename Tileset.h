#include "HWASurface.h"

struct Tileset
{
	char			path[256];
	cSurface*		surface;
	cSurface*		texture;
	COLORREF		transpCol;

	Tileset() : surface(0), texture(0), transpCol(0xFF00FF)
	{
		path[0] = 0;
	}

	~Tileset() { delete surface; }

	Tileset(const Tileset& src)
	{
		int n = min(254, strlen(src.path));
		strncpy(path, src.path, n);
		path[n] = 0;
		transpCol = src.transpCol;
		
		if(src.surface)
		{
			surface = new cSurface;
			surface->Clone(*src.surface);
			texture = 0;
			updateTexture();
		}
		else
		{
			surface = 0;
			texture = 0;
		}
	}

	inline bool isValid()
	{
		return surface != 0 && surface->IsValid();
	}

	/* For HWA: Need texture and bitmap surface... */
	void updateTexture()
	{
#ifdef HWABETA
		if (texture)
		{
			delete texture;
			texture = 0;
		}

		if (surface)
		{
			texture = new cSurface;
			texture->Create(surface->GetWidth(), surface->GetHeight(), getPrototype(surface->GetDepth(), true));
			copyBlit(*surface, *texture);
			texture->SetTransparentColor(surface->GetTransparentColor());
		}
#else
		texture = surface;
#endif
	}

};