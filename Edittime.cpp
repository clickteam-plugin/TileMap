// ============================================================================
//
// This file contains routines that are handled only during the Edittime,
// under the Frame and Event editors.
//
// Including creating, display, and setting up your object.
// 
// ============================================================================

#include "common.h"

// --------------------
// Properties
// --------------------

PROPS_IDS_START()
	
	PROPID_GRP_FILES,
	//PROPID_BLOCKMAP,
	PROPID_BLOCKTILE,
	PROPID_BLOCKLAYER,

	//PROPID_GRP_DISPLAY,
	//PROPID_AUTOSCROLL,
	//PROPID_TRANSPARENT,
	////PROPID_TRANSPCOLOR,
	//PROPID_BGCOLOR,

	//PROPID_GRP_MAP,

	PROPID_GRP_DATA,
	PROPID_TILESIZE,
	PROPID_TILESETS,
	PROPID_DELETESETS,

PROPS_IDS_END()

int tileSize[] = {
	8, 8,
	16, 16,
	24, 24,
	32, 32,
	48, 48,
	64, 64,
	96, 96,
	128, 128,
    0, 0
};

PROPS_DATA_START()


	//PropData_Group(PROPID_GRP_DISPLAY, (int)"Display", (int)""),
	//PropData_CheckBox(PROPID_TRANSPARENT, (int)"Transparent", (int)"Use with caution - a background may greatly increase the FPS, especially in HWA."),
	//PropData_CheckBox(PROPID_AUTOSCROLL, (int)"Follow MMF camera", (int)"If checked, the Tile Map automatically follows the MMF camera."),

	PropData_Group(PROPID_GRP_FILES, (int)"File blocks", (int)"Set up what kind of data should be loaded from and saved to files."),
	//PropData_CheckBox(PROPID_BLOCKMAP, (int)"Map", (int)"Stores general map information. Right now, this is only includes tile size."),
	PropData_CheckBox(PROPID_BLOCKLAYER, (int)"Layers", (int)"Stores each layer including its tiles and settings."),
	PropData_CheckBox(PROPID_BLOCKTILE, (int)"Tilesets", (int)"Stores every tileset with settings and path, but not the image itself."),

	// PropData_Group(PROPID_GRP_MAP, (int)"Map", (int)""),
	PropData_Group(PROPID_GRP_DATA, (int)"Data", (int)""),
	PropData_Size(PROPID_TILESIZE, (int)"Default tile size", (int)"The default size of each tile in a new layer.", tileSize),

	PropData_ImageList(PROPID_TILESETS, (int)"Tilesets",(int)""),
	PropData_Button(PROPID_DELETESETS,(int)"",(int)"",(int)"Delete all"),

PROPS_DATA_END()
//
//PropData BgColor[] =
//{
//	PropData_Color(PROPID_BGCOLOR, (int)"Background color", (int)""),
//	PropData_End()
//};

//PropData TranspColor[] =
//{
//	PropData_Color(PROPID_TRANSPCOLOR, (int)"Transparent color", (int)""),
//	PropData_End()
//};
// --------------------
// Debugger
// --------------------

DEBUGGER_IDS_START()

	// DB_CURRENTSTRING,

DEBUGGER_IDS_END()

DEBUGGER_ITEMS_START()

	//  DB_CURRENTSTRING,
	//	DB_CURRENTSTRING|DB_EDITABLE,
	//	DB_CURRENTVALUE|DB_EDITABLE,
	//	DB_CURRENTCHECK,
	//	DB_CURRENTCOMBO,

DEBUGGER_ITEMS_END()

// --------------------
// GetProperties
// --------------------
// Inserts properties into the properties of the object.
//

BOOL WINAPI DLLExport GetProperties(LPMV mV, LPEDATA edPtr, BOOL bMasterItem)
{
#ifndef RUN_ONLY
	mvInsertProps(mV, edPtr, Properties, PROPID_TAB_GENERAL, TRUE);
	
#endif // !RUN_ONLY

	// OK
	return TRUE;
}

// --------------------
// ReleaseProperties
// --------------------
// Called when the properties are removed from the property window.
//

void WINAPI DLLExport ReleaseProperties(LPMV mV, LPEDATA edPtr, BOOL bMasterItem)
{
#ifndef RUN_ONLY
	// Write your code here
#endif // !RUN_ONLY
}

// --------------------
// GetPropCreateParam
// --------------------
// Called when a property is initialized and its creation parameter is NULL (in the PropData).
// Allows you, for example, to change the content of a combobox property according to specific settings in the EDITDATA structure.
//

LPARAM WINAPI DLLExport GetPropCreateParam(LPMV mV, LPEDATA edPtr, UINT nPropID)
{
#ifndef RUN_ONLY
	// Example
	// -------
//	if ( nPropID == PROPID_COMBO )
//	{
//		switch (edPtr->sType)
//		{
//		case TYPE1:
//			return (LPARAM)ComboList1;
//		case TYPE2:
//			return (LPARAM)ComboList2;
//		}
//	}
#endif // !RUN_ONLY
	return NULL;
}

// ----------------------
// ReleasePropCreateParam
// ----------------------
// Called after a property has been initialized.
// Allows you, for example, to free memory allocated in GetPropCreateParam.
//

void WINAPI DLLExport ReleasePropCreateParam(LPMV mV, LPEDATA edPtr, UINT nPropID, LPARAM lParam)
{
#ifndef RUN_ONLY
#endif // !RUN_ONLY
}

