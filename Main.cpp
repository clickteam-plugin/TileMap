// ============================================================================
//
// This file contains the actions, conditions and expressions your object uses
// 
// ============================================================================

#include "Common.h"
#include "Paramacro.h"
#include "HWASurface.h"
#include "TMIStream.h"
#include "TMOStream.h"

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

CONDITION(
	/* ID */			4,
	/* Name */			"%o: On map property",
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
		rdPtr->currentLayer->settings.tileWidth = (unsigned short)intParam();
		rdPtr->currentLayer->settings.tileHeight = (unsigned short)intParam();
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

		for (std::vector<SubLayer>::iterator it = rdPtr->currentLayer->subLayers.begin(); it != rdPtr->currentLayer->subLayers.end(); ++it)
		{
			(*it).resize(width, height);
		}

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
		rdPtr->currentLayer->settings.scrollX = fltParam();
		rdPtr->currentLayer->settings.scrollY = fltParam();
		rdPtr->redraw = true;
	}
}

ACTION(
	/* ID */			3,
	/* Name */			"Assign file path %0 to tileset",
	/* Flags */			0,
	/* Params */		(1, PARAM_FILENAME2, "File path (Relative file path, e.g. \"./Tileset.png\" or custom identifier)")
) {
	if (rdPtr->currentTileset)
	{
		rdPtr->currentTileset->setPath((const char*)param1);
	}
}

