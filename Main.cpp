// ============================================================================
//
// This file contains the actions, conditions and expressions your object uses
// 
// ============================================================================

#include "Common.h"
#define MINIZ_HEADER_FILE_ONLY 1
#include "miniz.c"
#include "Paramacro.h"
#include "HWASurface.h"

// ============================================================================
//
// CONDITIONS
// 
// ============================================================================

CONDITION(
	/* ID */			0,
	/* Name */			"%o: File loaded",
	/* Flags */			0,
	/* Params */		(0)
) {
	return true;
}

CONDITION(
	/* ID */			1,
	/* Name */			"%o: File saved",
	/* Flags */			0,
	/* Params */		(0)
) {
	return true;
}

CONDITION(
	/* ID */			2,
	/* Name */			"%o: Loading file failed",
	/* Flags */			0,
	/* Params */		(0)
) {
	return true;
}

CONDITION(
	/* ID */			3,
	/* Name */			"%o: Saving file failed",
	/* Flags */			0,
	/* Params */		(0)
) {
	return true;
}



// ============================================================================
//
// ACTIONS
// 
// ============================================================================

// Place tiles at (x,y)
short WINAPI DLLExport ActionFunc32(LPRDATA rdPtr, long param1, long param2);

ACTION(
	/* ID */			0,
	/* Name */			"Set layer tile size to (%0,%1)",
	/* Flags */			0,
	/* Params */		(2, PARAM_NUMBER,"Tile width (pixels)", PARAM_NUMBER,"Tile height (pixels)")
) {
	if (rdPtr->currentLayer)
	{
		rdPtr->currentLayer->tileWidth = (unsigned short)intParam();
		rdPtr->currentLayer->tileHeight = (unsigned short)intParam();
		rdPtr->redraw = true;
	}
}

ACTION(
	/* ID */			1,
	/* Name */			"Resize layer to (%0, %1)",
	/* Flags */			0,
	/* Params */		(2, PARAM_NUMBER,"Layer width (tiles)", PARAM_NUMBER,"Layer height (tiles)")
) {
	if (rdPtr->currentLayer)
	{
		int width = intParam();
		int height = intParam();

		rdPtr->currentLayer->resize(width, height);
		rdPtr->redraw = true;
	}
}

ACTION(
	/* ID */			2,
	/* Name */			"Set layer scroll speed to (%0, %1)",
	/* Flags */			0,
	/* Params */		(2, PARAM_NUMBER,"X speed (1.0 = normal)", PARAM_NUMBER,"Y speed (1.0 = normal)")
) {
	if (rdPtr->currentLayer)
	{
		rdPtr->currentLayer->scrollX = fltParam();
		rdPtr->currentLayer->scrollY = fltParam();
		rdPtr->redraw = true;
	}
}

ACTION(
	/* ID */			3,
	/* Name */			"Assign file path %0 to tileset",
	/* Flags */			0,
	/* Params */		(1, PARAM_STRING, "File path (Relative file path, e.g. \"./Tileset.png\" or custom identifier)")
) {
	if (rdPtr->currentTileset)
	{
		memset(rdPtr->currentTileset->path, 0, 256);
		strncpy(rdPtr->currentTileset->path, strParam(), 255);
	}
}

ACTION(
	/* ID */			4,
	/* Name */			"Set tileset image from file %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_FILENAME2,"File path")
) {
	if (rdPtr->currentTileset)
	{
		// Load file
		cSurface file;
		if (!ImportImage(rdPtr->rHo.hoAdRunHeader->rh4.rh4Mv->mvImgFilterMgr, (const char*)param1, &file, 0, 0))
			return;

		// Allocate surface
		Tileset* tileset = rdPtr->currentTileset;
		if (!tileset->surface)
			tileset->surface = new cSurface;
		else
			tileset->surface->Delete();

		// Create blank surface
		cSurface* proto = getPrototype(rdPtr->depth);
		tileset->surface->Create(file.GetWidth(), file.GetHeight(), proto);
		tileset->surface->SetTransparentColor(tileset->transpCol);

		// Blit file onto surface
		copyBlit(file, *tileset->surface);

		// Update 'texture' surface (trivial in non-hwa)
		tileset->updateTexture();

		// Store relative file path, in case the file is saved
		memset(tileset->path, 0, 256);
		char path[MAX_PATH], workingDir[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, workingDir);
		if (PathRelativePathTo(path, workingDir, FILE_ATTRIBUTE_DIRECTORY, (const char*)param1, 0))
		{
			strcpy_s(tileset->path, 256, path);
		}
		// Failed to store relative path, use absolute
		else
		{
			strcpy_s(tileset->path, 256, (const char*)param1);
		}

		rdPtr->redraw = true;
	}
}

ACTION(
	/* ID */			5,
	/* Name */			"Set cursor fill pattern to (%0, %1)",
	/* Flags */			0,
	/* Params */		(2, PARAM_NUMBER, "X pattern (0: Wrap, 1: Repeat, 2: Box, 3: Random)", PARAM_NUMBER, "Y pattern (0: Wrap, 1: Repeat, 2: Box, 3: Random)")
) {
	rdPtr->cursor.patternX = (unsigned char)intParam();
	rdPtr->cursor.patternY = (unsigned char)intParam();
}

ACTION(
	/* ID */			6,
	/* Name */			"Clear tiles at cursor",
	/* Flags */			0,
	/* Params */		(0)
) {
	TileRange temp = rdPtr->cursor.tiles;
	rdPtr->cursor.tiles.a.id = Tile::EMPTY;
	rdPtr->cursor.tiles.b.id = Tile::EMPTY;
	ActionFunc32(rdPtr, rdPtr->cursor.x, rdPtr->cursor.y);
	rdPtr->cursor.tiles = temp;
	rdPtr->redraw = true;
}


ACTION(
	/* ID */			7,
	/* Name */			"Set layer tile at (%0, %1) to (%2, %3)",
	/* Flags */			0,
	/* Params */		(4, PARAM_NUMBER,"Tile X", PARAM_NUMBER,"Tile Y", PARAM_NUMBER,"Tileset index (0-99, -1: Empty)",
	PARAM_NUMBER,"Tileset X (-1: Empty)", PARAM_NUMBER,"Tileset Y (-1: Empty)")
) {
	if (rdPtr->currentLayer)
	{
		unsigned int x = intParam();
		unsigned int y = intParam();
		unsigned char tileX = (unsigned char)intParam();
		unsigned char tileY = (unsigned char)intParam();

		Layer* layer = rdPtr->currentLayer;
		if (layer->isValid() && x < layer->width && y < layer->height)
		{
			Tile* tile = layer->get(x, y);
			tile->x = tileX;
			tile->y = tileY;
		}

		rdPtr->redraw = true;
	}
}