// --------------------
// GetPropValue
// --------------------
// Returns the value of properties that have a value.
// Note: see GetPropCheck for checkbox properties
//

LPVOID WINAPI DLLExport GetPropValue(LPMV mV, LPEDATA edPtr, UINT nPropID)
{
#ifndef RUN_ONLY
	switch (nPropID) {

	case PROPID_TILESIZE:
		return new CPropSizeValue(edPtr->tileWidth, edPtr->tileHeight);
	case PROPID_TILESETS:
		CPropDataValue* pv = new CPropDataValue((edPtr->tilesetCount + 1) * sizeof(WORD), NULL);
            if ( pv != NULL )
            {
                if ( pv->m_pData != NULL )
                {
                    LPWORD pw = (LPWORD)pv->m_pData;
                    *pw++ = edPtr->tilesetCount;
                    for (WORD w=0; w<edPtr->tilesetCount; w++)
                        *pw++ = edPtr->tilesets[w];
                    return pv;
                }
                pv->Delete();
            }
		break;
	}
#endif // !RUN_ONLY
	return NULL;
}

// --------------------
// GetPropCheck
// --------------------
// Returns the checked state of properties that have a check box.
//

BOOL WINAPI DLLExport GetPropCheck(LPMV mV, LPEDATA edPtr, UINT nPropID)
{
#ifndef RUN_ONLY
	// Example
	// -------

	switch(nPropID)
	{
	case PROPID_BLOCKLAYER:
		return edPtr->blockLayers;
	//case PROPID_BLOCKMAP:
	//	return edPtr->blockMap;
	case PROPID_BLOCKTILE:
		return edPtr->blockTilesets;
	}

#endif // !RUN_ONLY
	return 0;		// Unchecked
}

// --------------------
// SetPropValue
// --------------------
// This routine is called by MMF after a property has been modified.
//

void WINAPI DLLExport SetPropValue(LPMV mV, LPEDATA edPtr, UINT nPropID, LPVOID lParam)
{
#ifndef RUN_ONLY
	// Gets the pointer to the CPropValue structure
	CPropValue* pValue = (CPropValue*)lParam;

	// Example
	// -------
	switch(nPropID)
	{

		case PROPID_TILESIZE:
			edPtr->tileWidth = max(1, ((CPropSizeValue*)pValue)->m_cx);
			edPtr->tileHeight = max(1, ((CPropSizeValue*)pValue)->m_cy);
			mvInvalidateObject(mV,edPtr);
			break;

		case PROPID_TILESETS:
		if (((CPropDataValue*)pValue)->m_pData)
		{
			LPWORD pw = (LPWORD)((CPropDataValue*)pValue)->m_pData;
			//edPtr->imageCount = *pw++;
            for (WORD w=0;w<edPtr->tilesetCount;w++)
                edPtr->tilesets[w] = *pw++;
		}
		mvInvalidateObject(mV,edPtr);
		break;
	}

	// You may want to have your object redrawn in the frame editor after the modifications,
	// in this case, just call this function
	// mvInvalidateObject(mV, edPtr);

#endif // !RUN_ONLY
}

// --------------------
// SetPropCheck
// --------------------
// This routine is called by MMF when the user modifies a checkbox in the properties.
//

void WINAPI DLLExport SetPropCheck(LPMV mV, LPEDATA edPtr, UINT nPropID, BOOL nCheck)
{
#ifndef RUN_ONLY
	// Example
	// -------

	switch(nPropID)
	{
	case PROPID_BLOCKLAYER:
		edPtr->blockLayers = nCheck != 0;
		break;
	//case PROPID_BLOCKMAP: // deprecated
	//	edPtr->blockMap = nCheck != 0;
	//	break;
	case PROPID_BLOCKTILE:
		edPtr->blockTilesets = nCheck != 0;
		break;
	}

#endif // !RUN_ONLY
}

// --------------------
// EditProp
// --------------------
// This routine is called when the user clicks the button of a Button or EditButton property.
//

LPSTR titles[] = {"Tileset 0", "Tileset 1", "Tileset 2", "Tileset 3", "Tileset 4", "Tileset 5", "Tileset 6", "Tileset 7",
"Tileset 8", "Tileset 9", "Tileset 10", "Tileset 11", "Tileset 12", "Tileset 13", "Tileset 14", "Tileset 15"};

BOOL WINAPI DLLExport EditProp(LPMV mV, LPEDATA edPtr, UINT nPropID)
{
#ifndef RUN_ONLY

	// Example
	// -------
	if (nPropID==PROPID_DELETESETS)
	{
		int kill = MessageBox(0,"Do you really want to delete all tilesets?","Warning",MB_YESNO|MB_ICONWARNING);
		if (kill==IDYES)
		{
			mvInvalidateObject(mV,edPtr);
			for (int i=0;i<TILESETCOUNT;i++)
				edPtr->tilesets[i] = 0;
			edPtr->tilesetCount = 0;
			mvRefreshProp(mV, edPtr, PROPID_TILESETS, true);
			return true;
		}
		return false;
	}
	else if (nPropID==PROPID_TILESETS)
	{	
		mvInvalidateObject(mV,edPtr);
		EditAnimationParams eip;
		eip.m_dwSize = sizeof(EditAnimationParams);
		eip.m_pWindowTitle = 0;
		eip.m_nImages = edPtr->tilesetCount;
		eip.m_nMaxImages = TILESETCOUNT;
		eip.m_nStartIndex = 0;
		eip.m_pImages = &edPtr->tilesets[0];
		eip.m_pImageTitles = titles;
		eip.m_dwOptions = 0;
		eip.m_dwFixedWidth = 256;
		eip.m_dwFixedHeight = 256;
		BOOL output = mV->mvEditAnimation(edPtr,&eip,mV->mvHEditWin);
		//Update object count
		edPtr->tilesetCount = eip.m_nImages;
		mvRefreshProp(mV, edPtr, PROPID_TILESETS, true);
		return output;
	}


#endif // !RUN_ONLY
	return FALSE;
}

