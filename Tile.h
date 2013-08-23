#pragma once

struct TileSettings
{
	bool		visible;
	float		opacity;
	int			offsetX;
	int			offsetY;
	BYTE		tileset;
	BYTE		animation;

	// HWA specific
	COLORREF	tint;
	bool		transform;
	float		scaleX;
	float		scaleY;
	float		angle;

	TileSettings()
		:	visible(true), opacity(1.0f), offsetX(0), offsetY(0),
			tint(WHITE), transform(false), scaleX(1.0f), scaleY(1.0f), angle(0.0f),
			animation(0)
	{
	}
};

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