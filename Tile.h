union Tile
{
	static const unsigned short EMPTY = 0xffff; 

	struct
	{
		unsigned char x;
		unsigned char y;
	};
	unsigned short id;
};

struct TileRange
{
	Tile a;
	Tile b;
};