// --------------------
// IsPropEnabled
// --------------------
// This routine returns the enabled state of a property.
//

BOOL WINAPI IsPropEnabled(LPMV mV, LPEDATA edPtr, UINT nPropID)
{
#ifndef RUN_ONLY
	// Example
	// -------
/*
	switch (nPropID) {

	case PROPID_CHECK:
		return (edPtr->nComboIndex != 0);
	}
*/
#endif // !RUN_ONLY
	return TRUE;
}


// ============================================================================
//
// TEXT PROPERTIES
// 
// ============================================================================

// --------------------
// GetTextCaps
// --------------------
// Return the text capabilities of the object under the frame editor.
//

DWORD WINAPI DLLExport GetTextCaps(mv _far *mV, LPEDATA edPtr)
{
	return 0;	// (TEXT_ALIGN_LEFT|TEXT_ALIGN_HCENTER|TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP|TEXT_ALIGN_VCENTER|TEXT_ALIGN_BOTTOM|TEXT_FONT|TEXT_COLOR);
}

// --------------------
// GetTextFont
// --------------------
// Return the font used the object.
// Note: the pStyle and cbSize parameters are obsolete and passed for compatibility reasons only.
//

BOOL WINAPI DLLExport GetTextFont(mv _far *mV, LPEDATA edPtr, LPLOGFONT plf, LPSTR pStyle, UINT cbSize)
{
#if !RUN_ONLY
	// Example: copy LOGFONT structure from EDITDATA
	// memcpy(plf, &edPtr->m_lf, sizeof(LOGFONT));
#endif // !RUN_ONLY

	return TRUE;
}

// --------------------
// SetTextFont
// --------------------
// Change the font used the object.
// Note: the pStyle parameter is obsolete and passed for compatibility reasons only.
//

BOOL WINAPI DLLExport SetTextFont(mv _far *mV, LPEDATA edPtr, LPLOGFONT plf, LPCSTR pStyle)
{
#if !RUN_ONLY
	// Example: copy LOGFONT structure to EDITDATA
	// memcpy(&edPtr->m_lf, plf, sizeof(LOGFONT));
#endif // !RUN_ONLY

	return TRUE;
}

// --------------------
// GetTextClr
// --------------------
// Get the text color of the object.
//

COLORREF WINAPI DLLExport GetTextClr(mv _far *mV, LPEDATA edPtr)
{
	// Example
	return 0;	// edPtr->fontColor;
}

// --------------------
// SetTextClr
// --------------------
// Set the text color of the object.
//

void WINAPI DLLExport SetTextClr(mv _far *mV, LPEDATA edPtr, COLORREF color)
{
	// Example
	//edPtr->fontColor = color;
}

// --------------------
// GetTextAlignment
// --------------------
// Get the text alignment of the object.
//

DWORD WINAPI DLLExport GetTextAlignment(mv _far *mV, LPEDATA edPtr)
{
	DWORD dw = 0;
#if !RUN_ONLY
	// Example
	// -------
/*	if ( (edPtr->eData.dwFlags & ALIGN_LEFT) != 0 )
		dw |= TEXT_ALIGN_LEFT;
	if ( (edPtr->eData.dwFlags & ALIGN_HCENTER) != 0 )
		dw |= TEXT_ALIGN_HCENTER;
	if ( (edPtr->eData.dwFlags & ALIGN_RIGHT) != 0 )
		dw |= TEXT_ALIGN_RIGHT;
	if ( (edPtr->eData.dwFlags & ALIGN_TOP) != 0 )
		dw |= TEXT_ALIGN_TOP;
	if ( (edPtr->eData.dwFlags & ALIGN_VCENTER) != 0 )
		dw |= TEXT_ALIGN_VCENTER;
	if ( (edPtr->eData.dwFlags & ALIGN_BOTTOM) != 0 )
		dw |= TEXT_ALIGN_BOTTOM;
*/
#endif // !RUN_ONLY
	return dw;
}

// --------------------
// SetTextAlignment
// --------------------
// Set the text alignment of the object.
//