ACTION(
	/* ID */			8,
	/* Name */			"Set current layer to %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_NUMBER,"Layer index")
) {
	unsigned int id = intParam();

	if (id < rdPtr->layers->size())
		rdPtr->currentLayer = &(*rdPtr->layers)[id];
	else	
		rdPtr->currentLayer = 0;
}

ACTION(
	/* ID */			9,
	/* Name */			"Set layer wrapping to (%0,%1)",
	/* Flags */			0,
	/* Params */		(2, PARAM_NUMBER,"Wrap X (0: No, 1: Yes)", PARAM_NUMBER,"Wrap Y (0: No, 1: Yes)")
) {
	if (rdPtr->currentLayer)
	{
		rdPtr->currentLayer->wrapX = intParam() != 0;
		rdPtr->currentLayer->wrapY = intParam() != 0;
		rdPtr->redraw = true;
	}
}

ACTION(
	/* ID */			10,
	/* Name */			"Set layer offset to (%0,%1)",
	/* Flags */			0,
	/* Params */		(2, PARAM_NUMBER,"Offset X (pixels)", PARAM_NUMBER,"Offset Y (pixels)")
) {
	if (rdPtr->currentLayer)
	{	
		rdPtr->currentLayer->offsetX = (short)intParam();
		rdPtr->currentLayer->offsetY = (short)intParam();
		rdPtr->redraw = true;
	}
}

ACTION(
	/* ID */			11,
	/* Name */			"Attach viewport %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_OBJECT, "Viewport object")
) {
	TMAPVIEW* o = (TMAPVIEW*)objParam();

	// Assign this Tile Map and redraw
	o->p = rdPtr;
	rdPtr->rRd->LPRO_Redraw((LPRO)o);

	// Add to list
	rdPtr->viewports->push_back(o);
}

ACTION(
	/* ID */			12,
	/* Name */			"Add new tileset",
	/* Flags */			0,
	/* Params */		(0)
) {
	rdPtr->tilesets->push_back(Tileset());
	rdPtr->currentTileset = &rdPtr->tilesets->back();
}

ACTION(
	/* ID */			13,
	/* Name */			"Fill layer area (%0, %1)(%2, %3) with tiles (%4, %5)(%6, %7) using method %8",
	/* Flags */			0,
	/* Params */		(10, PARAM_NUMBER,"Top-left tile X (Negative numbers will wrap)", PARAM_NUMBER,"Top-left tile Y (Negative numbers will wrap)",
	PARAM_NUMBER,"Bottom-right tile X (Negative numbers will wrap)", PARAM_NUMBER,"Bottom-right tile Y (Negative numbers will wrap)",
	//PARAM_NUMBER, "Tileset index (0-99, -1: Empty)",
	PARAM_NUMBER,"Top-left tileset X", PARAM_NUMBER,"Top-left tileset Y", PARAM_NUMBER,"Bottom-right tileset X", PARAM_NUMBER,"Bottom-right tileset Y",
	PARAM_NUMBER,"Method, add one of X (1: Wrap, 4: Repeat, 16: Box, 64: Random) and Y (2: Wrap, 8: Repeat, 32: Box, 128: Random)")
) {

	if (rdPtr->currentLayer)
	{
		if (!rdPtr->currentLayer->isValid())
			return;

		int tlX = intParam();
		int tlY = intParam();
		int brX = intParam();
		int brY = intParam();

		//int tilesetID = intParam();

		int x1 = intParam();
		int y1 = intParam();
		int x2 = intParam();
		int y2 = intParam();

		int method = intParam();

		int width = rdPtr->currentLayer->width;
		int height = rdPtr->currentLayer->height;

		if (tlX < 0) tlX += width;
		if (tlY < 0) tlY += height;
		if (brX < 0) brX += width;
		if (brY < 0) brY += height;

		tlX = max(0, min(width-1, tlX));
		brX = max(0, min(width-1, brX));
		tlY = max(0, min(height-1, tlY));
		brY = max(0, min(height-1, brY));

		if (brX-tlX < 0 || brY-tlY < 0)
			return;

		Tile* data = rdPtr->currentLayer->data;
		for (int x = 0; x <= brX-tlX; ++x)
		{
			for (int y = 0; y <= brY-tlY; ++y)
			{
				Tile* tile = data + (tlX+x) + width*(tlY+y);
				//tile->tileset = tilesetID + 1;
			
				// X method
				switch(method & (1|4|16|64))
				{
				case 1: // Wrap
					tile->x = x1 + ((x2-x1) ? (x % (x2-x1+1)) : 0); break;
				case 4: // Repeat
					tile->x = max(x1, min(x2, x1+x)); break;
				case 16: // Box
					tile->x = x1 + (x ? (1 + x/(brX-tlX)) : 0); break;
				case 64: // Random
					tile->x = x1 + ((x2-x1) ? (rand() % (x2-x1+1)) : 0); break;
				default: // Undefined
					tile->x = x1 + x;
				}	

				// Y method
				switch(method & (2|8|32|128))
				{
				case 2: // Wrap
					tile->y = y1 + ((y2-y1) ? (y % (y2-y1+1)) : 0); break;
				case 8: // Repeat
					tile->y = max(y1, min(y2, y1+y)); break;
				case 32: // Box
					tile->y = y1 + (y ? (1 + y/(brY-tlY)) : 0); break;
				case 128: // Random
					tile->y = y1 + ((y2-y1) ? (rand() % (y2-y1+1)) : 0); break;
				default: // Undefined
					tile->y = y1 + y;
				}

			}
		}

		rdPtr->redraw = true;
	}
}


ACTION(
	/* ID */			14,
	/* Name */			"Set tileset image from Surface %0 image %1",
	/* Flags */			0,
	/* Params */		(2, PARAM_OBJECT,"Surface", PARAM_NUMBER,"Image index")
) {
	if (rdPtr->currentTileset)
	{
		SURFACE* surface = (SURFACE*)param1;
		int id = param2;

		if (surface->rHo.hoIdentifier != MAKEID(S, U, R, F))
			return;

		cSurface* image = surface->imageAt(surface, id);
		if (!image || !image->IsValid())
			return;

		// Allocate surface
		Tileset* tileset = rdPtr->currentTileset;
		if (!tileset->surface)
			tileset->surface = new cSurface;
		else
			tileset->surface->Delete();
		memset(tileset->path, 0, 256);

		// Create blank surface
		cSurface* proto = getPrototype(rdPtr->depth);
		tileset->surface->Create(image->GetWidth(), image->GetHeight(), proto);

		// Copy settings
		tileset->surface->SetTransparentColor(tileset->transpCol = image->GetTransparentColor());

		// Blit file onto surface
		copyBlit(*image, *tileset->surface);

		// Trivial in non-hwa
		tileset->updateTexture();

		rdPtr->redraw = true;
	}
}

