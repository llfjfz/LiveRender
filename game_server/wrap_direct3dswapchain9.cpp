#include "wrap_direct3dswapchain9.h"
#include "wrap_direct3dsurface9.h"


WrapperDirect3DSwapChain9::WrapperDirect3DSwapChain9(IDirect3DSwapChain9* ptr, int _id): m_chain(ptr), id(_id) {
	Log::log("WrapperDirect3DSwapChain9::WrapperDirect3DSwapChain9() called\n");
	m_list.AddMember(ptr, this);
}

int WrapperDirect3DSwapChain9::GetID() {
	return this->id;
}

void WrapperDirect3DSwapChain9::SetID(int id) {
	this->id = id;
}

WrapperDirect3DSwapChain9* WrapperDirect3DSwapChain9::GetWrapperSwapChain9(IDirect3DSwapChain9* ptr) {
	Log::log("WrapperDirect3DSwapChain9::GetWrapperSwapChain9() called\n");
	WrapperDirect3DSwapChain9* ret = (WrapperDirect3DSwapChain9*)( m_list.GetDataPtr(ptr) );

	if(ret == NULL) {
		Log::log("WrapperDirect3DSwapChain9::GetWrapperSwapChain9(), ret is NULL, creating a new one\n");
		//ret = new WrapperDirect3DSwapChain9(ptr, WrapperDirect3DSwapChain9::ins_count++);
	}

	return ret;
}

/*** IUnknown methods ***/
STDMETHODIMP WrapperDirect3DSwapChain9::QueryInterface(THIS_ REFIID riid, void** ppvObj) {
	Log::log("WrapperDirect3DSwapChain9::QueryInterface() called\n");
	HRESULT hr = m_chain->QueryInterface(riid, ppvObj);
	*ppvObj = this;
	return hr;
}
STDMETHODIMP_(ULONG) WrapperDirect3DSwapChain9::AddRef(THIS) {
	Log::log("WrapperDirect3DSwapChain9::AddRef() called\n");
	return m_chain->AddRef();
}
STDMETHODIMP_(ULONG) WrapperDirect3DSwapChain9::Release(THIS) {
	Log::log("WrapperDirect3DSwapChain9::Release() called\n");
	return m_chain->Release();
}

/*** IDirect3DSwapChain9 methods ***/
STDMETHODIMP WrapperDirect3DSwapChain9::Present(THIS_ CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion,DWORD dwFlags) {
	Log::log("WrapperDirect3DSwapChain9::Present() called\n");

	cs.begin_command(SwapChainPresent_Opcode, id);
	cs.end_command();

	/////////////////////////////////////////////////////////////////////
	//record the fps of the game
#if 0
	static float last_time = (float)timeGetTime();

	static float elapse_time = 0.0;
	static float frame_cnt = 0.0;
	static float fps = 0.0;

	float cur_time = (float)timeGetTime();
	elapse_time += (cur_time - last_time) * 0.001;
	last_time = cur_time;

	frame_cnt++;

	if(elapse_time >= 1.0) {
		fps = frame_cnt / elapse_time;
		frame_cnt = 0;
		elapse_time = 0;

		Log::slog("WrapperDirect3DSwapChain9::Present(), fps=%.3f\n", fps);
	}
#endif
	/////////////////////////////////////////////////////////////////////

	return m_chain->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
}

STDMETHODIMP WrapperDirect3DSwapChain9::GetFrontBufferData(THIS_ IDirect3DSurface9* pDestSurface) {
	Log::log("WrapperDirect3DSwapChain9::GetFrontBufferData() TODO\n");
	return m_chain->GetFrontBufferData(pDestSurface);
}

STDMETHODIMP WrapperDirect3DSwapChain9::GetBackBuffer(THIS_ UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer) {
	Log::log("WrapperDirect3DSwapChain9::GetBackBuffer() called\n");
	IDirect3DSurface9* base_surface = NULL;
	HRESULT hr = m_chain->GetBackBuffer(iBackBuffer, Type, &base_surface);

	WrapperDirect3DSurface9* surface = WrapperDirect3DSurface9::GetWrapperSurface9(base_surface);
	if(surface == NULL) {
		cs.begin_command(SwapChainGetBackBuffer_Opcode, id);
		cs.write_int(WrapperDirect3DSurface9::ins_count);
		cs.write_uint(iBackBuffer);
		cs.write_uint(Type);
		cs.end_command();

		surface = new WrapperDirect3DSurface9(base_surface, WrapperDirect3DSurface9::ins_count++);
	}

	*ppBackBuffer = surface;

	Log::log("WrapperDirect3DSwapChain9::GetBackBuffer(), ppBackBuffer=%d\n", *ppBackBuffer);
	return hr;
}

STDMETHODIMP WrapperDirect3DSwapChain9::GetRasterStatus(THIS_ D3DRASTER_STATUS* pRasterStatus) {
	Log::log("WrapperDirect3DSwapChain9::GetRasterStatus() called\n");
	return m_chain->GetRasterStatus(pRasterStatus);
}

STDMETHODIMP WrapperDirect3DSwapChain9::GetDisplayMode(THIS_ D3DDISPLAYMODE* pMode) {
	Log::log("WrapperDirect3DSwapChain9::GetDisplayMode() called\n");
	return m_chain->GetDisplayMode(pMode);
}

STDMETHODIMP WrapperDirect3DSwapChain9::GetDevice(THIS_ IDirect3DDevice9** ppDevice) {
	Log::log("WrapperDirect3DSwapChain9::GetDevice() TODO\n");
	return m_chain->GetDevice(ppDevice);
}

STDMETHODIMP WrapperDirect3DSwapChain9::GetPresentParameters(THIS_ D3DPRESENT_PARAMETERS* pPresentationParameters) {
	Log::log("WrapperDirect3DSwapChain9::GetPresentParameters() TODO\n");
	return m_chain->GetPresentParameters(pPresentationParameters);
}