void WINAPI DLLExport SetTextAlignment(mv _far *mV, LPEDATA edPtr, DWORD dwAlignFlags)
{
#if !RUN_ONLY
	// Example
	// -------
/*	DWORD dw = edPtr->eData.dwFlags;

	if ( (dwAlignFlags & TEXT_ALIGN_LEFT) != 0 )
		dw = (dw & ~(ALIGN_LEFT|ALIGN_HCENTER|ALIGN_RIGHT)) | ALIGN_LEFT;
	if ( (dwAlignFlags & TEXT_ALIGN_HCENTER) != 0 )
		dw = (dw & ~(ALIGN_LEFT|ALIGN_HCENTER|ALIGN_RIGHT)) | ALIGN_HCENTER;
	if ( (dwAlignFlags & TEXT_ALIGN_RIGHT) != 0 )
		dw = (dw & ~(ALIGN_LEFT|ALIGN_HCENTER|ALIGN_RIGHT)) | ALIGN_RIGHT;

	if ( (dwAlignFlags & TEXT_ALIGN_TOP) != 0 )
		dw = (dw & ~(ALIGN_TOP|ALIGN_VCENTER|ALIGN_BOTTOM)) | ALIGN_TOP;
	if ( (dwAlignFlags & TEXT_ALIGN_VCENTER) != 0 )
		dw = (dw & ~(ALIGN_TOP|ALIGN_VCENTER|ALIGN_BOTTOM)) | ALIGN_VCENTER;
	if ( (dwAlignFlags & TEXT_ALIGN_BOTTOM) != 0 )
		dw = (dw & ~(ALIGN_TOP|ALIGN_VCENTER|ALIGN_BOTTOM)) | ALIGN_BOTTOM;

	edPtr->eData.dwFlags = dw;
*/
#endif // !RUN_ONLY
}


// -----------------
// BmpToImg
// -----------------
// Converts an image from the resource to an image displayable under MMF2
// Not used in this template, but it is a good example on how to create
// an image.
//

/*
WORD BmpToImg(int bmID, npAppli idApp, short HotX = 0, short HotY = 0, short ActionX = 0, short ActionY = 0)
{
	Img					ifo;
	WORD				img;
	HRSRC				hs;
	HGLOBAL				hgBuf;
	LPBYTE				adBuf;
	LPBITMAPINFOHEADER	adBmi;

	img = 0;
	if ((hs = FindResource(hInstLib, MAKEINTRESOURCE(bmID), RT_BITMAP)) != NULL)
	{
		if ((hgBuf = LoadResource(hInstLib, hs)) != NULL)
		{
			if ((adBuf = (LPBYTE)LockResource(hgBuf)) != NULL)
			{
				adBmi = (LPBITMAPINFOHEADER)adBuf;
				ifo.imgXSpot = HotX;
				ifo.imgYSpot = HotY;
				ifo.imgXAction = ActionX;
				ifo.imgYAction = ActionY;
				if (adBmi->biBitCount > 4)
					RemapDib((LPBITMAPINFO)adBmi, idApp, NULL);
				img = (WORD)DibToImage(idApp, &ifo, adBmi);
				UnlockResource(hgBuf);
			}
			FreeResource(hgBuf);
		}
	}
	return img;
}
*/

// ============================================================================
//
// ROUTINES USED UNDER FRAME EDITOR
// 
// ============================================================================

// --------------------
// MakeIcon
// --------------------
// Called once object is created or modified, just after setup.
//
// Note: this function is optional. If it's not defined in your extension,
// MMF2 will load the EXO_ICON bitmap if it's defined in your resource file.
//
// If you need to draw the icon manually, remove the comments around this function and in the .def file.
//

/*
int WINAPI DLLExport MakeIconEx ( mv _far *mV, cSurface* pIconSf, LPSTR lpName, fpObjInfo oiPtr, LPEDATA edPtr )
{
	int error = -1;
#ifndef RUN_ONLY
	if ( pIconSf->LoadImage(hInstLib, EXO_ICON) != 0 )
		error = 0;
#endif // !RUN_ONLY
	return error;
}
*/


// --------------------
// CreateObject
// --------------------
// Called when you choose "Create new object". It should display the setup box 
// and initialize everything in the datazone.

int WINAPI DLLExport CreateObject(mv _far *mV, fpLevObj loPtr, LPEDATA edPtr)
{
#ifndef RUN_ONLY

	// Do some rSDK stuff
	#include "rCreateObject.h"
	
	// Set default object settings

	edPtr->tileWidth = 16;
	edPtr->tileHeight = 16;

	for (int i = 0; i < TILESETCOUNT; ++i)
		edPtr->tilesets[i] = 0;
	edPtr->tilesetCount = 0;

	edPtr->blockMap = false;
	edPtr->blockLayers = true;
	edPtr->blockTilesets = false;

	return 0;	// No error

#endif // !RUN_ONLY

	// Error
	return -1;
}

// --------------------
// EditObject
// --------------------
// One of the option from the menu has been selected, and not a default menu option
// automatically handled by MMF2: this routine is then called.
//

BOOL WINAPI EditObject (mv _far *mV, fpObjInfo oiPtr, fpLevObj loPtr, LPEDATA edPtr)
{
#ifndef RUN_ONLY
	
	return TRUE;

#endif // !RUN_ONLY

	// Error
	return FALSE;
}

// --------------------
// SetEditSize
// --------------------
// Called by MMF2 when the object has been resized
//
// Note: remove the comments if your object can be resized (and remove the comments in the .def file)

//
//BOOL WINAPI SetEditSize(LPMV mv, LPEDATA edPtr, int cx, int cy)
//{
//#ifndef RUN_ONLY
//#endif // !RUN_ONLY
//	return TRUE;	// OK
//}


// --------------------
// PutObject
// --------------------
// Called when each individual object is dropped in the frame.
//

void WINAPI	DLLExport PutObject(mv _far *mV, fpLevObj loPtr, LPEDATA edPtr, ushort cpt)
{
#ifndef RUN_ONLY
#endif // !RUN_ONLY
}

// --------------------
// RemoveObject
// --------------------
// Called when each individual object is removed from the frame.
//

