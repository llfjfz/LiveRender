#ifndef __CLIENT_SURFACE9__
#define __CLIENT_SURFACE9__

#include "game_client.h"

class ClientSurface9 {
private:
	IDirect3DSurface9* m_surface;

public:
	void ReplaceSurface(IDirect3DSurface9 * pnew);
	ClientSurface9(IDirect3DSurface9* ptr);
	IDirect3DSurface9* GetSurface9();
};

#endif