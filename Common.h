// Include guard
#ifndef COMMON_H
#define COMMON_H

// Suppress the deprecated warnings for VC2005
#define _CRT_SECURE_NO_WARNINGS

// General includes
#include	"TemplateInc.h"


#ifdef HWABETA
#define SURF_DRIVER SD_D3D9
#define SURF_TYPE ST_HWA_RTTEXTURE
#define BMODE_HWA BMODE_TRANSP
#else
#define SURF_DRIVER SD_DIB
#define SURF_TYPE ST_MEMORY
#define BMODE_HWA BMODE_OPAQUE
#endif


#define MAXTILESETS 100

// Specific to this extension
#include	<vector>
//#include	"tinyxml/tinyxml.h"
//#include	<b64/cencode.h>
//#include	<b64/cdecode.h>
#include <fstream>
using namespace std;

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include	"Resource.h"
#include	"FlagsPrefs.h"
#include	"Information.h"
#include	"ImageFlt.h"
#include	"ImgFlt.h"
#include	"CfcFile.h"
#include	"Data.h"

enum
{
	BLOCK_MAP = 1,
	BLOCK_LAYERS = 2,
	BLOCK_TILESETS = 4,
};

// rTemplate include
#include	"rTemplate.h"

inline float __getFloat(LPRDATA rdPtr) { int foo = CNC_GetFloatParameter(rdPtr); return *(float*)&foo; }

#define intParam() CNC_GetIntParameter(rdPtr)
#define strParam() (const char*)CNC_GetStringParameter(rdPtr)
#define fltParam() __getFloat(rdPtr)
#define objParam() CNC_GetParameter(rdPtr)
#define anyParam() CNC_GetParameter(rdPtr)

// Globals and prototypes
extern HINSTANCE hInstLib;
extern short * conditionsInfos;
extern short * actionsInfos;
extern short * expressionsInfos;
extern long (WINAPI ** ConditionJumps)(LPRDATA rdPtr, long param1, long param2);
extern short (WINAPI ** ActionJumps)(LPRDATA rdPtr, long param1, long param2);
extern long (WINAPI ** ExpressionJumps)(LPRDATA rdPtr, long param);
extern PropData Properties[];
extern WORD DebugTree[];

// End include guard
#endif


		///* Local to global tileset conversion table */
		//unsigned char tilesetsUsed = 128;
		//for(int i = 127; i > 0; --i)
		//{
		//	if(layer->tilesets[i] != -1)
		//		break;

		//	tilesetsUsed--;
		//}
		//fwrite(&tilesetsUsed, sizeof(char), 1, file);
		//fwrite(&layer->tilesets[0], tilesetsUsed*sizeof(short), 1, file);

						///* Local to global tileset conversion table */
						//unsigned char tilesetsUsed;
						//fread(&tilesetsUsed, sizeof(char), 1, file);
						//fread(&layer->tilesets[0], tilesetsUsed*sizeof(short), 1, file);

	///* For maximum speed, we copy the tilesets into a local array */
	///* That way we don't need to check if a tileset is within the range too */
	//unsigned int tilesetCount = rdPtr->tilesets->size();
	//if(tilesetCount > MAXTILESETS)
	//	tilesetCount = MAXTILESETS;
	//Tileset* tilesets[MAXTILESETS] = {0};
	//for(unsigned int i = 0; i < tilesetCount; ++i)
	//	tilesets[i] = &(*rdPtr->tilesets)[i];


			///* Store this layer's local tilesets */
			///* So we can quickly convert local to global tilesets */
			//Tileset* localTilesets[128] = {0};
			//for(unsigned int i = 0; i < 128; ++i)
			//{
			//	unsigned int globalTileset = layer->tilesets[i];
			//	if(globalTileset >= 0 && globalTileset < tilesetCount)
			//		localTilesets[i] = tilesets[globalTileset];
			//}

						///* Get the tileset's drawing settings */
						//if(tile->tileset == 0 || tile->tileset > 128)
						//	break;

						///* IMPORTANT: Subtract one, see Tile.h */
						//Tileset* tileset = tilesets[tile->tileset - 1]; //localTilesets[tile->tileset - 1];
						//if(!tileset)
						//	break;