void WINAPI	DLLExport RemoveObject(mv _far *mV, fpLevObj loPtr, LPEDATA edPtr, ushort cpt)
{
#ifndef RUN_ONLY
	// Is the last object removed?
    if (0 == cpt)
	{
		// Do whatever necessary to remove our data
	}
#endif // !RUN_ONLY
}

// --------------------
// DuplicateObject
// --------------------
// Called when an object is created from another one (note: should be called CloneObject instead...)
//

void WINAPI DLLExport DuplicateObject(mv __far *mV, fpObjInfo oiPtr, LPEDATA edPtr)
{
#ifndef RUN_ONLY
#endif // !RUN_ONLY
}

// --------------------
// GetObjectRect
// --------------------
// Returns the size of the rectangle of the object in the frame editor.
//

void WINAPI DLLExport GetObjectRect(mv _far *mV, RECT FAR *rc, fpLevObj loPtr, LPEDATA edPtr)
{
#ifndef RUN_ONLY
	rc->right = rc->left + 32;
	rc->bottom = rc->top + 32;
#endif // !RUN_ONLY
	return;
}


// --------------------
// EditorDisplay
// --------------------
// Displays the object under the frame editor
//
// Note: this function is optional. If it's not defined in your extension,
// MMF2 will load and display the EXO_IMAGE bitmap if it's defined in your resource file.
//
// If you need to draw the icon manually, remove the comments around this function and in the .def file.
//

//
//void WINAPI DLLExport EditorDisplay(mv _far *mV, fpObjInfo oiPtr, fpLevObj loPtr, LPEDATA edPtr, RECT FAR *rc)
//{
//#ifndef RUN_ONLY
//
//	// This is a simple case of drawing an image onto MMF's frame editor window
//	// First, we must get a pointer to the surface used by the frame editor
//
//	LPSURFACE ps = WinGetSurface((int)mV->mvIdEditWin);
//	if ( ps != NULL )		// Do the following if this surface exists
//	{
//		ps->SetClipRect(rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top);
//		srand((long)mV->mvEditFrame + edPtr->width + edPtr->height); /* Prevent the user from having a seizure */
//
//		/* Get surfaces of tilesets */
//		cSurface surfs[TILESETCOUNT];
//		for (int i = 0; i < edPtr->tilesetCount; ++i)
//		{
//			LockImageSurface(mV->mvIdAppli, edPtr->tilesets[i], surfs[i]);
//		}
//	
//		/* Draw opaque background */
//		if (!edPtr->transparent)
//			ps->Rectangle(rc->left, rc->top, rc->right, rc->bottom, edPtr->background, 0, 0, 1);
//
//		/* For each tile */
//		if (edPtr->tileWidth > 0 && edPtr->tileHeight > 0)
//		{
//			for (int x = rc->left; x <= rc->right; x += edPtr->tileWidth)
//			{
//				for (int y = rc->top; y <= rc->bottom; y += edPtr->tileHeight)
//				{
//					/* Draw a random tile */
//					if (edPtr->tilesetCount)
//					{
//						int tileset = 0;
//						//int tileset = rand() % edPtr->tilesetCount;
//						int tx = rand() % (surfs[tileset].GetWidth() / edPtr->tileWidth);
//						int ty = rand() % (surfs[tileset].GetHeight() / edPtr->tileHeight);
//						surfs[tileset].Blit(*ps, x, y, tx*edPtr->tileWidth, ty*edPtr->tileHeight, edPtr->tileWidth, edPtr->tileHeight, BMODE_TRANSP, BOP_COPY);
//					}
//					/* Draw grid */
//					else
//					{
//						ps->Rectangle(x, y, x+edPtr->tileWidth+1, y+edPtr->tileHeight+1, 1, 0x999999);
//					}
//				}
//			}
//		}
//		
//		/* Draw border */
//		ps->Rectangle(rc->left, rc->top, rc->right, rc->bottom, 1, OPAQUE_BLACK);
//		ps->ClearClipRect();
//
//		/* Unlock stuff */
//		for (int i = 0; i < edPtr->tilesetCount; ++i)
//		{
//			UnlockImageSurface(surfs[i]);
//		}
//	}
//
//#endif // !RUN_ONLY
//}


// --------------------
// IsTransparent
// --------------------
// This routine tells MMF2 if the mouse pointer is over a transparent zone of the object.
// 

extern "C" BOOL WINAPI DLLExport IsTransparent(mv _far *mV, fpLevObj loPtr, LPEDATA edPtr, int dx, int dy)
{
#ifndef RUN_ONLY
	// Write your code here
#endif // !RUN_ONLY
	return FALSE;
}

// --------------------
// PrepareToWriteObject
// --------------------
// Just before writing the datazone when saving the application, MMF2 calls this routine.
// 

void WINAPI	DLLExport PrepareToWriteObject(mv _far *mV, LPEDATA edPtr, fpObjInfo adoi)
{
#ifndef RUN_ONLY
	// Write your code here
#endif // !RUN_ONLY
}

// --------------------
// GetFilters
// --------------------
//

BOOL WINAPI GetFilters(LPMV mV, LPEDATA edPtr, DWORD dwFlags, LPVOID pReserved)
{
#ifndef RUN_ONLY

	if ( (dwFlags & GETFILTERS_IMAGES) != 0 )
		return TRUE;

	// If your extension uses sound filters
//	if ( (dwFlags & GETFILTERS_SOUNDS) != 0 )
//		return TRUE;
#endif // RUN_ONLY
	return FALSE;
}

