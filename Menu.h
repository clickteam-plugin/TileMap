// --------------------------------
// Condition menu
// --------------------------------

#ifdef CONDITION_MENU

SUB_START("Input/Output")
    ITEM(0, "File loaded")
    ITEM(2, "Loading file failed")
    SEPARATOR
    ITEM(1, "File saved")
    ITEM(3, "Saving file failed")
SUB_END
SUB_START("Properties")
    ITEM(4, "On map property")
SUB_END
// SEPARATOR
// SUB_START("Layer")
//	ITEM(5, "Compare to tile X")
//	ITEM(6, "Compare to tile Y")
//	ITEM(7, "Compare to tile ID")
// SUB_END

#endif

// --------------------------------
// Action menu
// --------------------------------

#ifdef ACTION_MENU

SEPARATOR
ITEM(11, "Attach viewport")
ITEM(31, "Detach viewport")
SEPARATOR
ITEM(0, "~<Map>")
SUB_START("Input / Output")
    ITEM(21, "Load from file")
    ITEM(22, "Save to file")
    SEPARATOR
    ITEM(26, "Set compression level")
    ITEM(40, "Set block options")
    SEPARATOR
    ITEM(50, "Set tileset origin")
    ITEM(51, "Set tileset user path")
SUB_END
SUB_START("Tilesets")
    ITEM(12, "Add tileset")
    ITEM(25, "Add tilesets from Surface")
    SEPARATOR
    ITEM(34, "Swap two tilesets")
    SEPARATOR
    ITEM(41, "Delete tileset")
    ITEM(43, "Delete all tilesets")
    SEPARATOR
    ITEM(30, "Set current tileset")
SUB_END
SUB_START("Layers")
    ITEM(29, "Add layer")
    ITEM(73, "Insert layer")
    SEPARATOR
    ITEM(42, "Delete layer")
    ITEM(44, "Delete all layers")
    SEPARATOR
    ITEM(8, "Set current layer")
SUB_END
SUB_START("Properties")
    ITEM(52, "Set integer")
    ITEM(53, "Set float")
    ITEM(54, "Set string")
    SEPARATOR
    ITEM(55, "Delete")
    ITEM(56, "Delete all")
    SEPARATOR
    ITEM(57, "Iterate")
SUB_END
SEPARATOR
ITEM(0, "~<Current tileset>")
SUB_START("Settings")
    ITEM(4, "Set image from file")
    ITEM(14, "Set image from Surface")
    ITEM(3, "Assign image path")
    SEPARATOR
    ITEM(27, "Set transparent color")
SUB_END
SEPARATOR
ITEM(0, "~<Current layer>")
SUB_START("Settings")
    ITEM(1, "Resize")
    ITEM(0, "Set tile size")
    SEPARATOR
    ITEM(37, "Set tileset")
    ITEM(15, "Set collision tileset")
    SEPARATOR
    ITEM(18, "Set visible")
    ITEM(19, "Set opacity")
    SEPARATOR
    ITEM(10, "Set offset")
    ITEM(2, "Set scroll speed")
    SEPARATOR
    ITEM(9, "Set wrapping")
    SEPARATOR
    SUB_START("Sub-layer links")
        ITEM(68, "Set animation index")
        ITEM(72, "Set animation frame offset")
        SEPARATOR
        ITEM(69, "Set tileset")
    SUB_END
SUB_END
SUB_START("Tiles")
    ITEM(45, "Clear layer")
    SEPARATOR
    ITEM(23, "Clear tile")
    ITEM(7, "Set tile")
    SEPARATOR
    ITEM(46, "Fill with tile")
    ITEM(24, "Fill area with tile")
    // SUB_END
    SEPARATOR
    SUB_START("With cursor")
        ITEM(6, "Clear tiles")
        SEPARATOR
        ITEM(28, "Place tiles")
        ITEM(32, "Place tiles at position...")
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
SUB_END
SUB_START("Sub-layers")
    SUB_START("Add sub-layer...")
        ITEM(48, "Small (one byte)")
        ITEM(60, "Medium (two bytes)")
        ITEM(61, "Large (four bytes)")
        ITEM(63, "Custom")
        SEPARATOR
        SUB_START("With default value...")
            ITEM(62, "Small")
            ITEM(58, "Custom")
        SUB_END
    SUB_END
    ITEM(74, "Delete all")
    ITEM(75, "Delete sub-layer")
    SEPARATOR
    ITEM(67, "Clear")
    SEPARATOR
    ITEM(59, "Set integer")
    ITEM(64, "Set float")
    SEPARATOR
    ITEM(65, "Fill with integer")
    ITEM(66, "Fill with float")
    SEPARATOR
    ITEM(71, "Randomize")
    SEPARATOR
    SUB_START("With cursor")
        ITEM(49, "Fill with integer")
        ITEM(70, "Fill with float")
    SUB_END
SUB_END
SEPARATOR
ITEM(0, "~<Tile cursor>")
SUB_START("Tileset source")
    ITEM(36, "Select single")
    ITEM(20, "Select range")
    ITEM(5, "Set fill pattern")
SUB_END
SUB_START("Layer source")
    ITEM(35, "Move")
    ITEM(33, "Resize")
    ITEM(47, "Select range")
    ITEM(39, "Maximize to current layer")
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
    ITEM(0, "Width")
    ITEM(1, "Height")
    SEPARATOR
    ITEM(18, "Absolute path")
SUB_END
SUB_START("Layers")
    ITEM(15, "Count")
    SEPARATOR
    SUB_START("Size")
        ITEM(2, "Width")
        ITEM(3, "Height")
    SUB_END
    SUB_START("Tile size")
        ITEM(10, "Width")
        ITEM(11, "Height")
    SUB_END
    ITEM(32, "Tileset")
    ITEM(33, "Collision tileset")
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
    SUB_START("Tile at position...")
        ITEM(12, "Tileset X")
        ITEM(13, "Tileset Y")
        SEPARATOR
        ITEM(16, "Tileset ID")
        ITEM(14, "Compound value")
    SUB_END
    SEPARATOR
    SUB_START("Sub-layers")
        ITEM(26, "Count")
        SEPARATOR
        ITEM(27, "Cell size")
        SEPARATOR
        // ITEM(23, "Byte at position")
        ITEM(24, "Integer at position")
        ITEM(25, "Float at position")
    SUB_END
SUB_END
SUB_START("Properties")
    ITEM(22, "Iterated property")
    SEPARATOR
    ITEM(19, "Integer value")
    ITEM(20, "Float value")
    ITEM(21, "String value")
SUB_END
SUB_START("Tile cursor")
    ITEM(28, "X")
    ITEM(29, "Y")
    ITEM(30, "Width")
    ITEM(31, "Height")
SUB_END
SEPARATOR

#endif