ACTION(
	/* ID */			15,
	/* Name */			"Set layer collision tileset to %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_NUMBER,"Tileset index (0-99, -1: Same as normal tileset)")
) {
	if (rdPtr->currentLayer)
	{
		rdPtr->currentLayer->collision = (unsigned char)intParam();
		rdPtr->redraw = true;
	}
}



ACTION(
	/* ID */			16,
	/* Name */			"Copy layer tiles to Surface %0 image %1",
	/* Flags */			0,
	/* Params */		(2, PARAM_OBJECT,"Surface", PARAM_NUMBER,"Image index")
) {
	Layer* layer = rdPtr->currentLayer;
	if (layer && layer->isValid())
	{
		SURFACE* surface = (SURFACE*)param1;
		int id = param2;

		if (surface->rHo.hoIdentifier != MAKEID(S, U, R, F))
			return;

		cSurface* image = surface->imageAt(surface, id);
		if (!image || !image->IsValid())
			return;

		// Get layer size
		int layerWidth = layer->width;
		int layerHeight = layer->height;

		// Get cursor size
		unsigned int width = rdPtr->cursor.width;
		unsigned int height = rdPtr->cursor.height;

		int x1 = rdPtr->cursor.x;
		int y1 = rdPtr->cursor.y;
		int x2 = x1 + width;
		int y2 = y1 + height;

		// Limit area to layer data
		x1 = max(0, min(layerWidth-1, x1));
		y1 = max(0, min(layerHeight-1, y1));
		x2 = max(0, min(layerWidth, x2));
		y2 = max(0, min(layerHeight, y2));
	
		for (int x = 0; x < x2-x1; ++x)
		{
			for (int y = 0; y < y2-y1; ++y)
			{
				Tile* tile = layer->get(x+x1, y+y1);

				COLORREF color = 0;
				color |= tile->x; // X = red
				color |= tile->y << 8; // Y = green
				image->SetPixel(x, y, color);
			}
		}

		// Redraw surface if necessary
		if (id == surface->targetId)
			surface->rc.rcChanged = true;
	}
}

ACTION(
	/* ID */			17,
	/* Name */			"Paste layer tiles from Surface %0 image %1",
	/* Flags */			0,
	/* Params */		(2, PARAM_OBJECT,"Surface", PARAM_NUMBER,"Image index")
) {
	Layer* layer = rdPtr->currentLayer;
	if (layer && layer->isValid())
	{
		SURFACE* surface = (SURFACE*)param1;
		int id = param2;

		if (surface->rHo.hoIdentifier != MAKEID(S, U, R, F))
			return;

		cSurface* image = surface->imageAt(surface, id);
		if (!image || !image->IsValid())
			return;

		// Get layer size
		int layerWidth = layer->width;
		int layerHeight = layer->height;

		// Get cursor size
		unsigned int width = rdPtr->cursor.width;
		unsigned int height = rdPtr->cursor.height;

		int x1 = rdPtr->cursor.x;
		int y1 = rdPtr->cursor.y;
		int x2 = x1 + width;
		int y2 = y1 + height;

		// Limit area to layer data
		x1 = max(0, min(layerWidth-1, x1));
		y1 = max(0, min(layerHeight-1, y1));
		x2 = max(0, min(layerWidth, x2));
		y2 = max(0, min(layerHeight, y2));
	
		for (int x = 0; x < x2-x1; ++x)
		{
			for (int y = 0; y < y2-y1; ++y)
			{
				Tile* tile = layer->get(x+x1, y+y1);

				COLORREF color = 0;
				image->GetPixel(x, y, color);

				tile->x = color & 0xff;
				tile->y = (color >> 8) & 0xff;
			}
		}

		rdPtr->redraw = true;
	}
}

ACTION(
	/* ID */			18,
	/* Name */			"Set layer visible to %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_NUMBER,"Visible (0: No, 1: Yes)")
) {
	if (rdPtr->currentLayer)
	{
		rdPtr->currentLayer->visible = intParam() != 0;
		rdPtr->redraw = true;
	}
}

ACTION(
	/* ID */			19,
	/* Name */			"Set layer opacity to %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_NUMBER,"Opacity (0.0-1.0)")
) {
	if (rdPtr->currentLayer)
	{
		rdPtr->currentLayer->opacity = fltParam();
		rdPtr->currentLayer->opacity = max(0, min(1, rdPtr->currentLayer->opacity));
		rdPtr->redraw = true;
	}
}

ACTION(
	/* ID */			20,
	/* Name */			"Select tileset range (%0, %1) => (%2, %3) for cursor",
	/* Flags */			0,
	/* Params */		(4, PARAM_NUMBER,"Top-left tileset X", PARAM_NUMBER,"Top-left tileset Y", PARAM_NUMBER,"Bottom-right tileset X", PARAM_NUMBER,"Bottom-right tileset Y")
) {
	rdPtr->cursor.tiles.a.x = (unsigned char)intParam();
	rdPtr->cursor.tiles.a.y = (unsigned char)intParam();
	rdPtr->cursor.tiles.b.x = (unsigned char)intParam();
	rdPtr->cursor.tiles.b.y = (unsigned char)intParam();
}


const char MAGIC[9] = "ACHTUNG!";
const int TILE = 'ELIT';
const int MAP_ = ' PAM'; // deprecated
const int LAYR = 'RYAL';
const int MAIN = 'NIAM'; // LAYR sub-block: Main (tile data)
const int TSID = 'DIST'; // LAYR sub-block: Tileset IDs
const int VALS = 'SLAV'; // LAYR sub-block: Alterable values
const short VER_12 = (1<<8) | 2;
const short VER_11 = (1<<8) | 1;
const short VER_10 = (1<<8) | 0;
#define VER VER_12