// --------------------
// UsesFile
// --------------------
// Triggers when a file is dropped onto the frame
// Return TRUE if you can create an object from the given file
//

BOOL WINAPI	DLLExport UsesFile (LPMV mV, LPSTR fileName)
{
	BOOL r = FALSE;
#ifndef RUN_ONLY

	// Example: return TRUE if file extension is ".txt"
/*	
	LPSTR	ext, npath;

	if ( fileName != NULL )
	{
		if ( (ext=(LPSTR)malloc(_MAX_EXT)) != NULL )
		{
			if ( (npath=(LPSTR)malloc(_MAX_PATH)) != NULL )
			{
				strcpy(npath, fileName);
				_splitpath(npath, NULL, NULL, NULL, ext);
				if ( _stricmp(ext, ".txt") == 0 )
					r = TRUE;
				free(npath);
			}
			free(ext);
		}
	} */
#endif // !RUN_ONLY
	return r;
}


// --------------------
// CreateFromFile
// --------------------
// Creates a new object from file
//

void WINAPI	DLLExport CreateFromFile (LPMV mV, LPSTR fileName, LPEDATA edPtr)
{
#ifndef RUN_ONLY
	// Initialize your extension data from the given file
//	edPtr->swidth = 32;
//	edPtr->sheight = 32;

	// Example: store the filename
	// strcpy(edPtr->myFileName, fileName);
#endif // !RUN_ONLY
}



// ============================================================================
//
// ROUTINES USED UNDER EVENT / TIME / STEP-THROUGH EDITOR
// You should not need to change these routines
// 
// ============================================================================

// -----------------
// menucpy
// -----------------
// Internal routine used later, copy one menu onto another
// 
#ifndef RUN_ONLY
void menucpy(HMENU hTargetMenu, HMENU hSourceMenu)
{
	int			n, id, nMn;
	NPSTR		strBuf;
	HMENU		hSubMenu;

	nMn = GetMenuItemCount(hSourceMenu);
	strBuf = (NPSTR)LocalAlloc(LPTR, 80);
	for (n=0; n<nMn; n++)
	{
		if (0 == (id = GetMenuItemID(hSourceMenu, n)))
			AppendMenu(hTargetMenu, MF_SEPARATOR, 0, 0L);
		else
		{
			GetMenuString(hSourceMenu, n, strBuf, 80, MF_BYPOSITION);
			if (strBuf[0] == '~')
			{
				AppendMenu(hTargetMenu, MF_DISABLED /*| MF_GRAYED*/, id, strBuf+1);
			}
			else
			{
				if (id != -1)
					AppendMenu(hTargetMenu, GetMenuState(hSourceMenu, n, MF_BYPOSITION), id, strBuf);
				else
				{
					hSubMenu = CreatePopupMenu();
					AppendMenu(hTargetMenu, MF_POPUP | MF_STRING, (uint)hSubMenu, strBuf);
					menucpy(hSubMenu, GetSubMenu(hSourceMenu, n));
				}
			}
		}
	}
	LocalFree((HLOCAL)strBuf);
}

// -----------------
// GetPopupMenu
// -----------------
// Internal routine used later. Returns the first popup from a menu
// 
HMENU GetPopupMenu(LPEDATA edPtr, short mn)
{
	HMENU hPopup=CreatePopupMenu();
	
	if (mn == MN_CONDITIONS)
		menucpy(hPopup, ConditionMenu(edPtr));
	else if (mn == MN_ACTIONS)
		menucpy(hPopup, ActionMenu(edPtr));
	else if (mn == MN_EXPRESSIONS)
		menucpy(hPopup, ExpressionMenu(edPtr));

	return hPopup;
}

// --------------------
// GetEventInformations
// --------------------
// Internal routine used later. Look for one event in one of the eventInfos array...
// No protection to go faster: you must properly enter the conditions/actions!
//

static LPEVENTINFOS2 GetEventInformations(LPEVENTINFOS2 eiPtr, short code)

{
	while (eiPtr->infos.code != code)
		eiPtr = EVINFO2_NEXT(eiPtr);
	
	return eiPtr;
}
#endif // !RUN_ONLY


// ----------------------------------------------------
// GetConditionMenu / GetActionMenu / GetExpressionMenu
// ----------------------------------------------------
// Load the condition/action/expression menu from the resource, eventually
// enable or disable some options, and returns it to MMF2.
//

HMENU WINAPI DLLExport GetConditionMenu(mv _far *mV, fpObjInfo oiPtr, LPEDATA edPtr)
{
#ifndef RUN_ONLY
	// Check compatibility
	return GetPopupMenu(edPtr, MN_CONDITIONS);
#endif // !RUN_ONLY
	return NULL;
}

HMENU WINAPI DLLExport GetActionMenu(mv _far *mV, fpObjInfo oiPtr, LPEDATA edPtr)
{
#ifndef RUN_ONLY
	// Check compatibility
	return GetPopupMenu(edPtr, MN_ACTIONS);
#endif // !RUN_ONLY
	return NULL;
}

HMENU WINAPI DLLExport GetExpressionMenu(mv _far *mV, fpObjInfo oiPtr, LPEDATA edPtr)
{
#ifndef RUN_ONLY
	// Check compatibility
	return GetPopupMenu(edPtr, MN_EXPRESSIONS);
#endif // !RUN_ONLY
	return NULL;
}


// -------------------------------------------------------
// GetConditionTitle / GetActionTitle / GetExpressionTitle
// -------------------------------------------------------
// Returns the title of the dialog box displayed when entering
// parameters for the condition, action or expressions, if any.
// Here, we simply return the title of the menu option
//

