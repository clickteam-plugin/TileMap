#include "Rundata.h"

typedef TILEMAP RUNDATA;
typedef RUNDATA * LPRDATA;

#define TILESETCOUNT 16
typedef struct tagEDATA_V1 {
    extHeader eHeader;

    /* Map */
    unsigned short tileWidth;
    unsigned char tileHeight; // Split up so we don't need to resize the editdata
    unsigned char tilesetPathMode;

    WORD tilesets[TILESETCOUNT];
    char tilesetCount : 5;
    bool blockMap : 1;
    bool blockLayers : 1;
    bool blockTilesets : 1;

} EDITDATA;
typedef EDITDATA * LPEDATA;