ACTION(
	/* ID */			21,
	/* Name */			"Load map from file %0", // (Ignore tilesets: %1)",
	/* Flags */			0,
	/* Params */		(1, PARAM_FILENAME2, "File")
) {
	const char* path = strParam();
	
	// Start reading file
	FILE* file = fopen(path, "rb");
	if (!file)
	{
		rdPtr->rRd->GenerateEvent(2);
		return;
	}

	// Keep reading file, allow exit
	bool error = true;
	do
	{
		// Check magic number
		char leMagic[8];
		fread(&leMagic, sizeof(char)*8, 1, file);
		if (memcmp(leMagic, MAGIC, sizeof(char)*8))
			break;

		// Check version
		short version;
		fread(&version, sizeof(short), 1, file);
		if (version < VER_10 || version > VER)
			break;
		
		// Read blocks
		error = false;
		while (!error && !feof(file))
		{
			// Read block identifier + size
			int block = 0, blockSize = 0;
			fread(&block, sizeof(int), 1, file);
			fread(&blockSize, sizeof(int), 1, file);

			switch(block)
			{
				case MAP_:

					/* Deprecated... however, we will still read the tile size as new default tile size
					   So it can be used when we will read the LAYR block. */
					if (rdPtr->blocks & BLOCK_MAP && version < VER_12)
					{
						// Invalid size, exit
						if (blockSize != sizeof(short)*2)
						{
							error = true;
							break;
						}

						// Tile size
						fread(&rdPtr->tileWidth, sizeof(short), 1, file);
						fread(&rdPtr->tileHeight, sizeof(short), 1, file);
					}
					else
					{
						fseek(file, blockSize, SEEK_CUR);
					}

					break;

				case TILE:

					if (rdPtr->blocks & BLOCK_TILESETS)
					{
						rdPtr->tilesets->clear();

						unsigned char tilesetCount = 0;
						fread(&tilesetCount, sizeof(char), 1, file);
						rdPtr->tilesets->reserve(tilesetCount);

						Tileset* oldTileset = rdPtr->currentTileset;
						for (int i = 0; i < tilesetCount; ++i)
						{
							rdPtr->tilesets->push_back(Tileset());
							Tileset* tileset = &rdPtr->tilesets->back();

							// Read settings
							fread(&tileset->transpCol, sizeof(COLORREF), 1, file);
							
							// Read path
							unsigned char pathLength = 0;
							fread(&pathLength, sizeof(char), 1, file);
							memset(tileset->path, 0, 256);
							fread(tileset->path, 1, pathLength, file);

							// If the file exists, try to load image
							if (GetFileAttributes(tileset->path) != 0xFFFFFFFF)
							{
								rdPtr->currentTileset = tileset;
								ActionFunc6(rdPtr, (long)&tileset->path[0], 0);
							}
						}
						
						rdPtr->currentTileset = oldTileset;
					}
					else
					{
						fseek(file, blockSize, SEEK_CUR);
					}
				break;

				case LAYR:

					if (rdPtr->blocks & BLOCK_LAYERS)
					{
						rdPtr->layers->clear();

						unsigned int layerCount = 0;
						fread(&layerCount, (version < VER_11) ? sizeof(char) : sizeof(short), 1, file);
						rdPtr->layers->reserve(layerCount);

						// Load them layers
						for (unsigned int i = 0; i < layerCount; ++i)
						{
							rdPtr->layers->push_back(Layer());
							Layer* layer = &rdPtr->layers->back();

							// Read settings
							fread(&layer->width, sizeof(int), 1, file);
							fread(&layer->height, sizeof(int), 1, file);

							if (version >= VER_12)
							{
								fread(&layer->tileWidth, sizeof(short), 1, file);
								fread(&layer->tileHeight, sizeof(short), 1, file);
							}
							else
							{
								layer->tileWidth = rdPtr->tileWidth;
								layer->tileHeight = rdPtr->tileHeight;
							}

							fread(&layer->tileset, sizeof(char), 1, file);
							fread(&layer->collision, sizeof(char), 1, file);
							fread(&layer->offsetX, sizeof(int), 1, file);
							fread(&layer->offsetY, sizeof(int), 1, file);
							fread(&layer->scrollX, sizeof(float), 1, file);
							fread(&layer->scrollY, sizeof(float), 1, file);
							fread(&layer->wrapX, sizeof(bool), 1, file);
							fread(&layer->wrapY, sizeof(bool), 1, file);
							fread(&layer->visible, sizeof(bool), 1, file);
							fread(&layer->opacity, sizeof(float), 1, file);

							// Get the number of data bocks
							unsigned char dataBlockCount;
							fread(&dataBlockCount, sizeof(char), 1, file);

							// Now, keeps read all the data blocks
							for (int i = 0; i < dataBlockCount; ++i)
							{
								int dataBlock;
								fread(&dataBlock, sizeof(int), 1, file);

								// Prepare for the data type
								unsigned char* destination = 0;

								switch(dataBlock)
								{
									case MAIN:

										// Try to allocate an array to hold the data
										layer->resize(layer->width, layer->height);

										// Allocation succeeded, assign data pointer
										if (layer->isValid())
											destination = (unsigned char*)layer->data;

										break;
								}

								// Read compressed size
								mz_ulong dataSize = 0;
								fread(&dataSize, sizeof(long), 1, file);
										
								// Recognized data, read it
								if (destination)
								{
									// Read the compressed data
									unsigned char* temp = new unsigned char[dataSize];
									fread(temp, dataSize, 1, file);

									// Uncompress data
									mz_ulong dataAlloc = sizeof(Tile) * layer->width * layer->height;
									mz_uncompress(destination, &dataAlloc, temp, dataSize);

									// Delete temporary compression buffer
									delete[] temp;
								}
								// We can't read this...
								else
								{
									fseek(file, dataSize, SEEK_CUR);
								}
							}
						}
					}
					else
					{
						fseek(file, blockSize, SEEK_CUR);
					}

				break;

				default:
					if (!feof(file))
					{
						// Error occured
						error = true;
					}
					break;

			} // switch(block)
		} // while (!error && !feof(file))

	} while (0);

	if (rdPtr->blocks & BLOCK_TILESETS)
		rdPtr->currentTileset = 0;
	rdPtr->currentLayer = 0;

	if (error)
		rdPtr->rRd->GenerateEvent(2);
	else
		rdPtr->rRd->GenerateEvent(0);

	fclose(file);

	rdPtr->redraw = true;
}