#ifndef RUN_ONLY
void GetCodeTitle(LPEVENTINFOS2 eiPtr, short code, short param, short mn, LPSTR strBuf, WORD maxLen)
{
	HMENU		hMn;

	// Finds event in array
	eiPtr=GetEventInformations(eiPtr, code);

	// If a special string is to be returned
	short strID = EVINFO2_PARAMTITLE(eiPtr, param);

	if (strID) {
		switch(mn) {
		case MN_CONDITIONS:
			if (code>=0&&code<(short)Conditions.size()) {
				if (param>=0&&param<(short)Conditions[code]->getParamCount())
					strcpy(strBuf, Conditions[code]->getParamName(param));
			}
			break;
		case MN_ACTIONS:
			if (code>=0&&code<(short)Actions.size()) {
				if (param>=0&&param<(short)Actions[code]->getParamCount())
					strcpy(strBuf, Actions[code]->getParamName(param));
			}
			break;
		case MN_EXPRESSIONS:
			if (code>=0&&code<(short)Expressions.size()) {
				if (param>=0&&param<(short)Expressions[code]->getParamCount())
					strcpy(strBuf, Expressions[code]->getParamName(param));
			}
			break;
		}
	} else {
		if ((hMn=LoadMenu(hInstLib, MAKEINTRESOURCE(mn)))) {
			GetMenuString(hMn, eiPtr->menu, strBuf, maxLen, MF_BYCOMMAND);
			DestroyMenu(hMn);
		}
	}
}
#else
#define GetCodeTitle(a, b, c, d, e, f)
#endif // !RUN_ONLY

void WINAPI DLLExport GetConditionTitle(mv _far *mV, short code, short param, LPSTR strBuf, short maxLen) {
	GetCodeTitle((LPEVENTINFOS2)conditionsInfos, code, param, MN_CONDITIONS, strBuf, maxLen);
}
void WINAPI DLLExport GetActionTitle(mv _far *mV, short code, short param, LPSTR strBuf, short maxLen) {
	GetCodeTitle((LPEVENTINFOS2)actionsInfos, code, param, MN_ACTIONS, strBuf, maxLen);
}
void WINAPI DLLExport GetExpressionTitle(mv _far *mV, short code, LPSTR strBuf, short maxLen) {
	GetCodeTitle((LPEVENTINFOS2)expressionsInfos, code, 0, MN_EXPRESSIONS, strBuf, maxLen);
}

// -------------------------------------------------------
// GetConditionTitle / GetActionTitle / GetExpressionTitle
// -------------------------------------------------------
// From a menu ID, these routines returns the code of the condition,
// action or expression, as defined in the .H file
//

short WINAPI DLLExport GetConditionCodeFromMenu(mv _far *mV, short menuId)
{
#ifndef RUN_ONLY
	LPEVENTINFOS2 eiPtr;
	int n;
	for (n=Conditions.size(), eiPtr=(LPEVENTINFOS2)conditionsInfos;n>0&&eiPtr->menu!=menuId;n--)
		eiPtr=EVINFO2_NEXT(eiPtr);
	if (n>0)
		return eiPtr->infos.code;
#endif // !RUN_ONLY
	return -1;
}

short WINAPI DLLExport GetActionCodeFromMenu(mv _far *mV, short menuId)
{
#ifndef RUN_ONLY
	LPEVENTINFOS2 eiPtr;
	int n;

	for (n=Actions.size(), eiPtr=(LPEVENTINFOS2)actionsInfos;n>0&&eiPtr->menu!=menuId;n--)
		eiPtr = EVINFO2_NEXT(eiPtr);
	if (n>0) 
		return eiPtr->infos.code;
#endif // !RUN_ONLY
	return -1;
}

short WINAPI DLLExport GetExpressionCodeFromMenu(mv _far *mV, short menuId)
{
#ifndef RUN_ONLY
	LPEVENTINFOS2 eiPtr;
	int	n;
	for (n=Expressions.size(), eiPtr=(LPEVENTINFOS2)expressionsInfos;n>0&&eiPtr->menu!=menuId;n--)
		eiPtr = EVINFO2_NEXT(eiPtr);
	if (n>0) 
		return eiPtr->infos.code;
#endif // !RUN_ONLY
	return -1;
}


// -------------------------------------------------------
// GetConditionInfos / GetActionInfos / GetExpressionInfos
// -------------------------------------------------------
// From a action / condition / expression code, returns 
// an infosEvents structure. 
//

LPINFOEVENTSV2 WINAPI DLLExport GetConditionInfos(mv _far *mV, short code)
{
#ifndef RUN_ONLY
	return &GetEventInformations((LPEVENTINFOS2)conditionsInfos, code)->infos;
#else
	return NULL;
#endif // !RUN_ONLY
}

LPINFOEVENTSV2 WINAPI DLLExport GetActionInfos(mv _far *mV, short code)
{
#ifndef RUN_ONLY
	return &GetEventInformations((LPEVENTINFOS2)actionsInfos, code)->infos;
#else
	return NULL;
#endif // !RUN_ONLY
}

LPINFOEVENTSV2 WINAPI DLLExport GetExpressionInfos(mv _far *mV, short code)
{
#ifndef RUN_ONLY
	return &GetEventInformations((LPEVENTINFOS2)expressionsInfos, code)->infos;
#else
	return NULL;
#endif // !RUN_ONLY
}


