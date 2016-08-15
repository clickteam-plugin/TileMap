#pragma once

class rRundata;
typedef rRundata * LPRRDATA;

#include <list>
#include <map>
#include <string>
#include <vector>

using std::list;
using std::vector;
using std::map;
using std::string;

#include "Layer.h"
#include "Tile.h"
#include "Tileset.h"

// Simple rect structure with shorter member names than Windows' LEFT, BOTTOM,
// etc.
struct Rect {
    int x1, y1, x2, y2;

    void moveBy(int x, int y)
    {
        x1 += x;
        y1 += y;
        x2 += x;
        y2 += y;
    }

    void moveTo(int x, int y)
    {
        x2 += x - x1;
        y2 += y - y1;
        x1 = x;
        y1 = y;
    }
};

// Tileset path mode
enum TSPMODE {
    TSPM_UNTREATED, // The path is taken as-is
    TSPM_APP_PATH,  // The path is relative to the exe file
    TSPM_MAP_PATH,  // The path is relative to the TileMap file it is referenced
                    // in
    TSPM_USER_PATH, // The path is relative to a user-specified path
    TSPM_CUSTOM,    // The path is an identifier that cannot be loaded directly
                    // from the disk
};

struct TMAPVIEW;
struct TILEMAP {
    headerObject rHo;
    rVal rv;
    LPRRDATA rRd;

    // Stable ABI for STL containers
    std::size_t (*getLayerCount)(TILEMAP *);
    Layer * (*getLayerAt)(TILEMAP *, std::size_t);
    std::size_t (*getLayerSubLayerCount)(Layer *);
    SubLayer * (*getLayerSubLayerAt)(Layer *, std::size_t);
    void * _reserved[6];

    Layer * currentLayer;
    Tileset * currentTileset;

    // Stupid std containers that don't have a stable ABI
    std::vector<Layer> * layers;
    std::vector<Tileset> * tilesets;
    std::map<string, Property> * properties;

    /// What follows is mostly cold data

    // Attached viewports
    list<TMAPVIEW *> * viewports;
    bool redraw;

    // Default size of a tile in pixels
    unsigned short tileWidth;
    unsigned short tileHeight;

    // Depth for the tilesets (matches frame surface for max speed)
    int depth;

    // Compression level 0-10 for saving
    char compress;
    int blocks;

    // Location of the root folder of tilesets (saved tileset paths will be
    // relative to this)
    TSPMODE tilesetPathMode;
    char tilesetUserPath[256];
    char appPath[256]; // Used for APP_PATH mode

    // Property iterator
    const char * onProperty;

    // Pen used for more flexible tile drawing
    struct {
        // General
        int x;
        int y;
        unsigned int width;
        unsigned int height;

        // Drawing
        TileRange tiles;
        unsigned char patternX;
        unsigned char patternY;

    } cursor;
};

enum ANIMMODE {
    AM_LOOP,
    AM_PINGPONG,
    AM_ONCE,
    // ...
};

struct Animation {
    // Tiles per tick/sec
    double speed;

    // Whether to traverse columns before rows
    bool columnMajor;

    // Width of the tile source box
    unsigned width;

    // Height of the tile source box
    unsigned height;

    // In what order to play the frames
    ANIMMODE mode;

    void apply(Tile & tile, double time = 0.0, int frameOffset = 0);
};

enum OFTYPE {
    OFT_SUBLAYER,
    OFT_TILESETX,
    OFT_TILESETY,
    OFT_TILESETRANGE,
};

struct OVERLAPFLT {
    OFTYPE type;
    unsigned param;
    int value;
};

struct TileSettings {
    bool visible;
    float opacity;
    int offsetX;
    int offsetY;
    BYTE tileset;
    BYTE animation;

    // HWA specific
    COLORREF tint;
    bool transform;
    float scaleX;
    float scaleY;
    float angle;

    TileSettings()
        : visible(true), opacity(1.0f), offsetX(0), offsetY(0), tint(WHITE),
          transform(false), scaleX(1.0f), scaleY(1.0f), angle(0.0f), animation(0)
    {
    }
};

struct TMAPVIEW {
    headerObject rHo; // Header
    rCom rc;
    rMvt rm;
    rSpr rs;
    rVal rv;
    LPRRDATA rRd;

    bool isHwa;
    bool isUnicode;

    // Parent Tile Map
    TILEMAP * p;

    // Depth of the drawing surface. We use the same depth for all tilesets to
    // avoid conversion and ensure maximum speed
    int depth;

    // Scrolling center position
    float zoom;
    float zoomPointX;
    float zoomPointY;
    int cameraX;
    int cameraY;
    bool autoScroll;

    // Layer boundaries to draw
    unsigned minLayer;
    unsigned maxLayer;

    // Misc. settings
    bool outsideColl;
    bool fineColl;
    bool zoomColl;

    // Display surface
    bool transparent;
    COLORREF background;
    cSurface * surface;
    bool accurateClip;
    DWORD blitFlags;

    // Animation info table
    DWORD lastTick;
    int animMode;
    double animTime;
    Animation anim[255];

    // Overlaps condition
    Tileset * cndTileset;
    cSurface * cndAlphaSurf;
    Layer * cndLayer;

    // On collision
    Tile collTile;

    // Collision margin
    RECT collMargin;

    // Overlap condition filter
    OVERLAPFLT ovlpFilters[8];
    unsigned ovlpFilterCount;
    const SubLayer * sublayerCache[16];

    struct {
        // Configuration
        bool use;

        // On layer
        LayerSettings * settings;

        // Sub-layer linkage
        struct {
            unsigned char tileset;
            unsigned char scaleX;
            unsigned char scaleY;
            unsigned char angle;
            unsigned char animation;

        } link;

        // Get-only
        unsigned index;

    } layerCallback;

    // Callback for rendering
    struct {
        // Configuration
        bool use;
        int borderX;
        int borderY;

        // On tile
        Tile * tile;
        TileSettings * settings;

        // Get-only
        int x;
        int y;
    } tileCallback;
};

typedef struct tagSURFACE {
    headerObject rHo;
    rCom rc;
    rMvt rm;
    rSpr rs;
    rVal rv;
    LPRRDATA rRd;
    // Image bank and important IDs
    vector<cSurface *> * surf;
    cSurface * target;
    short targetId;
    bool targetValid;
    cSurface * current;
    short currentId;
    short lastId;
    // Functions
    cSurface * (*imageAt)(tagSURFACE *, int);
    int (*imageCount)(tagSURFACE *);
} SURFACE;
