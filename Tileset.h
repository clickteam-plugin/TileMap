struct Tileset
{
	char			path[256];
	cSurface*		surface;
	COLORREF		transpCol;

	/* Constructor/destructor */
	Tileset() : surface(0), transpCol(0xFF00FF)
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
		}
		else
		{
			surface = 0;
		}
	}

	/* Check if a tileset is usable */
	inline bool isValid()
	{
		return surface != 0 && surface->IsValid();
	}
};