// ----------------------------------------------------------
// GetConditionString / GetActionString / GetExpressionString
// ----------------------------------------------------------
// From a action / condition / expression code, returns 
// the string to use for displaying it under the event editor
//

void WINAPI DLLExport GetConditionString(mv _far *mV, short code, LPSTR strPtr, short maxLen)
{
#ifndef RUN_ONLY
		if (code>=0&&code<(short)Conditions.size())
			strcpy(strPtr, Conditions[code]->getName());
#endif // !RUN_ONLY
}

void WINAPI DLLExport GetActionString(mv _far *mV, short code, LPSTR strPtr, short maxLen)
{
#ifndef RUN_ONLY
		if (code>=0&&code<(short)Actions.size())
			strcpy(strPtr, Actions[code]->getName());
#endif // !RUN_ONLY
}

void WINAPI DLLExport GetExpressionString(mv _far *mV, short code, LPSTR strPtr, short maxLen)
{
#ifndef RUN_ONLY
		if (code>=0&&code<(short)Expressions.size())
			strcpy(strPtr, Expressions[code]->getName());
#endif // !RUN_ONLY
}

// ----------------------------------------------------------
// GetExpressionParam
// ----------------------------------------------------------
// Returns the parameter name to display in the expression editor
//

void WINAPI DLLExport GetExpressionParam(mv _far *mV, short code, short param, LPSTR strBuf, short maxLen)
{
#ifndef RUN_ONLY
	if (strlen(Expressions[code]->getParamName(param)))
		strcpy(strBuf, Expressions[code]->getParamName(param));
	else
		*strBuf=0;
#endif
}

// ----------------------------------------------------------
// Custom Parameters
// ----------------------------------------------------------

// --------------------
// InitParameter
// --------------------
// Initialize the parameter.
//

void WINAPI InitParameter(mv _far *mV, short code, paramExt* pExt)
{
#if !RUN_ONLY
	// Example
	// -------
	// strcpy(&pExt->pextData[0], "Parameter Test");
	// pExt->pextSize = sizeof(paramExt) + strlen(pExt->pextData)+1;
#endif // !RUN_ONLY
}

// Example of custom parameter setup proc
// --------------------------------------
/*
#if !RUN_ONLY
BOOL CALLBACK DLLExport SetupProc(HWND hDlg, UINT msgType, WPARAM wParam, LPARAM lParam)
{
	paramExt*			pExt;

	switch (msgType)
	{
		case WM_INITDIALOG: // Init dialog

			// Save edptr
			SetWindowLong(hDlg, DWL_USER, lParam);
			pExt=(paramExt*)lParam;

			SetDlgItemText(hDlg, IDC_EDIT, pExt->pextData);
			return TRUE;

		case WM_COMMAND: // Command

			// Retrieve edptr
			pExt = (paramExt *)GetWindowLong(hDlg, DWL_USER);

			switch (wmCommandID)
			{
			case IDOK:	// Exit
				GetDlgItemText(hDlg, IDC_EDIT, pExt->pextData, 500);
				pExt->pextSize=sizeof(paramExt)+strlen(pExt->pextData)+1;
				EndDialog(hDlg, TRUE);
				return TRUE;

				default:
					break;
			}
			break;

		default:
			break;
	}
	return FALSE;
}
#endif // !RUN_ONLY
*/

// --------------------
// EditParameter
// --------------------
// Edit the parameter.
//

void WINAPI EditParameter(mv _far *mV, short code, paramExt* pExt)
{
#if !RUN_ONLY

	// Example
	// -------
	// DialogBoxParam(hInstLib, MAKEINTRESOURCE(DB_TRYPARAM), mV->mvHEditWin, SetupProc, (LPARAM)(LPBYTE)pExt);

#endif // !RUN_ONLY
}

// --------------------
// GetParameterString
// --------------------
// Initialize the parameter.
//

void WINAPI GetParameterString(mv _far *mV, short code, paramExt* pExt, LPSTR pDest, short size)
{
#if !RUN_ONLY

	// Example
	// -------
	// wsprintf(pDest, "Super parameter %s", pExt->pextData);

#endif // !RUN_ONLY
}

// -----------------
// GetObjInfos
// -----------------
// Return object info
//
// Info displayed in the object's About properties
// Note: ObjComment is also displayed in the Quick Description box in the Insert Object dialog box
//

void WINAPI	DLLExport GetObjInfos (mv _far *mV, LPEDATA edPtr, LPSTR ObjName, LPSTR ObjAuthor, LPSTR ObjCopyright, LPSTR ObjComment, LPSTR ObjHttp)
{
#ifndef RUN_ONLY
	strcpy(ObjName, ObjectName);
	strcpy(ObjAuthor, ObjectAuthor);
	strcpy(ObjCopyright, ObjectCopyright);
	strcpy(ObjComment, ObjectComment);
	strcpy(ObjHttp, ObjectURL);
#endif
}

// -----------------
// GetHelpFileName
// -----------------
// Returns the help filename of the object.
//

LPCSTR WINAPI GetHelpFileName()
{
#ifndef RUN_ONLY
	return ObjectHelp;
#else
	return NULL;
#endif
}

// -----------------
// GetRegID
// -----------------
// Returns the registered ID of the object
//

int WINAPI GetRegID()
{
#ifndef RUN_ONLY
	return ObjectRegID;
#else
	return REGID_RUNTIME;
#endif
}
