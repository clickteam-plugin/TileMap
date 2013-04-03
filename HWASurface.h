inline cSurface* getPrototype(int depth)
{
	cSurface* proto = 0;

#ifdef HWABETA
	GetSurfacePrototype(&proto, depth, ST_HWA_ROUTEXTURE, SD_D3D9);
#else
	GetSurfacePrototype(&proto, depth, ST_MEMORY, SD_DIB);
#endif

	return proto;
}

inline bool copyBlit(cSurface& src, cSurface& dest)
{
#ifdef HWABETA
	return 0 != src.Blit(dest);
#else
	return 0 != src.Blit(dest, 0, 0, BMODE_OPAQUE, BOP_COPY, 0, BLTF_COPYALPHA);
#endif
}