ACTION(
	/* ID */			22,
	/* Name */			"Save map to file %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_FILENAME2,"File")
) {
	unsigned int layerCount = rdPtr->layers->size();
	unsigned int tilesetCount = rdPtr->tilesets->size();

	// Open file
	FILE* file = fopen((const char*)param1, "wb");
	if (!file)
	{
		rdPtr->rRd->GenerateEvent(3);
		return;
	}

	// Magic number, version, tileset and layer count
	fputs(MAGIC, file);
	fwrite(&VER, sizeof(short), 1, file);
	
	int blockSize = 0;

	// Write map block
	if (rdPtr->blocks & BLOCK_MAP)
	{
		fwrite(&MAP_, sizeof(int), 1, file);
		blockSize = sizeof(short)*2;
		fwrite(&blockSize, sizeof(int), 1, file);
		fwrite(&rdPtr->tileWidth, sizeof(short), 1, file);
		fwrite(&rdPtr->tileHeight, sizeof(short), 1, file);
	}

	// Write tileset block
	if (rdPtr->blocks & BLOCK_TILESETS)
	{
		fwrite(&TILE, sizeof(int), 1, file);
		blockSize = sizeof(char);
		for (unsigned int i = 0; i < tilesetCount; ++i)
		{
			blockSize += sizeof(COLORREF);
			blockSize += sizeof(char);
			blockSize += strlen((&(*rdPtr->tilesets)[i])->path);
		}
		fwrite(&blockSize, sizeof(int), 1, file);
		fwrite(&tilesetCount, sizeof(char), 1, file);

		for (unsigned int i = 0; i < tilesetCount; ++i)
		{
			Tileset* tileset = &(*rdPtr->tilesets)[i];
			fwrite(&tileset->transpCol, sizeof(COLORREF), 1, file);
			fputc(strlen(tileset->path), file);
			fputs(tileset->path, file);
		}
	}

	// Write layer block
	if (rdPtr->blocks & BLOCK_LAYERS)
	{
		fwrite(&LAYR, sizeof(int), 1, file);
		
		// Size will be written later...
		blockSize = ftell(file);
		fwrite(&blockSize, sizeof(int), 1, file);

		fwrite(&layerCount, sizeof(short), 1, file);

		for (unsigned int i = 0; i < layerCount; ++i)
		{
			Layer* layer = &(*rdPtr->layers)[i];

			// General settings
			fwrite(&layer->width, sizeof(int), 1, file);
			fwrite(&layer->height, sizeof(int), 1, file);
			fwrite(&layer->tileWidth, sizeof(short), 1, file);
			fwrite(&layer->tileHeight, sizeof(short), 1, file);
			fwrite(&layer->tileset, sizeof(char), 1, file);
			fwrite(&layer->collision, sizeof(char), 1, file);
			fwrite(&layer->offsetX, sizeof(int), 1, file);
			fwrite(&layer->offsetY, sizeof(int), 1, file);
			fwrite(&layer->scrollX, sizeof(float), 1, file);
			fwrite(&layer->scrollY, sizeof(float), 1, file);
			fwrite(&layer->wrapX, sizeof(bool), 1, file);
			fwrite(&layer->wrapY, sizeof(bool), 1, file);
			fwrite(&layer->visible, sizeof(bool), 1, file);
			fwrite(&layer->opacity, sizeof(float), 1, file);

			// Number of data blocks
			unsigned char dataBlockCount = 1;
			fwrite(&dataBlockCount, sizeof(char), 1, file);

			// Tile data
			fwrite(&MAIN, sizeof(int), 1, file);

			if (layer->isValid())
			{
				// Compress tile data...
				mz_ulong dataSize = sizeof(Tile) * layer->width * layer->height;
				mz_ulong dataAlloc = mz_compressBound(dataSize);
				unsigned char* temp = new unsigned char[dataAlloc];
				mz_compress2(temp, &dataAlloc, (const unsigned char*)layer->data, dataSize, rdPtr->compress);

				// Write compressed size
				fwrite(&dataAlloc, sizeof(int), 1, file);

				// Write compressed data
				fwrite(temp, dataAlloc, 1, file);

				// Delete temporary compression buffer
				delete[] temp;
			}
			else
			{
				// Write empty data
				int nullValue = 0;
				fwrite(&nullValue, sizeof(int), 1, file);
			}
		}

		// Go back and write the block size
		int blockSizeAfter = ftell(file);
		fseek(file, blockSize, SEEK_SET);
		blockSize = blockSizeAfter - blockSize - sizeof(int);
		fwrite(&blockSize, sizeof(int), 1, file);
	}

	rdPtr->rRd->GenerateEvent(1);
	fclose(file);
}

ACTION(
	/* ID */			23,
	/* Name */			"Clear layer tile at (%0, %1)",
	/* Flags */			0,
	/* Params */		(2, PARAM_NUMBER,"Tile X", PARAM_NUMBER,"Tile Y")
) {
	if (rdPtr->currentLayer)
	{
		unsigned int x = intParam();
		unsigned int y = intParam();

		Layer* layer = rdPtr->currentLayer;
		if (layer->isValid() && x < layer->width && y < layer->height)
		{
			Tile* tile = layer->get(x, y);
			tile->id = Tile::EMPTY;

			rdPtr->redraw = true;
		}
	}
}


ACTION(
	/* ID */			24,
	/* Name */			"Fill layer area (%0, %1) => (%2, %3) with tile (%4, %5)",
	/* Flags */			0,
	/* Params */		(6, PARAM_NUMBER,"Top-left tile X (Negative numbers will wrap)", PARAM_NUMBER,"Top-left tile Y (Negative numbers will wrap)",
	PARAM_NUMBER,"Bottom-right tile X (Negative numbers will wrap)", PARAM_NUMBER,"Bottom-right tile Y (Negative numbers will wrap)",
	 /*PARAM_NUMBER,"Tileset index (0-99, -1: Empty)",*/ PARAM_NUMBER,"Tileset X (-1: Empty)", PARAM_NUMBER,"Tileset Y (-1: Empty)")
) {

	if (rdPtr->currentLayer)
	{
		if (!rdPtr->currentLayer->isValid())
			return;

		int tlX = intParam();
		int tlY = intParam();
		int brX = intParam();
		int brY = intParam();

		//int tilesetID = intParam();
		int tileX = intParam();
		int tileY = intParam();

		int width = rdPtr->currentLayer->width;
		int height = rdPtr->currentLayer->height;

		if (tlX < 0) tlX += width;
		if (tlY < 0) tlY += height;
		if (brX < 0) brX += width;
		if (brY < 0) brY += height;

		tlX = max(0, min(width-1, tlX));
		brX = max(0, min(width-1, brX));
		tlY = max(0, min(height-1, tlY));
		brY = max(0, min(height-1, brY));

		if (brX-tlX < 0 || brY-tlY < 0)
			return;

		Tile* data = rdPtr->currentLayer->data;
		for (int x = 0; x <= brX-tlX; ++x)
		{
			for (int y = 0; y <= brY-tlY; ++y)
			{
				Tile* tile = data + (tlX+x) + width*(tlY+y);
				//tile->tileset = tilesetID + 1;
				tile->x = tileX;
				tile->y = tileY;
			}
		}

		rdPtr->redraw = true;
	}
}

ACTION(
	/* ID */			25,
	/* Name */			"Add tilesets from Surface %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_OBJECT,"Surface")
) {
	SURFACE* surface = (SURFACE*)objParam();
	Tileset* oldTileset = rdPtr->currentTileset;

	if (surface->rHo.hoIdentifier != MAKEID(S, U, R, F))
		return;

	for (int i = 0; i < surface->imageCount(surface); ++i)
	{
		// Add tileset
		ActionFunc12(rdPtr, 0, 0);
		// Load image into it
		ActionFunc14(rdPtr, (long)surface, i);
	}

	rdPtr->currentTileset = oldTileset;
	rdPtr->redraw = true;
}

ACTION(
	/* ID */			26,
	/* Name */			"Set file compression level to %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_NUMBER, "Compression level (0-10, 0: None, 6: Default, 10: Best but slowest)")
) {
	rdPtr->compress = (char)max(0, min(10, param1));
}

