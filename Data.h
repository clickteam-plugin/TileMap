class rRundata;
typedef rRundata * LPRRDATA;

// --------------------------------
// RUNNING OBJECT DATA STRUCTURE
// --------------------------------
// If you want to store anything between actions/conditions/expressions
// you should store it here

#include <list>
using namespace std;
#include "Tileset.h"
#include "Tile.h"
#include "Layer.h"

struct TMAPVIEW;
typedef struct tagRDATA
{
	headerObject	rHo;					// Header
	rVal			rv;
	LPRRDATA		rRd;

	/* Depth for the tilesets (matches frame surface for max speed) */
	int				depth;

	vector<Layer>*	layers;
	vector<Tileset>*tilesets;

	/* Current stuff */
	Layer*			currentLayer;
	Tileset*		currentTileset;
	
	/* Size of a tile in pixels */
	unsigned short	tileWidth;
	unsigned short	tileHeight;

	/* Attached viewports */
	list<TMAPVIEW*>*viewports;
	bool			redraw;

	/* Compression level 0-10 for saving */
	char			compress;
	int				blocks;

	/* Pen used for more flexible tile drawing */
	struct
	{
		/* General */
		int				x;
		int				y;
		unsigned int	width;
		unsigned int	height;

		/* Drawing */
		TileRange		tiles;
		unsigned char	patternX;
		unsigned char	patternY;

	}				cursor;
	
} RUNDATA;
typedef	RUNDATA	* LPRDATA;

struct TMAPVIEW
{
	//#include "MagicRDATA.h"
	headerObject	rHo;					// Header
	rCom			rc;
	rMvt			rm;
	rSpr			rs;
	rVal			rv;
	LPRRDATA		rRd;

	/* Parent Tile Map */
	RUNDATA*		p;

	/* Depth of the drawing surface */
	/* We use the same depth for all tilesets */
	/* To avoid conversion and ensure maximum speed */
	int				depth;

	/* Scrolling center position */
	int				cameraX;
	int				cameraY;
	bool			autoScroll;

	/* Display surface */
	bool			transparent;
	COLORREF		background;
	cSurface*		surface;
	
};

// --------------------------------
// EDITION OF OBJECT DATA STRUCTURE
// --------------------------------
// These values let you store data in your extension that will be saved in the MFA
// You should use these with properties

#define TILESETCOUNT 16

typedef struct tagEDATA_V1
{
	extHeader		eHeader;

	/* Map */
	unsigned short	tileWidth;
	unsigned short	tileHeight;

	WORD			tilesets[TILESETCOUNT];
	char			tilesetCount : 5;
	bool			blockMap : 1;
	bool			blockLayers : 1;
	bool			blockTilesets : 1;

} EDITDATA;
typedef EDITDATA * LPEDATA;




typedef struct tagSURFACE
{
headerObject rHo;
rCom rc;
rMvt rm;
rSpr rs;
rVal rv;
LPRRDATA rRd;
//Image bank and important IDs
vector<cSurface*>*          surf;
cSurface*                   target;
short                       targetId;
bool                        targetValid;
cSurface*                   current;
short                       currentId;
short                       lastId;
//Functions
cSurface*                   (*imageAt)(tagSURFACE*,int);
int                         (*imageCount)(tagSURFACE*);
} SURFACE;
