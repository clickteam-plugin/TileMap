// ===========================================================================
//
// This file contains routines that are handled during the Runtime
//
// ============================================================================

// Common Include
#include "Common.h"
#include "HWASurface.h"

std::size_t getLayerCount(TILEMAP * tileMap) { return tileMap->layers->size(); }

Layer * getLayerAt(TILEMAP * tileMap, std::size_t i)
{
    return i < tileMap->layers->size() ? &(*tileMap->layers)[i] : NULL;
}

std::size_t getLayerSubLayerCount(Layer * layer) { return layer->subLayers.size(); }

SubLayer * getLayerSubLayerAt(Layer * layer, std::size_t i)
{
    return i < layer->subLayers.size() ? &layer->subLayers[i] : NULL;
}

// --------------------
// GetRunObjectDataSize
// --------------------
// Returns the size of the runtime datazone of the object
//
short WINAPI DLLExport GetRunObjectDataSize(fprh rhPtr, LPEDATA edPtr)
{
    return (sizeof(RUNDATA));
}

// ---------------
// CreateRunObject
// ---------------
// The routine where the object is actually created
//
short WINAPI DLLExport CreateRunObject(LPRDATA rdPtr, LPEDATA edPtr, fpcob cobPtr)
{
// Do some rSDK stuff
#include "rCreateRunObject.h"

#ifdef _DEBUG

    AllocConsole();
    freopen("conin$", "r", stdin);
    freopen("conout$", "w", stdout);
    freopen("conout$", "w", stderr);

    printf("TILEMAP DEBUG MODE");

#ifdef HWABETA
    printf(" (HWA)");

#endif

    printf("\n");
#endif

    LPRH rhPtr = rdPtr->rHo.hoAdRunHeader;

    // Initialize function interface
    rdPtr->getLayerAt = getLayerAt;
    rdPtr->getLayerCount = getLayerCount;
    rdPtr->getLayerSubLayerAt = getLayerSubLayerAt;
    rdPtr->getLayerSubLayerCount = getLayerSubLayerCount;

    // Attached viewports
    rdPtr->viewports = new list<TMAPVIEW *>;
    rdPtr->redraw = false;

    // Create surface, get MMF depth..
    cSurface * ps = WinGetSurface((int)rdPtr->rHo.hoAdRunHeader->rhIdEditWin);

    if (!ps)
        return 1;

    rdPtr->depth = ps->GetDepth();
    cSurface * proto = getPrototype(rdPtr->depth);

    // Database
    rdPtr->layers = new vector<Layer>;
    rdPtr->tilesets = new vector<Tileset>;
    rdPtr->properties = new map<string, Property>;
    rdPtr->currentLayer = 0;
    rdPtr->currentTileset = 0;

    // Load edit tilesets
    cSurface is;
    for (int i = 0; i < edPtr->tilesetCount; ++i) {
        rdPtr->tilesets->push_back(Tileset());
        Tileset & tileset = rdPtr->tilesets->back();
		rdPtr->currentTileset->setRenderMode(ps->GetDriver());

        // Create a tileset for each image
        if (LockImageSurface(rhPtr->rhIdAppli, edPtr->tilesets[i], is)) {
            tileset.transpCol = is.GetTransparentColor();
            tileset.surface = new cSurface;
            tileset.surface->Create(is.GetWidth(), is.GetHeight(), proto);

            copyBlit(is, *tileset.surface);

            tileset.surface->SetTransparentColor(is.GetTransparentColor());
            UnlockImageSurface(is);

            tileset.updateTexture();
        }
    }

    // Select 0th tileset by default...
    if (edPtr->tilesetCount)
        rdPtr->currentTileset = &rdPtr->tilesets->front();

    // Tileset settings
    rdPtr->tileWidth = edPtr->tileWidth;
    rdPtr->tileHeight = edPtr->tileHeight;

    rdPtr->blocks = (edPtr->blockMap ? BLOCK_MAP : 0) | (edPtr->blockLayers ? BLOCK_LAYERS : 0) |
                    (edPtr->blockTilesets ? BLOCK_TILESETS : 0);

    // Default compression level
    rdPtr->compress = 6;

    // How to save tileset paths (by default, use path relative to map file)
    rdPtr->tilesetPathMode = (TSPMODE)edPtr->tilesetPathMode;
    callRunTimeFunction(rdPtr, RFUNCTION_GETFILEINFOS, FILEINFO_PATH, (long)&rdPtr->appPath[0]);

    // Set up tile cursor
    rdPtr->cursor.x = 0;
    rdPtr->cursor.y = 0;
    rdPtr->cursor.width = 1;
    rdPtr->cursor.height = 1;
    rdPtr->cursor.tiles.a.id = 0;
    rdPtr->cursor.tiles.b.id = 0;
    rdPtr->cursor.patternX = 0;
    rdPtr->cursor.patternY = 0;

    rdPtr->onProperty = 0;

    return 0;
}

// ----------------
// DestroyRunObject
// ----------------
// Destroys the run-time object
//
short WINAPI DLLExport DestroyRunObject(LPRDATA rdPtr, long fast)
{
    delete rdPtr->viewports;
    delete rdPtr->layers;
    delete rdPtr->tilesets;
    delete rdPtr->properties;
    // No errors
    delete rdPtr->rRd;
    return 0;
}

