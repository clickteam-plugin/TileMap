#pragma once

inline cSurface * getPrototype(int depth, int renderMode = SD_DIB)
{
    cSurface * proto = 0;

#ifdef HWABETA
    if (renderMode != SD_DIB) {
        GetSurfacePrototype(&proto, depth, ST_HWA_ROMTEXTURE, renderMode);
        return proto;
    }
#endif

    GetSurfacePrototype(&proto, depth, ST_MEMORY, SD_DIB);
    return proto;
}

inline bool copyBlit(cSurface & src, cSurface & dest)
{
#ifdef HWABETA
    return 0 != src.Blit(dest);
#else
    return 0 != src.Blit(dest, 0, 0, BMODE_OPAQUE, BOP_COPY, 0, BLTF_COPYALPHA);
#endif
}