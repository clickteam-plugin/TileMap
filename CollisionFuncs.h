#pragma once

// Assigns and returns a layer's collision tileset before checking for
// collisions
cSurface* prepareLayerCollSurf(LPRDATA rdPtr, const Layer& layer)
{
    // Get layer's collision tileset
    unsigned char tilesetID = (layer.settings.collision != 0xff)
                                  ? layer.settings.collision
                                  : layer.settings.tileset;
    if (tilesetID >= rdPtr->p->tilesets->size())
        return 0;

    rdPtr->cndTileset = &(*rdPtr->p->tilesets)[tilesetID];

    // Get tileset's settings
    return rdPtr->cndTileset->surface;
}

// Prepares a fine collision by locking the used buffer
BYTE* prepareFineColl(LPRDATA rdPtr, cSurface* surface)
{
    if (!rdPtr->fineColl)
        return 0;

    if (surface->HasAlpha()) {
        rdPtr->cndAlphaSurf = surface->GetAlphaSurface();
        return rdPtr->cndAlphaSurf->LockBuffer();
    }
    else {
        return surface->LockBuffer();
    }
}

// Unlocks the buffer
void unprepareFineColl(LPRDATA rdPtr, cSurface* surface, BYTE* buff)
{
    if (!rdPtr->fineColl)
        return;

    if (surface->HasAlpha()) {
        rdPtr->cndAlphaSurf->UnlockBuffer(buff);
        surface->ReleaseAlphaSurface(rdPtr->cndAlphaSurf);
    }
    else {
        surface->UnlockBuffer(buff);
    }
}

// Caches all sub-layer pointers that will be needed for the overlap filters
void cacheOverlapSublayers(LPRDATA rdPtr, const Layer& layer)
{
    // If there's a sublayer filter we'll cache the layer's sublayer pointers
    for (unsigned i = 0; i < rdPtr->ovlpFilterCount; ++i) {
        if (rdPtr->ovlpFilters[i].type == OFT_SUBLAYER) {
            unsigned sub = rdPtr->ovlpFilters[i].param;
            if (sub <= 16)
                rdPtr->sublayerCache[sub] = &layer.subLayers[sub];
        }
    }
}