// ----------------
// HandleRunObject
// ----------------
// Called (if you want) each loop, this routine makes the object live
//
short WINAPI DLLExport HandleRunObject(LPRDATA rdPtr)
{
    if (rdPtr->redraw) {
        // Redraw all attached viewports
        // list<TMAPVIEW*>::iterator it;
        // for (it = rdPtr->viewports->begin(); it != rdPtr->viewports->end();
        // ++it)
        //{
        //	if (((*it)->rHo.hoFlags & HOF_DESTROYED) == 0)
        //		rdPtr->rRd->LPRO_Redraw((LPRO)*it);
        //	else
        //		rdPtr->viewports->erase(it++);
        //}

        rdPtr->redraw = false;
    }

    return 0;
}

// ----------------
// DisplayRunObject
// ----------------
// Draw the object in the application screen.
//
short WINAPI DLLExport DisplayRunObject(LPRDATA rdPtr) { return 0; }

// -------------------
// GetRunObjectSurface
// -------------------
// Implement this function instead of DisplayRunObject if your extension
// supports ink effects and transitions. Note: you can support ink effects
// in DisplayRunObject too, but this is automatically done if you implement
// GetRunObjectSurface (MMF applies the ink effect to the transition).
//
// Note: do not forget to enable the function in the .def file
// if you remove the comments below.

// cSurface* WINAPI DLLExport GetRunObjectSurface(LPRDATA rdPtr)
//{
//	return 0;
//}

// -------------------------
// GetRunObjectCollisionMask
// -------------------------
// Implement this function if your extension supports fine collision mode
// (OEPREFS_FINECOLLISIONS),
// Or if it's a background object and you want Obstacle properties for this
// object.
//
// Should return NULL if the object is not transparent.
//
// Note: do not forget to enable the function in the .def file
// if you remove the comments below.
//
/*
cSurface* WINAPI DLLExport GetRunObjectCollisionMask(LPRDATA rdPtr, LPARAM
lParam)
{
        // Typical example for active objects
        // ----------------------------------
        // Opaque? collide with box
        if ( (rdPtr->rs.rsEffect & EFFECTFLAG_TRANSPARENT) == 0 )	//
Note: only if your
object has the OEPREFS_INKEFFECTS option
                return NULL;

        // Transparent? Create mask
        LPSMASK pMask = rdPtr->m_pColMask;
        if ( pMask == NULL )
        {
                if ( rdPtr->m_pSurface != NULL )
                {
                        DWORD dwMaskSize = rdPtr->m_pSurface->CreateMask(NULL,
lParam);
                        if ( dwMaskSize != 0 )
                        {
                                pMask = (LPSMASK)calloc(dwMaskSize, 1);
                                if ( pMask != NULL )
                                {
                                        rdPtr->m_pSurface->CreateMask(pMask,
lParam);
                                        rdPtr->m_pColMask = pMask;
                                }
                        }
                }
        }

        // Note: for active objects, lParam is always the same.
        // For background objects (OEFLAG_BACKGROUND), lParam maybe be different
if the user uses
your object
        // as obstacle and as platform. In this case, you should store 2
collision masks
        // in your data: one if lParam is 0 and another one if lParam is
different from 0.

        return pMask;
}
*/

// ----------------
// PauseRunObject
// ----------------
// Enters the pause mode
//
short WINAPI DLLExport PauseRunObject(LPRDATA rdPtr)
{
    // Ok
    return 0;
}

// -----------------
// ContinueRunObject
// -----------------
// Quits the pause mode
//
short WINAPI DLLExport ContinueRunObject(LPRDATA rdPtr)
{
    // Ok
    return 0;
}

// ============================================================================
//
// START APP / END APP / START FRAME / END FRAME routines
//
// ============================================================================

// -------------------
// StartApp
// -------------------
// Called when the application starts or restarts.
// Useful for storing global data
//
void WINAPI DLLExport StartApp(mv _far * mV, CRunApp * pApp)
{
    // Example
    // -------
    // Delete global data (if restarts application)
    //	CMyData* pData = (CMyData*)mV->mvGetExtUserData(pApp, hInstLib);
    //	if ( pData != NULL )
    //	{
    //		delete pData;
    //		mV->mvSetExtUserData(pApp, hInstLib, NULL);
    //	}
}

// -------------------
// EndApp
// -------------------
// Called when the application ends.
//
void WINAPI DLLExport EndApp(mv _far * mV, CRunApp * pApp)
{
    // Example
    // -------
    // Delete global data
    //	CMyData* pData = (CMyData*)mV->mvGetExtUserData(pApp, hInstLib);
    //	if ( pData != NULL )
    //	{
    //		delete pData;
    //		mV->mvSetExtUserData(pApp, hInstLib, NULL);
    //	}
}

// -------------------
// StartFrame
// -------------------
// Called when the frame starts or restarts.
//
void WINAPI DLLExport StartFrame(mv _far * mV, DWORD dwReserved, int nFrameIndex) {}