ACTION(
	/* ID */			27,
	/* Name */			"Set tileset transparent color to %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_COLOUR, "Transparent color")
) {
	if (rdPtr->currentTileset)
	{
		rdPtr->currentTileset->transpCol = param1;

		if (rdPtr->currentTileset->surface)
			rdPtr->currentTileset->surface->SetTransparentColor(param1);

		rdPtr->redraw = true;
	}
}

ACTION(
	/* ID */			28,
	/* Name */			"Place tiles at cursor",
	/* Flags */			0,
	/* Params */		(0)
) {
	ActionFunc32(rdPtr, rdPtr->cursor.x, rdPtr->cursor.y);
}


ACTION(
	/* ID */			29,
	/* Name */			"Add new layer with size (%0,%1)",
	/* Flags */			0,
	/* Params */		(2,PARAM_NUMBER,"Layer width (tiles)", PARAM_NUMBER,"Layer height (tiles)")
) {
	rdPtr->layers->push_back(Layer());
	rdPtr->currentLayer = &rdPtr->layers->back();
	ActionFunc1(rdPtr, param1, param2);
}

ACTION(
	/* ID */			30,
	/* Name */			"Set current tileset to %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_NUMBER,"Tileset index")
) {
	unsigned int id = intParam();

	if (id < rdPtr->tilesets->size())
		rdPtr->currentTileset = &(*rdPtr->tilesets)[id];
	else	
		rdPtr->currentTileset = 0;
}

ACTION(
	/* ID */			31,
	/* Name */			"Detach viewport %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_OBJECT, "Viewport object")
) {
	TMAPVIEW* o = (TMAPVIEW*)objParam();

	// Detach parent Tile Map, redraw (to clear)
	o->p = 0;
	rdPtr->rRd->LPRO_Redraw((LPRO)o);

	// Remove...
	rdPtr->viewports->remove(o);
}

ACTION(
	/* ID */			32,
	/* Name */			"Place tiles at (%0, %1)",
	/* Flags */			0,
	/* Params */		(2, PARAM_NUMBER, "Tile X", PARAM_NUMBER, "Tile Y")
) {
	Layer* layer = rdPtr->currentLayer;
	if (layer && layer->isValid())
	{
		// Get layer size
		int layerWidth = layer->width;
		int layerHeight = layer->height;

		// Get cursor size
		unsigned int width = rdPtr->cursor.width;
		unsigned int height = rdPtr->cursor.height;

		int x1 = param1;
		int y1 = param2;

		// Single tile, avoid loop
		if (width == 1 && height == 1)
		{
			if (x1 >= 0 && y1 >= 0 && x1 < layerWidth && y1 < layerHeight)
			{
				Tile* tile = layer->get(x1, y1);
				tile->x = rdPtr->cursor.tiles.a.x;
				tile->y = rdPtr->cursor.tiles.b.y;
			}
			
			
			return;
		}

		// Rectangular area
		int x2 = x1 + width-1;
		int y2 = y1 + height-1	;

		// Limit area to layer data
		x1 = max(0, min(layerWidth-1, x1));
		y1 = max(0, min(layerHeight-1, y1));
		x2 = max(0, min(layerWidth-1, x2));
		y2 = max(0, min(layerHeight-1, y2));

		// Get tile range and pattern
		TileRange tiles = rdPtr->cursor.tiles;
		unsigned char patternX = rdPtr->cursor.patternX;
		unsigned char patternY = rdPtr->cursor.patternY;

		for (int x = 0; x <= x2-x1; ++x)
		{
			for (int y = 0; y <= y2-y1; ++y)
			{
				Tile* tile = layer->get(x+x1, y+y1);

				// X value
				if (tiles.b.x-tiles.a.x)
				{
					switch(patternX)
					{
						case 1: // Repeat
							tile->x = min(tiles.b.x, x + tiles.a.x); break;
						case 2: // Box
							tile->x = tiles.a.x + (x ? (1 + x/(x2-x1)) : 0); break;
						case 3: // Random
							tile->x = tiles.a.x + rand() % (tiles.b.x-tiles.a.x+1); break;
						default: // Wrap
							tile->x = tiles.a.x + x % (tiles.b.x-tiles.a.x+1);
					}
				}
				else
				{
					tile->x = tiles.a.x;
				}

				// Y value
				if (tiles.b.y-tiles.a.y)
				{
					switch(patternY)
					{
						case 1: // Repeat
							tile->y = min(tiles.b.y, y + tiles.a.y); break;
						case 2: // Box
							tile->y = tiles.a.y + (y ? (1 + y/(y2-y1)) : 0); break;
						case 3: // Random
							tile->y = tiles.a.y + rand() % (tiles.b.y-tiles.a.y+1); break;
						default: // Wrap
							tile->y = tiles.a.y + y % (tiles.b.y-tiles.a.y+1);
					}
				}
				else
				{
					tile->y = tiles.a.y;
				}
			}
		}

		rdPtr->redraw = true;
	}
}

ACTION(
	/* ID */			33,
	/* Name */			"Resize cursor to (%0, %1)",
	/* Flags */			0,
	/* Params */		(2, PARAM_NUMBER, "Width in tiles", PARAM_NUMBER, "Height in tiles")
) {
	rdPtr->cursor.width = max(1, param1);
	rdPtr->cursor.height = max(1, param2);
}


ACTION(
	/* ID */			34,
	/* Name */			"Swap tilesets %0 and %1",
	/* Flags */			0,
	/* Params */		(2, PARAM_NUMBER, "Tileset A (0-99)", PARAM_NUMBER, "Tileset B (0-99)")
) {
	unsigned int a = intParam(), b = intParam();
	if (a < rdPtr->tilesets->size() && b < rdPtr->tilesets->size())
	{
		swap(rdPtr->tilesets->at(a), rdPtr->tilesets->at(b));
		rdPtr->redraw = true;
	}
}

ACTION(
	/* ID */			35,
	/* Name */			"Move cursor to (%0, %1)",
	/* Flags */			0,
	/* Params */		(2, PARAM_NUMBER, "Tile X", PARAM_NUMBER, "Tile Y")
) {
	rdPtr->cursor.x = intParam();
	rdPtr->cursor.y = intParam();
}

ACTION(
	/* ID */			36,
	/* Name */			"Select tileset tile (%0, %1) for cursor",
	/* Flags */			0,
	/* Params */		(2, PARAM_NUMBER, "Tileset X (-1: Empty)", PARAM_NUMBER, "Tileset Y (-1: Empty)")
) {
	rdPtr->cursor.tiles.a.x = (unsigned char)intParam();
	rdPtr->cursor.tiles.a.y = (unsigned char)intParam();
	rdPtr->cursor.tiles.b.x = rdPtr->cursor.tiles.a.x;
	rdPtr->cursor.tiles.b.y = rdPtr->cursor.tiles.a.y;
}