ACTION(
	/* ID */			4,
	/* Name */			"Set tileset image from file %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_FILENAME2,"File path")
) {
	const char* filePath = (const char*)param1;

	if (rdPtr->currentTileset)
	{
		// Load file
		cSurface file;
		if (!ImportImage(rdPtr->rHo.hoAdRunHeader->rh4.rh4Mv->mvImgFilterMgr, filePath, &file, 0, 0))
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

		// Store file path, in case tileset data blocks are saved
		rdPtr->currentTileset->setPath(filePath);

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
	/* Params */		(4, PARAM_NUMBER,"Tile X", PARAM_NUMBER,"Tile Y", //PARAM_NUMBER,"Tileset index (0-99, -1: Empty)",
	PARAM_NUMBER,"Tileset X (-1: Empty)", PARAM_NUMBER,"Tileset Y (-1: Empty)")
) {
	if (rdPtr->currentLayer)
	{
		unsigned x = intParam();
		unsigned y = intParam();
		unsigned char tileX = (unsigned char)intParam();
		unsigned char tileY = (unsigned char)intParam();

		Layer* layer = rdPtr->currentLayer;
		if (layer->isValid(x, y))
		{
			Tile* tile = layer->getTile(x, y);
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
	unsigned id = intParam();

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
		rdPtr->currentLayer->settings.wrapX = intParam() != 0;
		rdPtr->currentLayer->settings.wrapY = intParam() != 0;
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
		rdPtr->currentLayer->settings.offsetX = (short)intParam();
		rdPtr->currentLayer->settings.offsetY = (short)intParam();
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
	o->rc.rcChanged = true;

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
	// TODO: Replace
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
		rdPtr->currentTileset->setPath("");

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
	/* Params */		(1, PARAM_NUMBER,"Tileset index (0-255, -1: Same as normal tileset)")
) {
	if (rdPtr->currentLayer)
	{
		rdPtr->currentLayer->settings.collision = (unsigned char)intParam();
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
		int layerWidth = layer->getWidth();
		int layerHeight = layer->getHeight();

		// Get cursor size
		unsigned width = rdPtr->cursor.width;
		unsigned height = rdPtr->cursor.height;

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
				Tile* tile = layer->getTile(x+x1, y+y1);

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
		int layerWidth = layer->getWidth();
		int layerHeight = layer->getHeight();

		// Get cursor size
		unsigned width = rdPtr->cursor.width;
		unsigned height = rdPtr->cursor.height;

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
				Tile* tile = layer->getTile(x+x1, y+y1);

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
		rdPtr->currentLayer->settings.visible = intParam() != 0;
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
		rdPtr->currentLayer->settings.opacity = fltParam();
		rdPtr->currentLayer->settings.opacity = max(0, min(1, rdPtr->currentLayer->settings.opacity));
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
const unsigned TILE = 'ELIT';
const unsigned MAP_ = ' PAM';
const unsigned LAYR = 'RYAL';
const unsigned MAIN = 'NIAM'; // LAYR sub-block: Main (tile data)
const unsigned DATA = 'ATAD'; // LAYR sub-block: Data ("sub-layer")
const short VER_14 = (1<<8) |4;
const short VER_13 = (1<<8) | 3;
const short VER_12 = (1<<8) | 2;
const short VER_11 = (1<<8) | 1;
const short VER_10 = (1<<8) | 0;
const short VER = VER_14;

ACTION(
	/* ID */			21,
	/* Name */			"Load map from file %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_FILENAME2, "File")
) {
	char mapPath[256] = {};
	strcpy_s(mapPath, 256, (const char*)param1);

	// Start reading file
	TMIStream file(mapPath);

	if (!file.good())
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
		file >> leMagic;

		if (memcmp(leMagic, MAGIC, sizeof(char)*8))
			break;

		// Check version
		short version;
		file >> version;
		if (version < VER_10 || version > VER)
			break;
		
		// Read blocks
		error = false;
		while (!error && file.good())
		{
			switch(file.readBlockHeader())
			{
				// Map (properties)
				case MAP_:

					if((rdPtr->blocks & BLOCK_MAP) && version >= VER_13)
					{
						rdPtr->properties->clear();

						unsigned short propertyCount;
						file >> propertyCount;

						for (int i = 0; i < propertyCount; ++i)
						{
							char name[256] = {};
							file.readShortStr(name);
							unsigned char type;
							file >> type;

							if (type == TMPT_INT)
							{
								int value;
								file >> value;
								rdPtr->properties->insert(pair<string, Property>(name, value));
							}
							else if (type == TMPT_FLOAT)
							{
								float value;
								file >> value;
								rdPtr->properties->insert(pair<string, Property>(name, value));
							}
							else if(type == TMPT_STRING)
							{
								char* value = file.readLongStr();
								rdPtr->properties->insert(pair<string, Property>(name, value));
								delete[] value;
							}
							else
							{
								// Unknown type
								error = true;
								break;
							}
						}
					}
					// Deprecated... however, we will still read the tile size as new default tile size
					// So it can be used when we will read the LAYR block.
					else if ((rdPtr->blocks & BLOCK_MAP) && version < VER_13)
					{
						// Tile size
						file >> rdPtr->tileWidth;
						file >> rdPtr->tileHeight;
					}
					else
					{
						// We don't want to read this block, skip it.
						file.skipBlock();
					}

					break;

				case TILE:

					if (rdPtr->blocks & BLOCK_TILESETS)
					{
						rdPtr->tilesets->clear();

						unsigned char tilesetCount = 0;
						file >> tilesetCount;
						rdPtr->tilesets->reserve(tilesetCount);

						// Need to store a copy to the current tileset because we will temporarily change it
						Tileset* currentTileset = rdPtr->currentTileset;

						for (int i = 0; i < tilesetCount; ++i)
						{
							rdPtr->tilesets->push_back(Tileset());
							Tileset* tileset = &rdPtr->tilesets->back();

							// Read settings
							file >> tileset->transpCol;
							
							// Read relative path
							char relativeTilesetPath[256] = {};
							file.readShortStr(relativeTilesetPath);

							// Make this path absolute

							switch (rdPtr->tilesetPathMode)
							{
								case TSPM_APP_PATH:
									tileset->setPathFromRelative(rdPtr->appPath, relativeTilesetPath);
									break;

								case TSPM_MAP_PATH:
									// We'll have to get the directory of the map path (excluding the file name)
									char* slash;
									if ((slash = strrchr(mapPath, '\\')) || (slash = strrchr(mapPath, '/')))
										slash[1] = 0;
									tileset->setPathFromRelative(mapPath, relativeTilesetPath);
									break;

								case TSPM_USER_PATH:
									tileset->setPathFromRelative(rdPtr->tilesetUserPath, relativeTilesetPath);
									break;

								default:
									tileset->setPath(relativeTilesetPath);
							}

							// If the file exists, try to load image
							if (rdPtr->tilesetPathMode != TSPM_CUSTOM
							&&	GetFileAttributes(tileset->getPath()) != 0xFFFFFFFF)
							{
								rdPtr->currentTileset = tileset;
								char tilesetPath[256] = {};
								strcpy_s(tilesetPath, 256, tileset->getPath());
								ActionFunc4(rdPtr, (long)&tilesetPath[0], 0);
							}
						}
						
						rdPtr->currentTileset = currentTileset;
					}
					else
					{
						// We don't want to read this block, skip it.
						file.skipBlock();
					}
				break;

				case LAYR:

					if (rdPtr->blocks & BLOCK_LAYERS)
					{
						rdPtr->layers->clear();

						unsigned layerCount = 0;
						if (version < VER_11)
							file >> (char&)layerCount;
						else
							file >> (short&)layerCount;
						rdPtr->layers->reserve(layerCount);

						// Load them layers
						for (unsigned i = 0; i < layerCount; ++i)
						{

							rdPtr->layers->push_back(Layer());
							Layer* layer = &rdPtr->layers->back();

							// Read settings
							unsigned width, height;
							file >> width;
							file >> height;

							if (version >= VER_12)
							{
								file >> layer->settings.tileWidth;
								file >> layer->settings.tileHeight;
							}
							else
							{
								layer->settings.tileWidth = rdPtr->tileWidth;
								layer->settings.tileHeight = rdPtr->tileHeight;
							}

							file >> layer->settings.tileset;
							file >> layer->settings.collision;
							file >> layer->settings.offsetX;
							file >> layer->settings.offsetY;
							file >> layer->settings.scrollX;
							file >> layer->settings.scrollY;
							file >> layer->settings.wrapX;
							file >> layer->settings.wrapY;
							file >> layer->settings.visible;
							file >> layer->settings.opacity;

							// Sppecial sub-layer indices
							if (version >= VER_14)
								file >> layer->settings.subLayerLink;

							// Get the number of data bocks
							unsigned char dataBlockCount;
							file >> dataBlockCount;

							// Now, keeps read all the data blocks
							for (int i = 0; i < dataBlockCount; ++i)
							{
								unsigned dataBlock;
								file >> dataBlock;

								// Prepare for the data type
								char* destination = 0;
								unsigned size = 0;

								switch(dataBlock)
								{
									case MAIN:

										// Try to allocate an array to hold the data
										layer->resize(width, height);

										// Allocation succeeded, assign data pointer
										if (layer->isValid())
										{
											destination = (char*)layer->getDataPointer();
											size = layer->getByteSize();
										}

										break;

									case DATA:

										// Read sub-layer's cell settings
										char cellSize;
										file >> cellSize;
										unsigned defaultValue;
										file >> defaultValue;

										// Add new sub-layer
										layer->subLayers.push_back(SubLayer(cellSize, defaultValue));
										SubLayer& subLayer = layer->subLayers.back();

										subLayer.resize(width, height);

										if (subLayer.isValid())
										{
											destination = (char*)subLayer.getDataPointer();
											size = subLayer.getByteSize();
										}

										break;
								}

								// Read compressed size
								file.readCompressedData(destination, size);
							}
						}
					}
					else
					{
						// We don't want to read this block, skip it.
						file.skipBlock();
					}

				break;

				default:
					if (!file.eof())
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

	rdPtr->redraw = true;
}


ACTION(
	/* ID */			22,
	/* Name */			"Save map to file %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_FILENAME2,"File")
) {
	const char* mapPath = (const char*)param1;
	
	TMOStream file(mapPath, rdPtr->compress);

	if (!file.good())
	{
		rdPtr->rRd->GenerateEvent(3);
		return;
	}

	// Header: Magic number and format version
	file << MAGIC << VER;

	// Write map block
	if (rdPtr->blocks & BLOCK_MAP)
	{
		file.beginBlock(MAP_);

		file << (unsigned short)rdPtr->properties->size();

		map<string, Property>::iterator it = rdPtr->properties->begin();
		for (; it != rdPtr->properties->end(); ++it)
		{
			// Name
			file.writeShortStr(it->first.c_str());
			// Type
			file.put(it->second.type);
			// Value
			if (it->second.type == TMPT_STRING)
				file.writeLongStr(it->second.c);
			else
				file << it->second.i;
		}

		file.endBlock();
	}

	// Write tileset block
	if (rdPtr->blocks & BLOCK_TILESETS)
	{
		file.beginBlock(TILE);
		unsigned char tilesetCount = min(255, rdPtr->tilesets->size());	
		file.put(tilesetCount);

		for (unsigned i = 0; i < tilesetCount; ++i)
		{
			Tileset* tileset = &(*rdPtr->tilesets)[i];

			// Transparent colour
			file << tileset->transpCol;

			// Store file path (relative to app, map or absolute)
			char path[256] = {};

			switch (rdPtr->tilesetPathMode)
			{
				case TSPM_APP_PATH:
					tileset->getPathRelativeTo(rdPtr->appPath, path);
					break;

				case TSPM_MAP_PATH:
					tileset->getPathRelativeTo(mapPath, path, true);
					break;

				case TSPM_USER_PATH:
					tileset->getPathRelativeTo(rdPtr->tilesetUserPath, path);
					break;

				default:
					strcpy_s(path, 256, tileset->getPath());
			}

			// Write compueted relative path
			file.writeShortStr(path);
		}

		file.endBlock();
	}

	// Write layer block
	if (rdPtr->blocks & BLOCK_LAYERS)
	{
		file.beginBlock(LAYR);

		unsigned short layerCount = min(0xffff, rdPtr->layers->size());
		file << layerCount;

		for (unsigned i = 0; i < layerCount; ++i)
		{
			Layer* layer = &(*rdPtr->layers)[i];

			// General settings
			file << layer->getWidth() << layer->getHeight();
			file << layer->settings.tileWidth << layer->settings.tileHeight;
			file << layer->settings.tileset << layer->settings.collision;
			file << layer->settings.offsetX << layer->settings.offsetY;
			file << layer->settings.scrollX << layer->settings.scrollY;
			file << layer->settings.wrapX << layer->settings.wrapY;
			file << layer->settings.visible;
			file << layer->settings.opacity;

			// Special sub-layer indices
			file << layer->settings.subLayerLink;

			// Number of data blocks
			file.put(1 + layer->subLayers.size());

			// Main block (tiles)
			file << MAIN;
			file.writeCompressedData((unsigned char*)layer->getDataPointer(), layer->getByteSize());

			// Sub blocks
			for (unsigned s = 0; s < layer->subLayers.size(); ++s)
			{
				SubLayer& subLayer = layer->subLayers[s];
				file << DATA;
				file << subLayer.getCellSize();
				file << subLayer.getDefaultValue();
				file.writeCompressedData((unsigned char*)subLayer.getDataPointer(), subLayer.getByteSize());
			}
		}

		file.endBlock();
	}

	rdPtr->rRd->GenerateEvent(1);
}

ACTION(
	/* ID */			23,
	/* Name */			"Clear layer tile at (%0, %1)",
	/* Flags */			0,
	/* Params */		(2, PARAM_NUMBER,"Tile X", PARAM_NUMBER,"Tile Y")
) {
	if (rdPtr->currentLayer)
	{
		unsigned x = intParam();
		unsigned y = intParam();

		Layer* layer = rdPtr->currentLayer;
		if (layer->isValid(x, y))
		{
			Tile* tile = layer->getTile(x, y);
			tile->id = Tile::EMPTY;

			rdPtr->redraw = true;
		}
	}
}


ACTION(
	/* ID */			24,
	/* Name */			"Fill layer area (%0, %1) => (%2, %3) with tile (%4, %5)",
	/* Flags */			0,
	/* Params */		(6, PARAM_NUMBER,"Top-left tile X", PARAM_NUMBER,"Top-left tile Y",
	PARAM_NUMBER,"Bottom-right tile X", PARAM_NUMBER,"Bottom-right tile Y",
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

		int width = rdPtr->currentLayer->getWidth();
		int height = rdPtr->currentLayer->getHeight();

		if (tlX >= width || tlY >= height || brX < 0 || brY < 0)
			return;

		tlX = max(0, tlX);
		tlY = max(0, tlY);
		brX = min(width-1, brX);
		brY = min(height-1, brY);

		if (brX-tlX < 0 || brY-tlY < 0)
			return;

		Layer* layer = rdPtr->currentLayer;
		for (int x = 0; x <= brX-tlX; ++x)
		{
			for (int y = 0; y <= brY-tlY; ++y)
			{
				Tile* tile = layer->getTile(tlX + x, tlY + y);
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
	/* Name */			"Add new layer with size (%0, %1)",
	/* Flags */			0,
	/* Params */		(2,PARAM_NUMBER,"Layer width (tiles)", PARAM_NUMBER,"Layer height (tiles)")
) {
	rdPtr->layers->push_back(Layer());
	rdPtr->currentLayer = &rdPtr->layers->back();
	rdPtr->currentLayer->settings.tileWidth = rdPtr->tileWidth;
	rdPtr->currentLayer->settings.tileHeight = rdPtr->tileHeight;
	ActionFunc1(rdPtr, param1, param2);
}

ACTION(
	/* ID */			30,
	/* Name */			"Set current tileset to %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_NUMBER,"Tileset index")
) {
	unsigned id = intParam();

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
		int layerWidth = layer->getWidth();
		int layerHeight = layer->getHeight();

		// Get cursor size
		unsigned width = rdPtr->cursor.width;
		unsigned height = rdPtr->cursor.height;

		int x1 = param1;
		int y1 = param2;

		// Single tile, avoid loop
		if (width == 1 && height == 1)
		{
			if (x1 >= 0 && y1 >= 0 && x1 < layerWidth && y1 < layerHeight)
			{
				Tile* tile = layer->getTile(x1, y1);
				tile->x = rdPtr->cursor.tiles.a.x;
				tile->y = rdPtr->cursor.tiles.b.y;
			}
			
			return;
		}

		// Rectangular area
		int x2 = x1 + width - 1;
		int y2 = y1 + height - 1;

		if (x1 >= (int)layerWidth || y1 >= (int)layerHeight || x2 < 0 || y2 < 0)
			return;

		// Limit area to layer data
		x1 = max(0, x1);
		y1 = max(0, y1);
		x2 = min(layerWidth - 1, x2);
		y2 = min(layerHeight - 1, y2);

		// Get tile range and pattern
		TileRange tiles = rdPtr->cursor.tiles;
		unsigned char patternX = rdPtr->cursor.patternX;
		unsigned char patternY = rdPtr->cursor.patternY;

		for (int x = 0; x <= x2-x1; ++x)
		{
			for (int y = 0; y <= y2-y1; ++y)
			{
				Tile* tile = layer->getTile(x + x1, y + y1);

				// X value
				if (tiles.b.x - tiles.a.x)
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
				if (tiles.b.y - tiles.a.y)
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
	/* Params */		(2, PARAM_NUMBER, "Tileset A (0-255)", PARAM_NUMBER, "Tileset B (0-255)")
) {
	unsigned a = intParam(), b = intParam();
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
	/* Params */		(1, PARAM_NUMBER,"Tileset index (0-255)")
) {
	if (rdPtr->currentLayer)
	{
		rdPtr->currentLayer->settings.tileset = (unsigned char)intParam();
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
		int layerWidth = layer->getWidth();
		int layerHeight = layer->getHeight();

		// Get cursor size
		unsigned width = rdPtr->cursor.width;
		unsigned height = rdPtr->cursor.height;

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
				Tile* tile = layer->getTile(x+x1, y+y1);
				Tile* src = layer->getTile(x+srcX, y+srcY);
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
		rdPtr->cursor.width = rdPtr->currentLayer->getWidth();
		rdPtr->cursor.height = rdPtr->currentLayer->getHeight();
	}
}

ACTION(
	/* ID */			40,
	/* Name */			"Set file block options to %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_NUMBER, "Blocks to load/save, add together (1: Properties, 2: Layers, 4: Tilesets, Default: 2)")
) {
	rdPtr->blocks = param1;
}

ACTION(
	/* ID */			41,
	/* Name */			"Delete tileset %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_NUMBER, "Tileset index")
) {
	unsigned i = Param(TYPE_INT);

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
	unsigned i = Param(TYPE_INT);

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
		memset(rdPtr->currentLayer->getDataPointer(), Tile::EMPTY & 0xff, rdPtr->currentLayer->getWidth()*rdPtr->currentLayer->getHeight()*sizeof(Tile));
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

		Layer* layer = rdPtr->currentLayer;
		int width = layer->getWidth();
		int height = layer->getHeight();
		for (int x = 0; x < width; ++x)
		{
			for (int y = 0; y < height; ++y)
			{
				Tile* tile = layer->getTile(x, y);
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
	int x1 = intParam();
	int y1 = intParam();
	int x2 = intParam();
	int y2 = intParam();

	if (x1 > x2)
		swap(x1, x2);

	if (y1 > y2)
		swap(y1, y2);

	rdPtr->cursor.x = x1;
	rdPtr->cursor.y = y1;
	rdPtr->cursor.width = x2 - x1;
	rdPtr->cursor.height = y2 - y1;
}

short WINAPI DLLExport ActionFunc58(LPRDATA, long, long);
ACTION(
	/* ID */			48,
	/* Name */			"Add small sub-layer",
	/* Flags */			0,
	/* Params */		(0)
) {
	ActionFunc58(rdPtr, 1, 0);
}

ACTION(
	/* ID */			49,
	/* Name */			"Fill sub-layer %0 cells at cursor with %1",
	/* Flags */			0,
	/* Params */		(2, PARAM_NUMBER, "Sub-layer index", PARAM_NUMBER, "Value")
) {
	unsigned s = intParam();
	int value = intParam();

	Layer* layer = rdPtr->currentLayer;
	if (layer && layer->isValid() && s < layer->subLayers.size())
	{
		SubLayer& sub = layer->subLayers[s];

		// Get layer size
		int layerWidth = layer->getWidth();
		int layerHeight = layer->getHeight();

		// Get cursor size
		unsigned width = rdPtr->cursor.width;
		unsigned height = rdPtr->cursor.height;

		int x1 = rdPtr->cursor.x;
		int y1 = rdPtr->cursor.y;

		if (x1 >= (int)layerWidth || y1 >= (int)layerHeight)
			return;

		// Rectangular area
		int x2 = x1 + width-1;
		int y2 = y1 + height-1;

		if (x2 < 0 || y2 < 0)
			return;

		// Limit area to layer data
		x1 = max(0, x1);
		y1 = max(0, y1);
		x2 = min(layerWidth-1, x2);
		y2 = min(layerHeight-1, y2);

		for (int x = 0; x <= x2-x1; ++x)
		{
			for (int y = 0; y <= y2-y1; ++y)
			{
				sub.setCell(x+x1, y+y1, value);
			}
		}
	}
}

ACTION(
	/* ID */			50,
	/* Name */			"Set tileset origin to %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_NUMBER, "Origin (0: Absolute, 1: Application folder, 2: Map file folder, 3: User path, 4: Custom (no physical file)")
) {
	rdPtr->tilesetPathMode = (TSPMODE)param1;
}

ACTION(
	/* ID */			51,
	/* Name */			"Set tileset user path to %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_STRING, "User path (used for the 'User path' tileset path mode)")
) {
	strcpy_s(rdPtr->tilesetUserPath, 256, (const char*)param1);
}

ACTION(
	/* ID */			52,
	/* Name */			"Set map property %0 to integer %1",
	/* Flags */			0,
	/* Params */		(2, PARAM_STRING, "Property name", PARAM_NUMBER, "Integer value")
) {
	string id = (const char*)strParam();
	int value = intParam();
	rdPtr->properties->erase(id);
	rdPtr->properties->insert(pair<string, Property>(id, value));
}

ACTION(
	/* ID */			53,
	/* Name */			"Set map property %0 to float %1",
	/* Flags */			0,
	/* Params */		(2, PARAM_STRING, "Property name", PARAM_NUMBER, "Float value")
) {
	string id = (const char*)strParam();
	float  value = fltParam();
	rdPtr->properties->erase(id);
	rdPtr->properties->insert(pair<string, Property>(id, value));
}

ACTION(
	/* ID */			54,
	/* Name */			"Set map property %0 to %1",
	/* Flags */			0,
	/* Params */		(2, PARAM_STRING, "Property name", PARAM_STRING, "String value")
) {
	string id = strParam();
	char* value = (char*)strParam();
	rdPtr->properties->erase(id);
	rdPtr->properties->insert(pair<string, Property>(id, value));
}

ACTION(
	/* ID */			55,
	/* Name */			"Delete map property %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_STRING, "Property name")
) {
	string id = strParam();
	rdPtr->properties->erase(id);
}

ACTION(
	/* ID */			56,
	/* Name */			"Delete all map properties",
	/* Flags */			0,
	/* Params */		(0)
) {
	rdPtr->properties->clear();
}

ACTION(
	/* ID */			57,
	/* Name */			"Iterate over map properties",
	/* Flags */			0,
	/* Params */		(0)
) {
	map<string, Property>::iterator it = rdPtr->properties->begin();
	for (; it != rdPtr->properties->end(); ++it)
	{
		rdPtr->onProperty = it->first.c_str();
		rdPtr->rRd->GenerateEvent(4);
	}

	rdPtr->onProperty = 0;
}

ACTION(
	/* ID */			58,
	/* Name */			"Add sub-layer with cell size %0 and default value %1",
	/* Flags */			0,
	/* Params */		(2, PARAM_NUMBER,  "Cell size in bytes", PARAM_NUMBER, "Default value, used for new and blank tiles")
) {
	if (rdPtr->currentLayer)
	{
		rdPtr->currentLayer->subLayers.push_back(SubLayer(param1, param2));
		rdPtr->currentLayer->subLayers.back().resize(
			rdPtr->currentLayer->getWidth(),
			rdPtr->currentLayer->getHeight()
		);
	}
}

ACTION(
	/* ID */			59,
	/* Name */			"Set sub-layer %0 value at (%1, %2) to %3",
	/* Flags */			0,
	/* Params */		(4, PARAM_NUMBER, "Sub-layer index", PARAM_NUMBER, "Cell X", PARAM_NUMBER, "Cell Y", PARAM_NUMBER, "Value")
) {
	unsigned s = intParam();
	int x = intParam();
	int y = intParam();
	int value = intParam();

	Layer* layer = rdPtr->currentLayer;
	if (layer && s < layer->subLayers.size())
	{
		SubLayer& sub = layer->subLayers[s];
		
		if (sub.isValid(x, y))
		{
			sub.setCell(x, y, value);
		}
	}
}

ACTION(
	/* ID */			60,
	/* Name */			"Add medium sub-layer",
	/* Flags */			0,
	/* Params */		(0)
) {
	ActionFunc58(rdPtr, 2, 0);
}

ACTION(
	/* ID */			61,
	/* Name */			"Add large sub-layer",
	/* Flags */			0,
	/* Params */		(0)
) {
	ActionFunc58(rdPtr, 4, 0);
}

ACTION(
	/* ID */			62,
	/* Name */			"Add small sub-layer with default value %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_NUMBER, "Default value, used for new and blank tiles")
) {
	ActionFunc58(rdPtr, 1, param1);
}

ACTION(
	/* ID */			63,
	/* Name */			"Add sub-layer with cell size %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_NUMBER, "Cell size in bytes (E.g. 1: Small, 2: Medium, 4: Large)")
) {
	ActionFunc58(rdPtr, param1, 0);
}

ACTION(
	/* ID */			64,
	/* Name */			"Set sub-layer %0 value at (%1, %2) to float %3",
	/* Flags */			0,
	/* Params */		(4, PARAM_NUMBER, "Sub-layer index", PARAM_NUMBER, "Cell X", PARAM_NUMBER, "Cell Y", PARAM_NUMBER, "Float value")
) {
	unsigned s = intParam();
	int x = intParam();
	int y = intParam();
	float value = fltParam();

	Layer* layer = rdPtr->currentLayer;
	if (layer && s < layer->subLayers.size())
	{
		SubLayer& sub = layer->subLayers[s];
		
		if (sub.isValid(x, y))
		{
			sub.setCell(x, y, *(unsigned int*)&value);
		}
	}
}

ACTION(
	/* ID */			65,
	/* Name */			"Fill sub-layer %0 with value %1",
	/* Flags */			0,
	/* Params */		(2, PARAM_NUMBER, "Sub-layer index", PARAM_NUMBER, "Value")
) {
	unsigned s = intParam();
	int value = intParam();

	Layer* layer = rdPtr->currentLayer;
	if (layer && s < layer->subLayers.size())
	{
		SubLayer& sub = layer->subLayers[s];
		
		unsigned width = sub.getWidth(), height = sub.getHeight();
		for (unsigned x = 0; x < width; ++x)
			for (unsigned y = 0; y < height; ++y)
				sub.setCell(x, y, value);
	}
}

ACTION(
	/* ID */			66,
	/* Name */			"Fill sub-layer %0 with float value %1",
	/* Flags */			0,
	/* Params */		(2, PARAM_NUMBER, "Sub-layer index", PARAM_NUMBER, "Float value")
) {
	unsigned s = intParam();
	float value = fltParam();

	Layer* layer = rdPtr->currentLayer;
	if (layer && s < layer->subLayers.size())
	{
		SubLayer& sub = layer->subLayers[s];
		
		unsigned width = sub.getWidth(), height = sub.getHeight();
		for (unsigned x = 0; x < width; ++x)
			for (unsigned y = 0; y < height; ++y)
				sub.setCell(x, y, *(unsigned int*)&value);
	}
}

ACTION(
	/* ID */			67,
	/* Name */			"Clear sub-layer %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_NUMBER, "Sub-layer index")
) {
	unsigned s = intParam();

	Layer* layer = rdPtr->currentLayer;
	if (layer && s < layer->subLayers.size())
	{
		SubLayer& sub = layer->subLayers[s];
		
		unsigned width = sub.getWidth(), height = sub.getHeight();
		unsigned value = sub.getDefaultValue();
		for (unsigned x = 0; x < width; ++x)
			for (unsigned y = 0; y < height; ++y)
				sub.setCell(x, y, value);
	}
}

ACTION(
	/* ID */			68,
	/* Name */			"Set layer animation link to %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_NUMBER,"Animation sub-layer index (-1: None)")
) {
	if (rdPtr->currentLayer)
	{
		rdPtr->currentLayer->settings.subLayerLink.animation = (unsigned char)intParam();
		rdPtr->redraw = true;
	}
}

ACTION(
	/* ID */			69,
	/* Name */			"Set layer tileset link to %0",
	/* Flags */			0,
	/* Params */		(1, PARAM_NUMBER,"Tileset sub-layer index (-1: None)")
) {
	if (rdPtr->currentLayer)
	{
		rdPtr->currentLayer->settings.subLayerLink.tileset = (unsigned char)intParam();
		rdPtr->redraw = true;
	}
}

ACTION(
	/* ID */			70,
	/* Name */			"Fill sub-layer %0 cells at cursor with %1",
	/* Flags */			0,
	/* Params */		(2, PARAM_NUMBER, "Sub-layer index", PARAM_NUMBER, "Value")
) {
	int i = intParam();
	float value = fltParam();
	ActionFunc49(rdPtr, i, *(int*)&value);
}

// ============================================================================
//
// EXPRESSIONS
// 
// ============================================================================

EXPRESSION(
	/* ID */			0,
	/* Name */			"TilesetWidth(",
	/* Flags */			0,
	/* Params */		(1, EXPPARAM_NUMBER, "Tileset index")
) {
	unsigned i = ExParam(TYPE_INT);

	if (i < rdPtr->tilesets->size())
	{
		if ((*rdPtr->tilesets)[i].isValid())
			return (*rdPtr->tilesets)[i].surface->GetWidth();
	}
	
	return 0;
}

EXPRESSION(
	/* ID */			1,
	/* Name */			"TilesetHeight(",
	/* Flags */			0,
	/* Params */		(1, EXPPARAM_NUMBER, "Tileset index")
) {
	unsigned i = ExParam(TYPE_INT);

	if (i < rdPtr->tilesets->size())
	{
		if ((*rdPtr->tilesets)[i].isValid())
			return (*rdPtr->tilesets)[i].surface->GetHeight();
	}
	
	return 0;
}

EXPRESSION(
	/* ID */			2,
	/* Name */			"LayerWidth(",
	/* Flags */			0,
	/* Params */		(1, EXPPARAM_NUMBER, "Layer index")
) {
	unsigned i = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
		return (*rdPtr->layers)[i].getWidth();
	
	return 0;
}

EXPRESSION(
	/* ID */			3,
	/* Name */			"LayerHeight(",
	/* Flags */			0,
	/* Params */		(1, EXPPARAM_NUMBER, "Layer index")
) {
	unsigned i = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
		return (*rdPtr->layers)[i].getHeight();
	
	return 0;
}

EXPRESSION(
	/* ID */			4,
	/* Name */			"LayerWrapX(",
	/* Flags */			0,
	/* Params */		(1, EXPPARAM_NUMBER, "Layer index")
) {
	unsigned i = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
		return (*rdPtr->layers)[i].settings.wrapX;
	
	return 0;
}

EXPRESSION(
	/* ID */			5,
	/* Name */			"LayerWrapY(",
	/* Flags */			0,
	/* Params */		(1, EXPPARAM_NUMBER, "Layer index")
) {
	unsigned i = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
		return (*rdPtr->layers)[i].settings.wrapY;
	
	return 0;
}

EXPRESSION(
	/* ID */			6,
	/* Name */			"LayerOffsetX(",
	/* Flags */			0,
	/* Params */		(1, EXPPARAM_NUMBER, "Layer index")
) {
	unsigned i = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
		return (*rdPtr->layers)[i].settings.offsetX;
	
	return 0;
}

EXPRESSION(
	/* ID */			7,
	/* Name */			"LayerOffsetY(",
	/* Flags */			0,
	/* Params */		(1, EXPPARAM_NUMBER, "Layer index")
) {
	unsigned i = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
		return (*rdPtr->layers)[i].settings.offsetY;
	
	return 0;
}

EXPRESSION(
	/* ID */			8,
	/* Name */			"LayerScrollX(",
	/* Flags */			EXPFLAG_DOUBLE,
	/* Params */		(1, EXPPARAM_NUMBER, "Layer index")
) {
	unsigned i = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
	{
		// Stupid macro that expands to TWO lines!
		ReturnFloat((*rdPtr->layers)[i].settings.scrollX);
	}
	
	ReturnFloat(0);
}

EXPRESSION(
	/* ID */			9,
	/* Name */			"LayerScrollY(",
	/* Flags */			EXPFLAG_DOUBLE,
	/* Params */		(1, EXPPARAM_NUMBER, "Layer index")
) {
	unsigned i = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
	{
		// Stupid macro that expands to TWO lines!
		ReturnFloat((*rdPtr->layers)[i].settings.scrollY);
	}
	
	ReturnFloat(0);
}

EXPRESSION(
	/* ID */			10,
	/* Name */			"LayerTileWidth(",
	/* Flags */			0,
	/* Params */		(1, EXPPARAM_NUMBER, "Layer index")
) {
	unsigned i = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
		return (*rdPtr->layers)[i].settings.tileWidth;
	
	return 0;
}

EXPRESSION(
	/* ID */			11,
	/* Name */			"LayerTileHeight(",
	/* Flags */			0,
	/* Params */		(1, EXPPARAM_NUMBER, "Layer index")
) {
	unsigned i = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
		return (*rdPtr->layers)[i].settings.tileHeight;
	
	return 0;
}

EXPRESSION(
	/* ID */			12,
	/* Name */			"LayerTilesetX(",
	/* Flags */			0,
	/* Params */		(3, EXPPARAM_NUMBER, "Layer index", EXPPARAM_NUMBER, "Tile X", EXPPARAM_NUMBER, "Tile Y")
) {
	unsigned i = ExParam(TYPE_INT);
	unsigned x = ExParam(TYPE_INT);
	unsigned y = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
	{
		Layer* layer = &(*rdPtr->layers)[i];

		if (x < layer->getWidth() && y < layer->getHeight())
		{
			int value = layer->getTile(x, y)->x;
			return value == 0xff ? -1 : value;
		}
	}
	
	return -1;
}

EXPRESSION(
	/* ID */			13,
	/* Name */			"LayerTilesetY(",
	/* Flags */			0,
	/* Params */		(3, EXPPARAM_NUMBER, "Layer index", EXPPARAM_NUMBER, "Tile X", EXPPARAM_NUMBER, "Tile Y")
) {
	unsigned i = ExParam(TYPE_INT);
	unsigned x = ExParam(TYPE_INT);
	unsigned y = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
	{
		Layer* layer = &(*rdPtr->layers)[i];

		if (x < layer->getWidth() && y < layer->getHeight())
		{
			int value = layer->getTile(x, y)->y;
			return value == 0xff ? -1 : value;
		}
	}
	
	return -1;
}

EXPRESSION(
	/* ID */			14,
	/* Name */			"LayerTile(",
	/* Flags */			0,
	/* Params */		(3, EXPPARAM_NUMBER, "Layer index", EXPPARAM_NUMBER, "Tile X", EXPPARAM_NUMBER, "Tile Y")
) {
	unsigned i = ExParam(TYPE_INT);
	unsigned x = ExParam(TYPE_INT);
	unsigned y = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
	{
		Layer* layer = &(*rdPtr->layers)[i];

		if (x < layer->getWidth() && y < layer->getHeight())
		{
			Tile* tile = layer->getTile(x, y);
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
	unsigned i = ExParam(TYPE_INT);
	unsigned x = ExParam(TYPE_INT);
	unsigned y = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
	{
		Layer* layer = &(*rdPtr->layers)[i];

		if (x < layer->getWidth() && y < layer->getHeight())
			return layer->getTile(x, y)->id;
	}
	
	return Tile::EMPTY;
}

EXPRESSION(
	/* ID */			18,
	/* Name */			"TilesetAbsPath$(",
	/* Flags */			EXPFLAG_STRING,
	/* Params */		(1, EXPPARAM_NUMBER, "Tileset index")
) {
	unsigned i = ExParam(TYPE_INT);

	if (i < rdPtr->tilesets->size())
	{
		if ((*rdPtr->tilesets)[i].isValid())
		{
			ReturnString((*rdPtr->tilesets)[i].getPath());
		}
	}
	
	ReturnString("");
}

EXPRESSION(
	/* ID */			19,
	/* Name */			"MapIntProp(",
	/* Flags */			0,
	/* Params */		(1, EXPPARAM_STRING, "Property name")
) {
	string id = (const char*)ExParam(TYPE_STRING);

	map<string, Property>::iterator it = rdPtr->properties->find(id);
	if (it != rdPtr->properties->end())
	{
		if (it->second.type == TMPT_INT)
			return it->second.i;
		if (it->second.type == TMPT_FLOAT)
			return (int)it->second.f;

		return atoi(it->second.c);
	}

	return 0;
}

EXPRESSION(
	/* ID */			20,
	/* Name */			"MapFloatProp(",
	/* Flags */			0,
	/* Params */		(1, EXPPARAM_STRING, "Property name")
) {
	string id = (const char*)ExParam(TYPE_STRING);
	
	float out = 0.0f;

	map<string, Property>::iterator it = rdPtr->properties->find(id);
	if (it != rdPtr->properties->end())
	{
		if (it->second.type == TMPT_FLOAT)
			out = it->second.f;
		else if (it->second.type == TMPT_INT)
			out = (float)it->second.i;
		else
			out = (float)atof(it->second.c);
	}

	ReturnFloat(out);
}

EXPRESSION(
	/* ID */			21,
	/* Name */			"MapStrProp$(",
	/* Flags */			EXPFLAG_STRING,
	/* Params */		(1, EXPPARAM_STRING, "Property name")
) {
	string id = (const char*)ExParam(TYPE_STRING);

	map<string, Property>::iterator it = rdPtr->properties->find(id);
	if (it != rdPtr->properties->end())
	{
		if (it->second.type == TMPT_STRING)
		{	
			ReturnString(it->second.i);
		}
		else
		{
			char buff[32] = {};

			if (it->second.type == TMPT_FLOAT)
				sprintf_s(buff, 32, "%f", it->second.f);
			else
				sprintf_s(buff, 32, "%d", it->second.i);

			ReturnStringSafe(buff);
		}
	}

	ReturnString("");
}

EXPRESSION(
	/* ID */			22,
	/* Name */			"MapIterProp$(",
	/* Flags */			EXPFLAG_STRING,
	/* Params */		(0)
) {
	ReturnString((rdPtr->onProperty ? rdPtr->onProperty : ""));
}

EXPRESSION(
	/* ID */			23,
	/* Name */			"SubLayerByteVal(",
	/* Flags */			0,
	/* Params */		(4, EXPPARAM_NUMBER, "Layer index", EXPPARAM_NUMBER, "Sub-layer index", EXPPARAM_NUMBER, "Tile X", EXPPARAM_NUMBER, "Tile Y")
) {
	unsigned i = ExParam(TYPE_INT);
	unsigned s = ExParam(TYPE_INT);
	unsigned x = ExParam(TYPE_INT);
	unsigned y = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
	{
		Layer& layer = (*rdPtr->layers)[i];

		if (x < layer.getWidth() && y < layer.getHeight())
		{
			if (s < layer.subLayers.size())
			{
				SubLayer& subLayer = layer.subLayers[s];

				return *subLayer.getCell(x, y);
			}
		}
	}
	
	return 0;
}

EXPRESSION(
	/* ID */			24,
	/* Name */			"SubLayerIntVal(",
	/* Flags */			0,
	/* Params */		(4, EXPPARAM_NUMBER, "Layer index", EXPPARAM_NUMBER, "Sub-layer index", EXPPARAM_NUMBER, "Tile X", EXPPARAM_NUMBER, "Tile Y")
) {
	unsigned i = ExParam(TYPE_INT);
	unsigned s = ExParam(TYPE_INT);
	unsigned x = ExParam(TYPE_INT);
	unsigned y = ExParam(TYPE_INT);

	if (i < rdPtr->layers->size())
	{
		Layer& layer = (*rdPtr->layers)[i];

		if (x < layer.getWidth() && y < layer.getHeight())
		{
			if (s < layer.subLayers.size())
			{
				SubLayer& subLayer = layer.subLayers[s];

				return subLayer.getCellAs<int>(x, y);
			}
		}
	}
	
	return 0;
}

EXPRESSION(
	/* ID */			25,
	/* Name */			"SubLayerFloatVal(",
	/* Flags */			EXPFLAG_DOUBLE,
	/* Params */		(4, EXPPARAM_NUMBER, "Layer index", EXPPARAM_NUMBER, "Sub-layer index", EXPPARAM_NUMBER, "Tile X", EXPPARAM_NUMBER, "Tile Y")
) {
	unsigned i = ExParam(TYPE_INT);
	unsigned s = ExParam(TYPE_INT);
	unsigned x = ExParam(TYPE_INT);
	unsigned y = ExParam(TYPE_INT);

	rdPtr->rHo.hoFlags|=HOF_FLOAT; // MUST be done after getting params

	if (i < rdPtr->layers->size())
	{
		Layer& layer = (*rdPtr->layers)[i];

		if (x < layer.getWidth() && y < layer.getHeight())
		{
			if (s < layer.subLayers.size())
			{
				SubLayer& subLayer = layer.subLayers[s];

				return subLayer.getCellAs<int>(x, y);
			}
		}
	}
	
	return 0;
}