#pragma once

class rRundata;
typedef rRundata * LPRRDATA;

#include <list>
#include <vector>
#include <map>
#include <string>

#include "Tileset.h"
#include "Tile.h"
#include "Layer.h"

// Tileset path mode
enum TSPMODE
{
	TSPM_UNTREATED, // The path is taken as-is
	TSPM_APP_PATH,	// The path is relative to the exe file
	TSPM_MAP_PATH,	// The path is relative to the TileMap file it is referenced in
	TSPM_USER_PATH,	// The path is relative to a user-specified path
	TSPM_CUSTOM,	// The path is an identifier that cannot be loaded directly from the disk
};

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
	map<string, Property>*properties;

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

	// Location of the root folder of tilesets (saved tileset paths will be relative to this)
	TSPMODE			tilesetPathMode;
	char			tilesetUserPath[256];
	char			appPath[256]; // Used for APP_PATH mode

	// Property iterator
	const char*		onProperty;

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

enum ANIMMODE
{
	AM_LOOP,
	AM_PINGPONG,
	AM_ONCE,
	// ...
};

struct Animation
{
	// Tiles per tick/sec
	double		speed;

	// Whether to traverse columns before rows
	bool		columnMajor;

	// Width of the tile source box
	unsigned	width;

	// Height of the tile source box
	unsigned	height;

	// In what order to play the frames
	ANIMMODE	mode;
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

	// Animation info table
	DWORD			lastTick;
	int				animMode;
	double			animTime;
	Animation		anim[255];

	// Overlaps condition
	Tileset*		cndTileset;
	cSurface*		cndAlphaSurf;

	// On collision
	Tile			collTile;

	// Collision margin
	RECT collMargin;

	struct
	{
		// Configuration
		bool			use;

		// On layer
		LayerSettings*	settings;

		// Sub-layer linkage
		struct
		{
			unsigned char tileset;
			unsigned char scaleX;
			unsigned char scaleY;
			unsigned char angle;
			unsigned char animation;

		} link;

		// Get-only
		unsigned		index;

	} layerCallback;

	// Callback for rendering
	struct
	{
		// Configuration
		bool			use;
		int				borderX;
		int				borderY;

		// On tile
		Tile*			tile;
		TileSettings*	settings;

		// Get-only
		int				x;
		int				y;
	} tileCallback;

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
