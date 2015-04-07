#pragma once

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

	bool isWithin(const Tile& tile) const
	{
		return tile.x >= a.x
			&& tile.y >= a.y
			&& tile.x <= b.x
			&& tile.y <= b.y;
	}
};