ACTION(
	/* ID */			37,
	/* Name */			"Set layer tileset to %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_NUMBER,"Tileset index (0-99)")
) {
	if (rdPtr->currentLayer)
	{
		rdPtr->currentLayer->tileset = (unsigned char)intParam();
		rdPtr->redraw = true;
	}
}


ACTION(
	/* ID */			38,
	/* Name */			"Paste layer tiles from (%0, %1)",
	/* Flags */			0,
	/* Params */		(2,PARAM_NUMBER,"Tile X",PARAM_NUMBER,"Tile Y")
) {
	Layer* layer = rdPtr->currentLayer;
	if (layer && layer->isValid())
	{
		// Get layer size
		int layerWidth = layer->width;
		int layerHeight = layer->height;

		// Get cursor size
		unsigned int width = rdPtr->cursor.width;
		unsigned int height = rdPtr->cursor.height;

		// Limit cursor size to rectangle
		int srcX = intParam();
		int srcY = intParam();
		width = min((unsigned)layerWidth-srcX, width);
		height = min((unsigned)layerHeight-srcY, height);

		// Get rectangle
		int x1 = rdPtr->cursor.x;
		int y1 = rdPtr->cursor.y;
		int x2 = x1 + width;
		int y2 = y1 + height;

		// Limit area to layer data
		x1 = max(0, min(layerWidth-1, x1));
		y1 = max(0, min(layerHeight-1, y1));
		x2 = max(0, min(layerWidth, x2));
		y2 = max(0, min(layerHeight, y2));
	
		for (int x = 0; x < x2-x1; ++x)
		{
			for (int y = 0; y < y2-y1; ++y)
			{
				Tile* tile = layer->get(x+x1, y+y1);
				Tile* src = layer->get(x+srcX, y+srcY);
				tile->x = src->x;
				tile->y = src->y;
			}
		}
	}

	rdPtr->redraw = true;
}

ACTION(
	/* ID */			39,
	/* Name */			"Maximize cursor",
	/* Flags */			0,
	/* Params */		(0)
) {
	if (rdPtr->currentLayer)
	{
		rdPtr->cursor.x = 0;
		rdPtr->cursor.y = 0;
		rdPtr->cursor.width = rdPtr->currentLayer->width;
		rdPtr->cursor.height = rdPtr->currentLayer->height;
	}
}

ACTION(
	/* ID */			40,
	/* Name */			"Set file block options to %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_NUMBER, "Blocks to load/save, add together (1: *Unused*, 2: Layers, 4: Tilesets, Default: 2)")
) {
	rdPtr->blocks = param1;
}

ACTION(
	/* ID */			41,
	/* Name */			"Delete tileset %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_NUMBER, "Tileset index")
) {
	unsigned int i = Param(TYPE_INT);

	if (i < rdPtr->tilesets->size())
	{
		if (&(*rdPtr->tilesets)[i] == rdPtr->currentTileset)
			rdPtr->currentTileset = 0;

		rdPtr->tilesets->erase(rdPtr->tilesets->begin() + i);

		rdPtr->redraw = true;
	}
}

ACTION(
	/* ID */			42,
	/* Name */			"Delete layer %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_NUMBER, "Layer index")
) {
	unsigned int i = Param(TYPE_INT);

	if (i < rdPtr->layers->size())
	{
		if (&(*rdPtr->layers)[i] == rdPtr->currentLayer)
			rdPtr->currentLayer = 0;

		rdPtr->layers->erase(rdPtr->layers->begin() + i);

		rdPtr->redraw = true;
	}
}

ACTION(
	/* ID */			43,
	/* Name */			"Delete all tilesets",
	/* Flags */			0,
	/* Params */		(0)
) {
	rdPtr->tilesets->clear();
	rdPtr->currentTileset = 0;
}

ACTION(
	/* ID */			44,
	/* Name */			"Delete all layers",
	/* Flags */			0,
	/* Params */		(0)
) {
	rdPtr->layers->clear();
	rdPtr->currentLayer = 0;
}

ACTION(
	/* ID */			45,
	/* Name */			"Clear layer",
	/* Flags */			0,
	/* Params */		(0)
) {
	if (rdPtr->currentLayer && rdPtr->currentLayer->isValid())
	{
		memset(rdPtr->currentLayer->data, Tile::EMPTY & 0xff, rdPtr->currentLayer->width*rdPtr->currentLayer->height*sizeof(Tile));
	}
}

ACTION(
	/* ID */			46,
	/* Name */			"Fill layer with tile (%0, %1)",
	/* Flags */			0,
	/* Params */		(2, PARAM_NUMBER,"Tileset X (-1: Empty)", PARAM_NUMBER,"Tileset Y (-1: Empty)")
) {

	if (rdPtr->currentLayer)
	{
		if (!rdPtr->currentLayer->isValid())
			return;

		int tileX = intParam();
		int tileY = intParam();

		int width = rdPtr->currentLayer->width;
		int height = rdPtr->currentLayer->height;

		Tile* data = rdPtr->currentLayer->data;
		for (int x = 0; x < width; ++x)
		{
			for (int y = 0; y < height; ++y)
			{
				Tile* tile = data + x + width*y;
				tile->x = tileX;
				tile->y = tileY;
			}
		}

		rdPtr->redraw = true;
	}
}

ACTION(
	/* ID */			47,
	/* Name */			"Select (%0, %1) => (%2, %3) with cursor",
	/* Flags */			0,
	/* Params */		(4, PARAM_NUMBER, "Top-left tile X", PARAM_NUMBER, "Top-left tile Y",
							PARAM_NUMBER, "Bottom-right tile X", PARAM_NUMBER, "Bottom-right tile Y")
) {
	rdPtr->cursor.x = intParam();
	rdPtr->cursor.y = intParam();
	rdPtr->cursor.width = intParam() - rdPtr->cursor.x;
	rdPtr->cursor.height = intParam() - rdPtr->cursor.y;
}

// ============================================================================
//
// EXPRESSIONS
// 
// ============================================================================

EXPRESSION(
	/* ID */			0,
	/* Name */			"TilesetCols(",
	/* Flags */			0,
	/* Params */		(1, EXPPARAM_NUMBER, "Tileset index")
) {
	unsigned int i = ExParam(TYPE_INT);

	if (i < rdPtr->tilesets->size())
	{
		if ((*rdPtr->tilesets)[i].isValid())
			return (*rdPtr->tilesets)[i].surface->GetWidth() / rdPtr->tileWidth;
	}
	
	return 0;
}

EXPRESSION(
	/* ID */			1,
	/* Name */			"TilesetRows(",
	/* Flags */			0,
	/* Params */		(1, EXPPARAM_NUMBER, "Tileset index")
) {
	unsigned int i = ExParam(TYPE_INT);

	if (i < rdPtr->tilesets->size())
	{
		if ((*rdPtr->tilesets)[i].isValid())
			return (*rdPtr->tilesets)[i].surface->GetHeight() / rdPtr->tileHeight;
	}
	
	return 0;
}

