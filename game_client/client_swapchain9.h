#ifndef __CLIENT_SWAPCHAIN9__
#define __CLIENT_SWAPCHAIN9__

#include "game_client.h"

class ClientSwapChain9 {
private:
	IDirect3DSwapChain9* m_chain;

public:
	ClientSwapChain9(IDirect3DSwapChain9* ptr);
	IDirect3DSwapChain9* GetSwapChain9();
	HRESULT Present(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion,DWORD dwFlags);
};

#endif