// -------------------
// EndFrame
// -------------------
// Called when the frame ends.
//
void WINAPI DLLExport EndFrame(mv _far * mV, DWORD dwReserved, int nFrameIndex) {}

// ============================================================================
//
// TEXT ROUTINES (if OEFLAG_TEXT)
//
// ============================================================================

// -------------------
// GetRunObjectFont
// -------------------
// Return the font used by the object.
//
/*

  // Note: do not forget to enable the functions in the .def file
  // if you remove the comments below.

void WINAPI GetRunObjectFont(LPRDATA rdPtr, LOGFONT* pLf)
{
        // Example
        // -------
        // GetObject(rdPtr->m_hFont, sizeof(LOGFONT), pLf);
}

// -------------------
// SetRunObjectFont
// -------------------
// Change the font used by the object.
//
void WINAPI SetRunObjectFont(LPRDATA rdPtr, LOGFONT* pLf, RECT* pRc)
{
        // Example
        // -------
//	HFONT hFont = CreateFontIndirect(pLf);
//	if ( hFont != NULL )
//	{
//		if (rdPtr->m_hFont!=0)
//			DeleteObject(rdPtr->m_hFont);
//		rdPtr->m_hFont = hFont;
//		SendMessage(rdPtr->m_hWnd, WM_SETFONT, (WPARAM)rdPtr->m_hFont,
FALSE);
//	}

}

// ---------------------
// GetRunObjectTextColor
// ---------------------
// Return the text color of the object.
//
COLORREF WINAPI GetRunObjectTextColor(LPRDATA rdPtr)
{
        // Example
        // -------
        return 0;	// rdPtr->m_dwColor;
}

// ---------------------
// SetRunObjectTextColor
// ---------------------
// Change the text color of the object.
//
void WINAPI SetRunObjectTextColor(LPRDATA rdPtr, COLORREF rgb)
{
        // Example
        // -------
        rdPtr->m_dwColor = rgb;
        InvalidateRect(rdPtr->m_hWnd, NULL, TRUE);
}
*/

// ============================================================================
//
// DEBUGGER ROUTINES
//
// ============================================================================

// -----------------
// GetDebugTree
// -----------------
// This routine returns the address of the debugger tree
//
LPWORD WINAPI DLLExport GetDebugTree(LPRDATA rdPtr)
{
#if !defined(RUN_ONLY)
    return DebugTree;
#else
    return NULL;
#endif // !defined(RUN_ONLY)
}

// -----------------
// GetDebugItem
// -----------------
// This routine returns the text of a given item.
//
void WINAPI DLLExport GetDebugItem(LPSTR pBuffer, LPRDATA rdPtr, int id)
{
#if !defined(RUN_ONLY)

// Example
// -------
/*
        char temp[DB_BUFFERSIZE];

        switch (id)
        {
        case DB_CURRENTSTRING:
                LoadString(hInstLib, IDS_CURRENTSTRING, temp, DB_BUFFERSIZE);
                wsprintf(pBuffer, temp, rdPtr->text);
                break;
        case DB_CURRENTVALUE:
                LoadString(hInstLib, IDS_CURRENTVALUE, temp, DB_BUFFERSIZE);
                wsprintf(pBuffer, temp, rdPtr->value);
                break;
        case DB_CURRENTCHECK:
                LoadString(hInstLib, IDS_CURRENTCHECK, temp, DB_BUFFERSIZE);
                if (rdPtr->check)
                        wsprintf(pBuffer, temp, "TRUE");
                else
                        wsprintf(pBuffer, temp, "FALSE");
                break;
        case DB_CURRENTCOMBO:
                LoadString(hInstLib, IDS_CURRENTCOMBO, temp, DB_BUFFERSIZE);
                wsprintf(pBuffer, temp, rdPtr->combo);
                break;
        }
*/

#endif // !defined(RUN_ONLY)
}

// -----------------
// EditDebugItem
// -----------------
// This routine allows to edit editable items.
//
void WINAPI DLLExport EditDebugItem(LPRDATA rdPtr, int id)
{
#if !defined(RUN_ONLY)

// Example
// -------
/*
        switch (id)
        {
        case DB_CURRENTSTRING:
                {
                        EditDebugInfo dbi;
                        char buffer[256];

                        dbi.pText=buffer;
                        dbi.lText=TEXT_MAX;
                        dbi.pTitle=NULL;

                        strcpy(buffer, rdPtr->text);
                        long ret=callRunTimeFunction(rdPtr, RFUNCTION_EDITTEXT,
   0, (LPARAM)&dbi);
                        if (ret)
                                strcpy(rdPtr->text, dbi.pText);
                }
                break;
        case DB_CURRENTVALUE:
                {
                        EditDebugInfo dbi;

                        dbi.value=rdPtr->value;
                        dbi.pTitle=NULL;

                        long ret=callRunTimeFunction(rdPtr, RFUNCTION_EDITINT,
   0, (LPARAM)&dbi);
                        if (ret)
                                rdPtr->value=dbi.value;
                }
                break;
        }
*/
#endif // !defined(RUN_ONLY)
}