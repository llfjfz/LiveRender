#include "client_surface9.h"
void ClientSurface9::ReplaceSurface(IDirect3DSurface9 * pnew){
	this->m_surface->Release();
	this->m_surface = pnew;
}
ClientSurface9::ClientSurface9(IDirect3DSurface9* ptr): m_surface(ptr) {
	
}

IDirect3DSurface9* ClientSurface9::GetSurface9() {
	return m_surface;
}