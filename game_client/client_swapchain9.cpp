#include "client_swapchain9.h"

ClientSwapChain9::ClientSwapChain9(IDirect3DSwapChain9* ptr): m_chain(ptr) {

}

IDirect3DSwapChain9* ClientSwapChain9::GetSwapChain9() {
	Log::log("ClientSwapChain9::GetSwapChain9() called\n");
	return this->m_chain;
}

HRESULT ClientSwapChain9::Present(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion,DWORD dwFlags) {
	return m_chain->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
}