EXPRESSION(
	/* ID */			2,
	/* Name */			"LayerWidth(",
	/* Flags */			0,
	/* Params */		(1, EXPPARAM_NUMBER, "Layer index")
) {
	unsigned int i = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
		return (*rdPtr->layers)[i].width;
	
	return 0;
}

EXPRESSION(
	/* ID */			3,
	/* Name */			"LayerHeight(",
	/* Flags */			0,
	/* Params */		(1, EXPPARAM_NUMBER, "Layer index")
) {
	unsigned int i = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
		return (*rdPtr->layers)[i].height;
	
	return 0;
}

EXPRESSION(
	/* ID */			4,
	/* Name */			"LayerWrapX(",
	/* Flags */			0,
	/* Params */		(1, EXPPARAM_NUMBER, "Layer index")
) {
	unsigned int i = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
		return (*rdPtr->layers)[i].wrapX;
	
	return 0;
}

EXPRESSION(
	/* ID */			5,
	/* Name */			"LayerWrapY(",
	/* Flags */			0,
	/* Params */		(1, EXPPARAM_NUMBER, "Layer index")
) {
	unsigned int i = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
		return (*rdPtr->layers)[i].wrapY;
	
	return 0;
}

EXPRESSION(
	/* ID */			6,
	/* Name */			"LayerOffsetX(",
	/* Flags */			0,
	/* Params */		(1, EXPPARAM_NUMBER, "Layer index")
) {
	unsigned int i = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
		return (*rdPtr->layers)[i].offsetX;
	
	return 0;
}

EXPRESSION(
	/* ID */			7,
	/* Name */			"LayerOffsetY(",
	/* Flags */			0,
	/* Params */		(1, EXPPARAM_NUMBER, "Layer index")
) {
	unsigned int i = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
		return (*rdPtr->layers)[i].offsetY;
	
	return 0;
}

EXPRESSION(
	/* ID */			8,
	/* Name */			"LayerScrollX(",
	/* Flags */			EXPFLAG_DOUBLE,
	/* Params */		(1, EXPPARAM_NUMBER, "Layer index")
) {
	unsigned int i = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
	{
		// Stupid macro that expands to TWO lines!
		ReturnFloat((*rdPtr->layers)[i].scrollX);
	}
	
	ReturnFloat(0);
}

EXPRESSION(
	/* ID */			9,
	/* Name */			"LayerScrollY(",
	/* Flags */			EXPFLAG_DOUBLE,
	/* Params */		(1, EXPPARAM_NUMBER, "Layer index")
) {
	unsigned int i = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
	{
		// Stupid macro that expands to TWO lines!
		ReturnFloat((*rdPtr->layers)[i].scrollY);
	}
	
	ReturnFloat(0);
}

EXPRESSION(
	/* ID */			10,
	/* Name */			"LayerTileWidth(",
	/* Flags */			0,
	/* Params */		(1, EXPPARAM_NUMBER, "Layer index")
) {
	unsigned int i = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
		return (*rdPtr->layers)[i].tileWidth;
	
	return 0;
}

EXPRESSION(
	/* ID */			11,
	/* Name */			"LayerTileHeight(",
	/* Flags */			0,
	/* Params */		(1, EXPPARAM_NUMBER, "Layer index")
) {
	unsigned int i = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
		return (*rdPtr->layers)[i].tileHeight;
	
	return 0;
}

EXPRESSION(
	/* ID */			12,
	/* Name */			"LayerTilesetX(",
	/* Flags */			0,
	/* Params */		(3, EXPPARAM_NUMBER, "Layer index", EXPPARAM_NUMBER, "Tile X", EXPPARAM_NUMBER, "Tile Y")
) {
	unsigned int i = ExParam(TYPE_INT);
	unsigned int x = ExParam(TYPE_INT);
	unsigned int y = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
	{
		Layer* layer = &(*rdPtr->layers)[i];

		if (x >= layer->width || y >= layer->height)
			return -1;
		
		Tile* tile = layer->get(x, y);

		if(tile->id == Tile::EMPTY)
			return -1;

		return tile->x;
	}
	
	return -1;
}

EXPRESSION(
	/* ID */			13,
	/* Name */			"LayerTilesetY(",
	/* Flags */			0,
	/* Params */		(3, EXPPARAM_NUMBER, "Layer index", EXPPARAM_NUMBER, "Tile X", EXPPARAM_NUMBER, "Tile Y")
) {
	unsigned int i = ExParam(TYPE_INT);
	unsigned int x = ExParam(TYPE_INT);
	unsigned int y = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
	{
		Layer* layer = &(*rdPtr->layers)[i];

		if (x >= layer->width || y >= layer->height)
			return -1;
		
		Tile* tile = layer->get(x, y);

		if(tile->id == Tile::EMPTY)
			return -1;

		return tile->y;
	}
	
	return -1;
}

EXPRESSION(
	/* ID */			14,
	/* Name */			"LayerTile(",
	/* Flags */			0,
	/* Params */		(3, EXPPARAM_NUMBER, "Layer index", EXPPARAM_NUMBER, "Tile X", EXPPARAM_NUMBER, "Tile Y")
) {
	unsigned int i = ExParam(TYPE_INT);
	unsigned int x = ExParam(TYPE_INT);
	unsigned int y = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
	{
		Layer* layer = &(*rdPtr->layers)[i];

		if (x < layer->width && y < layer->height)
		{
			Tile* tile = layer->get(x, y);

			if(tile->id == Tile::EMPTY)
				return -1;

			return 1000*tile->x + tile->y;
		}
	}
	
	return -1;
}

EXPRESSION(
	/* ID */			15,
	/* Name */			"LayerCount(",
	/* Flags */			0,
	/* Params */		(0)
) {
	return rdPtr->layers->size();
}

EXPRESSION(
	/* ID */			16,
	/* Name */			"TilesetCount(",
	/* Flags */			0,
	/* Params */		(0)
) {
	return rdPtr->tilesets->size();
}

EXPRESSION(
	/* ID */			17,
	/* Name */			"LayerTileID(",
	/* Flags */			0,
	/* Params */		(3, EXPPARAM_NUMBER, "Layer index", EXPPARAM_NUMBER, "Tile X", EXPPARAM_NUMBER, "Tile Y")
) {
	unsigned int i = ExParam(TYPE_INT);
	unsigned int x = ExParam(TYPE_INT);
	unsigned int y = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
	{
		Layer* layer = &(*rdPtr->layers)[i];

		if (x < layer->width && y < layer->height)
			return layer->get(x, y)->id;
	}
	
	return Tile::EMPTY;
}