#pragma once

class rRundata;
typedef rRundata * LPRRDATA;

#include <list>
using namespace std;
#include "Tileset.h"
#include "Tile.h"
#include "Layer.h"

struct TMAPVIEW;
struct TILEMAP
{
	headerObject	rHo;
	rVal			rv;
	LPRRDATA		rRd;

	// Depth for the tilesets (matches frame surface for max speed)
	int				depth;

	vector<Layer>*	layers;
	vector<Tileset>*tilesets;

	// Current stuff
	Layer*			currentLayer;
	Tileset*		currentTileset;
	
	// Size of a tile in pixels
	unsigned short	tileWidth;
	unsigned short	tileHeight;

	// Attached viewports
	list<TMAPVIEW*>*viewports;
	bool			redraw;

	// Compression level 0-10 for saving
	char			compress;
	int				blocks;

	// Pen used for more flexible tile drawing
	struct
	{
		// General
		int				x;
		int				y;
		unsigned int	width;
		unsigned int	height;

		// Drawing
		TileRange		tiles;
		unsigned char	patternX;
		unsigned char	patternY;

	}				cursor;
	
};

struct TMAPVIEW
{
	headerObject	rHo;					// Header
	rCom			rc;
	rMvt			rm;
	rSpr			rs;
	rVal			rv;
	LPRRDATA		rRd;

	// Parent Tile Map
	TILEMAP*		p;

	// Depth of the drawing surface
	// We use the same depth for all tilesets
	// To avoid conversion and ensure maximum speed
	int				depth;

	// Scrolling center position
	int				cameraX;
	int				cameraY;
	bool			autoScroll;

	// Layer boundaries to draw
	unsigned		minLayer;
	unsigned		maxLayer;

	// Misc. settings
	bool			outsideColl;
	bool			fineColl;

	// Display surface
	bool			transparent;
	COLORREF		background;
	cSurface*		surface;
	bool			accurateClip;

	// Overlaps condition
	Tileset*		cndTileset;
	cSurface*		cndAlphaSurf;

	// On collision
	Tile			collTile;

	// Collision margin
	RECT collMargin;

	// Callback for rendering
	struct
	{
		bool		use;

		// Additional tile render border
		int			borderX;
		int			borderY;

		// Data that can be modified
		bool		visible;
		float		opacity;
		int			offsetX;
		int			offsetY;
		Tile*		tile;

		// HWA specific
		COLORREF	tint;
		bool		transform;
		float		scaleX;
		float		scaleY;
		float		angle;

		// Get-only (for now?)
		int			x;
		int			y;

	}				callback;
};

typedef struct tagSURFACE
{
	headerObject rHo;
	rCom rc;
	rMvt rm;
	rSpr rs;
	rVal rv;
	LPRRDATA rRd;
	// Image bank and important IDs
	vector<cSurface*>*          surf;
	cSurface*                   target;
	short                       targetId;
	bool                        targetValid;
	cSurface*                   current;
	short                       currentId;
	short                       lastId;
	// Functions
	cSurface*                   (*imageAt)(tagSURFACE*,int);
	int                         (*imageCount)(tagSURFACE*);
} SURFACE;
