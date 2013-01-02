struct Tile
{
	/* Index of the tileset */
	/* For optimization 0 is empty (MMF: -1) */
	/* First tileset is actually 1 (MMF: 0) */
	//unsigned char	tileset		;

	/* Local tile position in the tileset */
	unsigned char	x;
	unsigned char	y;
};

struct TileRange
{
	unsigned char x1;
	unsigned char y1;
	unsigned char x2;
	unsigned char y2;

};