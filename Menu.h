// --------------------------------
// Condition menu
// --------------------------------

#ifdef CONDITION_MENU

	ITEM(0, "File loaded")
	ITEM(2, "Loading file failed")
	SEPARATOR
	ITEM(1, "File saved")
	ITEM(3, "Saving file failed")
	
#endif

// --------------------------------
// Action menu
// --------------------------------

#ifdef ACTION_MENU

	SEPARATOR
	ITEM(11, "Attach viewport")
	ITEM(31, "Detach viewport")
	SEPARATOR
	SUB_START("Input / Output")
		ITEM(21, "Load from file")
		ITEM(22, "Save to file")
		SEPARATOR
		ITEM(26, "Set compression level")
		ITEM(40, "Set block options")
	SUB_END
	SUB_START("Tilesets")
		ITEM(12, "Add tileset")
		ITEM(25, "Add tilesets from Surface")
		SEPARATOR
		ITEM(41, "Delete tileset")
		ITEM(43, "Delete all tilesets")
		SEPARATOR
		ITEM(30, "Set current tileset")
		ITEM(34, "Swap two tilesets")
		SUB_START("Set image")
			ITEM(4, "From file")
			ITEM(14, "From Surface")
		SUB_END
		ITEM(27, "Set transparent color")
		//ITEM(3, "Assign file path")
	SUB_END
	SUB_START("Layers")
		ITEM(29, "Add layer")
		SEPARATOR
		ITEM(42, "Delete layer")
		ITEM(44, "Delete all layers")
		SEPARATOR
		ITEM(8,"Set current layer")
		SEPARATOR
		ITEM(1, "Resize")
		SEPARATOR
		ITEM(0, "Set tile size")
		ITEM(37, "Set tileset")
		ITEM(15, "Set collision tileset")
		SEPARATOR
		ITEM(18, "Set visible")
		ITEM(19, "Set opacity")
		SEPARATOR
		ITEM(10, "Set offset")
		ITEM(2, "Set scroll speed")
		ITEM(9, "Set wrapping")
	SUB_END
	SEPARATOR
	SUB_START("Tiles")
		ITEM(45, "Clear layer")
		SEPARATOR
		ITEM(23, "Clear tile")
		ITEM(7, "Set tile")
		SEPARATOR
		ITEM(46, "Fill with tile")
		ITEM(24, "Fill area with tile")
	SUB_END
	SUB_START("Cursor")
		ITEM(35, "Move")
		ITEM(33, "Resize")
		ITEM(39, "Maximize")
		SEPARATOR
		SUB_START("Drawing")
			SUB_START("Tileset source")
				ITEM(36, "Select single")
				ITEM(20, "Select range")
				ITEM(5, "Set fill pattern")
			SUB_END
			SEPARATOR
			SUB_START("Clear tiles")
				ITEM(6, "At cursor")
			SUB_END
			SUB_START("Place tiles")
				ITEM(28, "At cursor")
				ITEM(32, "At position...")
			SUB_END
		SUB_END
		SEPARATOR
		SUB_START("Copy tiles to...")
			ITEM(16, "Surface")
		SUB_END
		SUB_START("Paste tiles from...")
			ITEM(38, "Another location")
			SEPARATOR
			ITEM(17, "Surface")
		SUB_END
	SUB_END
	SEPARATOR
#endif


// --------------------------------
// Expression menu
// --------------------------------

#ifdef EXPRESSION_MENU

	SEPARATOR
	SUB_START("Tilesets")
		ITEM(16, "Count")
		SEPARATOR
		ITEM(0, "Column count")
		ITEM(1, "Row count")
	SUB_END
	SUB_START("Layer")
		ITEM(15, "Count")
		SEPARATOR
		SUB_START("Offset")
			ITEM(6, "X")
			ITEM(7, "Y")
		SUB_END
		SUB_START("Scroll speed")
			ITEM(8, "X")
			ITEM(9, "Y")
		SUB_END
		SUB_START("Wrapping")
			ITEM(4, "X")
			ITEM(5, "Y")
		SUB_END
		SEPARATOR
		SUB_START("Size")
			ITEM(2, "Width")
			ITEM(3, "Height")
		SUB_END
		SUB_START("Tile size")
			ITEM(10, "Width")
			ITEM(11, "Height")
		SUB_END
		SEPARATOR
		SUB_START("Tile at position...")
			ITEM(12, "Tileset X")
			ITEM(13, "Tileset Y")
			SEPARATOR
			ITEM(16, "Tileset ID")
			ITEM(14, "Compound value")
		SUB_END
	SUB_END
	SEPARATOR

#endif