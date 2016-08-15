#pragma once

union Tile {
    static const unsigned short EMPTY = 0xffff;

    struct {
        unsigned char x;
        unsigned char y;
    };
    unsigned short id;

    static Tile ByXY(unsigned char x, unsigned char y)
    {
        Tile t;
        t.x = x;
        t.y = y;
        return t;
    }

    static Tile ByID(unsigned short id)
    {
        Tile t;
        t.id = id;
        return t;
    }
};

struct TileRange {
    Tile a;
    Tile b;

    bool isWithin(const Tile & tile) const
    {
        return tile.x >= a.x && tile.y >= a.y && tile.x <= b.x && tile.y <= b.y;
    }
};