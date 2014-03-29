#include "game_server.h"
#include "wrap_direct3d9.h"
#include "wrap_direct3ddevice9.h"
#include "wrap_direct3dvertexbuffer9.h"
#include "wrap_direct3dindexbuffer9.h"
#include "wrap_direct3dvertexdeclaration9.h"
#include "wrap_direct3dvertexshader9.h"
#include "wrap_direct3dpixelshader9.h"
#include "wrap_direct3dtexture9.h"
#include "wrap_direct3dstateblock9.h"
#include "wrap_direct3dcubetexture9.h"
#include "wrap_direct3dswapchain9.h"
#include "wrap_direct3dsurface9.h"
#include "wrap_direct3dvolumetexture9.h"
#include "opcode.h"

//#define SCRATCH_MEMO
#define DELAY_TO_DRAW

bool tex_send[4024] = {0};

WrapperDirect3DDevice9::WrapperDirect3DDevice9(IDirect3DDevice9* device, int _id): m_device(device), id(_id) {
	Log::log("I am in WrapperDirect3DDevice9\n");
	m_list.AddMember(device, this);

	cur_decl_ = NULL;
	cur_ib_ = NULL;

	is_even_frame_ = 1;

	for(int i=0; i<Source_Count; ++i) {
		cur_vbs_[i] = NULL;
	}
}

int WrapperDirect3DDevice9::GetID() {
	return this->id;
}

void WrapperDirect3DDevice9::SetID(int id) {
	this->id = id;
}

WrapperDirect3DDevice9* WrapperDirect3DDevice9::GetWrapperDevice9(IDirect3DDevice9* ptr) {
	//Log::slog("WrapperDirect3DDevice9::GetWrapperDevice9(), ptr=%u\n", ptr);
	WrapperDirect3DDevice9* ret = (WrapperDirect3DDevice9*)( m_list.GetDataPtr(ptr) );
	if(ret == NULL) {
		Log::log("WrapperDirect3DDevice9::GetWrapperDevice9(), ret is NULL\n");
	}
	return ret;
}

STDMETHODIMP WrapperDirect3DDevice9::QueryInterface(THIS_ REFIID riid, void** ppvObj) {
	Log::log("WrapperDirect3DDevice9::QueryInterface() called, base_device=%d, this=%d, riid: %d, ppvObj=%d \n", m_device, this,riid, *ppvObj);
	HRESULT hr = m_device->QueryInterface(riid, ppvObj);
	*ppvObj = this;
	if(hr == E_NOINTERFACE){
		Log::log("WrapperDirect3DDevice9::QueryInterface() failed!, No Interface, RIID:%d\n",riid);
		
	}
	Log::log("WrapperDirect3DDevice9::QueryInterface() end, base_device=%d, this=%d, riid: %d, ppvObj=%d\n", m_device, this,riid, *ppvObj);
	return hr;
	//return S_OK;
}
STDMETHODIMP_(ULONG) WrapperDirect3DDevice9::AddRef(THIS) {
	Log::log("WrapperDirect3DDevice9::AddRef() called\n");
	return m_device->AddRef();
}
STDMETHODIMP_(ULONG) WrapperDirect3DDevice9::Release(THIS) {
	Log::log("WrapperDirect3DDevice9::Release() called\n");
	return m_device->Release();
	//return D3D_OK;
}

/*** IDirect3DDevice9 methods ***/

STDMETHODIMP WrapperDirect3DDevice9::TestCooperativeLevel(THIS){
	Log::log("WrapperDirect3DDevice9::TestCooperativeLevel() called\n");
	return m_device->TestCooperativeLevel();
}

STDMETHODIMP_(UINT) WrapperDirect3DDevice9::GetAvailableTextureMem(THIS) { return m_device->GetAvailableTextureMem(); }

STDMETHODIMP WrapperDirect3DDevice9::EvictManagedResources(THIS) {
	Log::log("WrapperDirect3DDevice9::EvictManagedResources() TODO\n");
	return m_device->EvictManagedResources();
}

STDMETHODIMP WrapperDirect3DDevice9::GetDirect3D(THIS_ IDirect3D9** ppD3D9) {
	Log::log("WrapperDirect3DDevice9::GetDirect3D() called\n");
	LPDIRECT3D9 base_d3d9;
	HRESULT hr = m_device->GetDirect3D(&base_d3d9);

	*ppD3D9 = WrapperDirect3D9::GetWrapperD3D9(base_d3d9);

	return hr;
}
STDMETHODIMP WrapperDirect3DDevice9::GetDeviceCaps(THIS_ D3DCAPS9* pCaps) {
	Log::log("WrapperDirect3DDevice9::GetDeviceCaps() called\n");
	return m_device->GetDeviceCaps(pCaps);
}
STDMETHODIMP WrapperDirect3DDevice9::GetDisplayMode(THIS_ UINT iSwapChain,D3DDISPLAYMODE* pMode) { 
	Log::log("WrapperDirect3DDevice9::GetDisplayMode() called\n");
	return m_device->GetDisplayMode(iSwapChain, pMode); }
STDMETHODIMP WrapperDirect3DDevice9::GetCreationParameters(THIS_ D3DDEVICE_CREATION_PARAMETERS *pParameters) { 
	Log::log("WrapperDirect3DDevice9::GetCreationParameters() TODO\n");
	return m_device->GetCreationParameters(pParameters); }

STDMETHODIMP WrapperDirect3DDevice9::SetCursorProperties(THIS_ UINT XHotSpot,UINT YHotSpot,IDirect3DSurface9* pCursorBitmap) {
	Log::log("WrapperDirect3DDevice9::SetCursorProperties() TODO\n");
	return m_device->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap);
}

STDMETHODIMP_(void) WrapperDirect3DDevice9::SetCursorPosition(THIS_ int X,int Y,DWORD Flags) {
	Log::log("WrapperDirect3DDevice9::SetCursorPosition() TODO\n");
	return m_device->SetCursorPosition(X, Y, Flags);
}

STDMETHODIMP_(BOOL) WrapperDirect3DDevice9::ShowCursor(THIS_ BOOL bShow) {
	Log::log("WrapperDirect3DDevice9::ShowCursor() TODO\n");
	return m_device->ShowCursor(bShow);
}

STDMETHODIMP WrapperDirect3DDevice9::CreateAdditionalSwapChain(THIS_ D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DSwapChain9** pSwapChain) {
	Log::log("WrapperDirect3DDevice9::CreateAdditionalSwapChain() TODO\n");
	return m_device->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain);
}

STDMETHODIMP WrapperDirect3DDevice9::GetSwapChain(THIS_ UINT iSwapChain,IDirect3DSwapChain9** pSwapChain) {
	Log::log("WrapperDirect3DDevice9::GetSwapChain() called\n");
	IDirect3DSwapChain9* base_chain = NULL;
	HRESULT hr = m_device->GetSwapChain(iSwapChain, &base_chain);

	WrapperDirect3DSwapChain9* chain = WrapperDirect3DSwapChain9::GetWrapperSwapChain9(base_chain);

	if(chain == NULL) {
		cs.begin_command(GetSwapChain_Opcode, id);
		cs.write_int(WrapperDirect3DSwapChain9::ins_count);
		cs.write_uint(iSwapChain);
		

		chain = new WrapperDirect3DSwapChain9(base_chain, WrapperDirect3DSwapChain9::ins_count++);
		cs.end_command();
	}

	*pSwapChain = chain;

	return hr;
}

STDMETHODIMP_(UINT) WrapperDirect3DDevice9::GetNumberOfSwapChains(THIS) { return m_device->GetNumberOfSwapChains(); }

STDMETHODIMP WrapperDirect3DDevice9::Reset(THIS_ D3DPRESENT_PARAMETERS* pPresentationParameters) {
	Log::log("WrapperDirect3DDevice9::Reset() called\n");
#if 0
	cs.begin_command(Reset_Opcode, id);

	cs.write_byte_arr((char*)pPresentationParameters, sizeof(D3DPRESENT_PARAMETERS));
	cs.end_command();
#endif
	return m_device->Reset(pPresentationParameters);
}
int presented = 0;
extern CRITICAL_SECTION f9;
extern int serverInputArrive;

STDMETHODIMP WrapperDirect3DDevice9::Present(THIS_ CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion) {
	Log::log("WrapperDirect3DDevice9::Present(), source %d, dst %d, wind %d, rgbdata %d\n", pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
	
	EnterCriticalSection(&f9);
	if(F9Pressed && presented > 1){
		Log::log("send null instruction, deal input total use:%d\n", GetTickCount() - serverInputArrive);

		cs.begin_command(NULLINSTRUCT_Opcode, id);
		SYSTEMTIME sys, clientSys;
		GetLocalTime(&sys);
		cs.end_command(1);

		F9Pressed = false;
		presented = 0;
	}
	LeaveCriticalSection(&f9);
	presented++;

	cs.begin_command(Present_Opcode, id);
	cs.write_int(0);
	cs.write_int(0);
	cs.write_int(0);
	cs.write_int(0);
	

	/////////////////////////////////////////////////////////////////////
	//limit it to max_fps
#if 1
	static double last_time = timeGetTime();
	
	static double elapse_time = 0.0;
	static double frame_cnt = 0.0;
	static double fps = 0.0;

	double frame_time = 0.0f;

	double cur_time = (float)timeGetTime();
	elapse_time += (cur_time - last_time);
	frame_time = cur_time - last_time;
	last_time = cur_time;
	
	frame_cnt++;

	//limit it to max_fps
	double to_sleep = 1000.0 / cs.config_->max_fps_ * frame_cnt - elapse_time;
	if(to_sleep > 0) {
		//DWORD t1 = timeGetTime();

		Sleep((DWORD)to_sleep);
		//DWORD t2 = timeGetTime();

		//Log::slog("WrapperDirect3DDevice9::Present(), exp=%d, slp=%d\n", (DWORD)to_sleep, t2 - t1);
	}

	if(elapse_time >= 1000.0) {
		fps = frame_cnt * 1000.0 / elapse_time;
		frame_cnt = 0;
		elapse_time = 0;
		Log::slog("WrapperDirect3DDevice9::Present(), exp=%d fps=%.3f frame time:%.3f\n", cs.config_->max_fps_, fps,frame_time);
	}
#endif
	/////////////////////////////////////////////////////////////////////
	cs.end_command();

	is_even_frame_ ^= 1;
	
	HRESULT hh = m_device->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
	
	return hh;
}
STDMETHODIMP WrapperDirect3DDevice9::GetBackBuffer(THIS_ UINT iSwapChain,UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer) {
	Log::log("WrapperDirect3DDevice9::GetBackBuffer() called, iSwapChain:%d, iBackBuffer:%d, Buffer type:%d\n", iSwapChain, iBackBuffer, Type);
	IDirect3DSurface9* base_surface = NULL;
	HRESULT hr = m_device->GetBackBuffer(iSwapChain, iBackBuffer, Type, &base_surface);

	if(hr== D3D_OK){
		cs.begin_command(D3DDeviceGetBackBuffer_Opcode, id);
		cs.write_int(WrapperDirect3DSurface9::ins_count);

		cs.write_uint(iSwapChain);
		cs.write_uint(iBackBuffer);
		cs.write_uint(Type);
		cs.end_command();
		
		WrapperDirect3DSurface9* ret = new WrapperDirect3DSurface9(base_surface, WrapperDirect3DSurface9::ins_count++);
		// return the surface
		*ppBackBuffer = dynamic_cast<IDirect3DSurface9*>(ret);
		
	}else{
		Log::log("ERROR! WrapperDirect3DDevice::GetBackBuffer() failed!\n");
	}
	return hr;
}
STDMETHODIMP WrapperDirect3DDevice9::GetRasterStatus(THIS_ UINT iSwapChain,D3DRASTER_STATUS* pRasterStatus) {
	Log::log("WrapperDirect3DDevice9::GetRasterStatus() TODO\n");
	return m_device->GetRasterStatus(iSwapChain, pRasterStatus);}

STDMETHODIMP WrapperDirect3DDevice9::SetDialogBoxMode(THIS_ BOOL bEnableDialogs) {
	Log::log("WrapperDirect3DDevice9::SetDialogBoxMode() TODO\n");
	return m_device->SetDialogBoxMode(bEnableDialogs);
}

STDMETHODIMP_(void) WrapperDirect3DDevice9::SetGammaRamp(THIS_ UINT iSwapChain,DWORD Flags,CONST D3DGAMMARAMP* pRamp) {
	Log::log("WrapperDirect3DDevice9::SetGammaRamp() called\n");

	cs.begin_command(SetGammaRamp_Opcode, id);
	cs.write_uint(iSwapChain);
	cs.write_uint(Flags);
	cs.write_byte_arr((char*)pRamp, sizeof(D3DGAMMARAMP));
	cs.end_command();

	return m_device->SetGammaRamp(iSwapChain, Flags, pRamp);
}
STDMETHODIMP_(void) WrapperDirect3DDevice9::GetGammaRamp(THIS_ UINT iSwapChain,D3DGAMMARAMP* pRamp) {return m_device->GetGammaRamp(iSwapChain, pRamp);}

STDMETHODIMP WrapperDirect3DDevice9::CreateTexture(THIS_ UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9** ppTexture,HANDLE* pSharedHandle) {
	Log::log("WrapperDirect3DDevice9::CreateTexture() called\n");

	cs.begin_command(CreateTexture_Opcode, id);
	cs.write_int(WrapperDirect3DTexture9::ins_count);
	cs.write_uint(Width);
	cs.write_uint(Height);
	cs.write_uint(Levels);
	cs.write_uint(Usage);
	cs.write_uint(Format);
	cs.write_uint(Pool);
	cs.end_command();


	Log::log("WrapperDirect3DDevice9::CreateTexture(), width=%d, height=%d, Usage=%d, id:%d\n", Width, Height, Usage, WrapperDirect3DTexture9::ins_count);

	WrapperDirect3DTexture9::ins_count++;
	LPDIRECT3DTEXTURE9 base_tex = NULL;

	HRESULT hr = m_device->CreateTexture(Width, Height, Levels, Usage, Format, Pool, &base_tex, pSharedHandle);	

	if(SUCCEEDED(hr)) {

		WrapperDirect3DTexture9 * wt = new WrapperDirect3DTexture9(base_tex, WrapperDirect3DTexture9::ins_count - 1);
		*ppTexture = dynamic_cast<IDirect3DTexture9*>(wt);
		wt->Format = Format;
		wt->Levels = Levels;
		wt->Height = Height;
		wt->Width = Width;
		wt->usage = Usage;
		wt->Pool = Pool;
		Log::log("WrapperDirect3DDevice9::CreateTexture() end construct WrapperTexture,id:%d\n",WrapperDirect3DTexture9::ins_count);
		//*ppTexture = base_tex;
	}
	else {
		Log::log("WrapperDirect3DDevice9::CreateTexture() failed\n");
	}
	
	return hr;
}

STDMETHODIMP WrapperDirect3DDevice9::CreateVolumeTexture(THIS_ UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9** ppVolumeTexture,HANDLE* pSharedHandle) {
	Log::log("WrapperDirect3DDevice9::CreateVolumeTexture() called, Width:%d, Height:%d, Depth:%d, Levels:%d, Usage:%d, Format:%d, Pool:%d\n",Width, Height, Depth, Levels,Usage, Format, Pool);
	IDirect3DVolumeTexture9 * base_tex = NULL;
	HRESULT hr = m_device->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, &base_tex, pSharedHandle);
	if(SUCCEEDED(hr)){
		Log::log("WrapperDirect3DDevice9::CreateVolumeTexture() succeeded! base_tex:%d id:%d\n", base_tex,WrapperDirect3DVolumeTexture9::ins_count);
	
		WrapperDirect3DVolumeTexture9 * w_v_t = new WrapperDirect3DVolumeTexture9(base_tex, WrapperDirect3DVolumeTexture9::ins_count++);
		*ppVolumeTexture = dynamic_cast<IDirect3DVolumeTexture9*>(w_v_t);
	}
	else{
		Log::log("WrapperDirect3DDevice9::CreateVolumeTexture failed! ret:%d\n", hr);
	}
	return hr;
}

STDMETHODIMP WrapperDirect3DDevice9::CreateCubeTexture(THIS_ UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9** ppCubeTexture,HANDLE* pSharedHandle) {
	Log::log("WrapperDirect3DDevice9::CreateCubeTexture() called\n");
	IDirect3DCubeTexture9* base_cube_tex = NULL;

	cs.begin_command(CreateCubeTexture_Opcode, id);
	cs.write_int(WrapperDirect3DCubeTexture9::ins_count);
	cs.write_uint(EdgeLength);
	cs.write_uint(Levels);
	cs.write_uint(Usage);
	cs.write_uint(Format);
	cs.write_uint(Pool);
	cs.end_command();

	HRESULT hr = m_device->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, &base_cube_tex, pSharedHandle);

	if(SUCCEEDED(hr)) {
		*ppCubeTexture = dynamic_cast<IDirect3DCubeTexture9*>(new WrapperDirect3DCubeTexture9(base_cube_tex, WrapperDirect3DCubeTexture9::ins_count++));
	}
	else {
		Log::log("WrapperDirect3DDevice9::CreateCubeTexture() failed\n");
	}

	return hr;
}

STDMETHODIMP WrapperDirect3DDevice9::CreateVertexBuffer(THIS_ UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9** ppVertexBuffer,HANDLE* pSharedHandle) {
	Log::log("WrapperDirect3DDevice9::CreateVertextBuffer invoked! Usage:%d, FVF:%d, Pool:%d\n",Usage, FVF, Pool);
	
	LPDIRECT3DVERTEXBUFFER9 base_vb = NULL;

	cs.begin_command(CreateVertexBuffer_Opcode, id);
	cs.write_uint(WrapperDirect3DVertexBuffer9::ins_count);
	cs.write_uint(Length);
	cs.write_uint(Usage);
	cs.write_uint(FVF);
	cs.write_uint(Pool);
	cs.end_command();
	
	HRESULT hr = m_device->CreateVertexBuffer(Length, Usage, FVF, Pool, &base_vb, pSharedHandle);
	
	WrapperDirect3DVertexBuffer9::ins_count++;

	if(SUCCEEDED(hr)) {
		WrapperDirect3DVertexBuffer9 * wvb = new WrapperDirect3DVertexBuffer9(base_vb, WrapperDirect3DVertexBuffer9::ins_count - 1, Length);
		wvb->Usage = Usage;
		wvb->FVF = FVF;
		Log::log("vb id:%d, FVF:%d\n", wvb->GetId(), wvb->FVF);
		wvb->Pool = Pool;
		if(wvb)
			*ppVertexBuffer = dynamic_cast<IDirect3DVertexBuffer9*>(wvb);
		else{
			Log::log("WrapperDirect3DDevice9 constructor Failed\n");
		}
		Log::log("vb id:%d, size:%d\n", wvb->GetId(), wvb->GetLength());
	}
	else {
		Log::log("WrapperDirect3DDevice9::CreateVertexBuffer Failed\n");
	}
	Log::log("WrapperDirect3DDevice9::CreateVertexBuffer end,id:%d\n",WrapperDirect3DVertexBuffer9::ins_count - 1);

	
	return hr;
}

STDMETHODIMP WrapperDirect3DDevice9::CreateIndexBuffer(THIS_ UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9** ppIndexBuffer,HANDLE* pSharedHandle) {
	Log::log("WrapperDirect3DDevice9::CreateIndexBuffer invoked! Usage:%d, Format:%d, Pool:%d\n",Usage, Format, Pool);

	LPDIRECT3DINDEXBUFFER9 base_ib = NULL;

	cs.begin_command(CreateIndexBuffer_Opcode, id);
	cs.write_uint(WrapperDirect3DIndexBuffer9::ins_count);
	cs.write_uint(Length);
	cs.write_uint(Usage);
	cs.write_uint(Format);
	cs.write_uint(Pool);
	cs.end_command();

	HRESULT hr = m_device->CreateIndexBuffer(Length, Usage, Format, Pool, &base_ib, pSharedHandle);
	
	
	WrapperDirect3DIndexBuffer9::ins_count++;

	if(SUCCEEDED(hr)) {
		WrapperDirect3DIndexBuffer9 * wib = new WrapperDirect3DIndexBuffer9(base_ib, WrapperDirect3DIndexBuffer9::ins_count - 1, Length);
		wib->Usage = Usage;
		wib->Format = Format;
		wib->Pool = Pool;
		*ppIndexBuffer = dynamic_cast<IDirect3DIndexBuffer9*>(wib);
		
		if(*ppIndexBuffer == NULL) {
			Log::log("WrapperDirect3DDevice9::CreateIndexBuffer(), IndexBuffer is NULL\n");
		}
		else {
			Log::log("WrapperDirect3DDevice9::CreateIndexBuffer(), IndexBuffer id=%d, length=%d\n", ((WrapperDirect3DIndexBuffer9*)*ppIndexBuffer)->GetID(), ((WrapperDirect3DIndexBuffer9*)*ppIndexBuffer)->GetLength());
		}

		Log::log("ib: id:%d, size:%d\n", wib->GetID(), wib->GetLength());
	}
	else {
		Log::log("WrapperDirect3DDevice9::CreateIndexBuffer Failed\n");
	}

	return hr;
}

STDMETHODIMP WrapperDirect3DDevice9::CreateRenderTarget(THIS_ UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle) {
	Log::log("WrapperDirect3DDevice9::CreateRenderTarget() TODO\n");
	return m_device->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
}

STDMETHODIMP WrapperDirect3DDevice9::CreateDepthStencilSurface(THIS_ UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle) {
	/*TODO*/
	Log::log("WrapperDirect3DDevice9::CreateDepthStencilSurface() called\n");
	IDirect3DSurface9* base_surface = NULL;

	HRESULT hr = m_device->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, &base_surface, pSharedHandle);

	cs.begin_command(CreateDepthStencilSurface_Opcode, id);
	cs.write_int(WrapperDirect3DSurface9::ins_count);
	cs.write_uint(Width);
	cs.write_uint(Height);
	cs.write_uint(Format);
	cs.write_uint(MultiSample);
	cs.write_uint(MultisampleQuality);
	cs.write_int(Discard);
	cs.end_command();

	*ppSurface = new WrapperDirect3DSurface9(base_surface, WrapperDirect3DSurface9::ins_count++);
	
	return hr;
}

STDMETHODIMP WrapperDirect3DDevice9::UpdateSurface(THIS_ IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestinationSurface,CONST POINT* pDestPoint) {
	Log::log("WrapperDirect3DDevice9::UpdateSurface() TODO\n");
	IDirect3DSurface9* psrc = pDestinationSurface ? ((WrapperDirect3DSurface9*)pSourceSurface)->GetSurface9() : NULL;
	IDirect3DSurface9* pdst = pDestinationSurface ? ((WrapperDirect3DSurface9*)pSourceSurface)->GetSurface9() : NULL;
	return m_device->UpdateSurface(psrc, pSourceRect, pdst, pDestPoint);
}
STDMETHODIMP WrapperDirect3DDevice9::UpdateTexture(THIS_ IDirect3DBaseTexture9* pSourceTexture,IDirect3DBaseTexture9* pDestinationTexture) {
	Log::log("WrapperDirect3DDevice9::UpdateTexture() TODO\n");

	return m_device->UpdateTexture(pSourceTexture, pDestinationTexture);
}
STDMETHODIMP WrapperDirect3DDevice9::GetRenderTargetData(THIS_ IDirect3DSurface9* pRenderTarget,IDirect3DSurface9* pDestSurface) {
	Log::log("WrapperDirect3DDevice9::GetRenderTargetData() TODO\n");
	// check the surface's right

	if(pRenderTarget == NULL || pDestSurface == NULL) {
		Log::log("WrapperDirect3DDevice9::GetRenderTargetData(), surface is NULL\n");
	}

	WrapperDirect3DSurface9 * src = NULL, * dst = NULL;
	src = (WrapperDirect3DSurface9 *)pRenderTarget;
	dst = (WrapperDirect3DSurface9 *)pDestSurface;
	Log::log("WrapperDirect3DSurface::getRenderTargetData called! src id:%d, dst id:%d, src IDirect3DSurface:%d, dst IDirect3DSurface:%d\n", src->GetID(), dst->GetID(), src->GetSurface9(), dst->GetSurface9());

	//return m_device->GetRenderTargetData(pRenderTarget, pDestSurface);
	return m_device->GetRenderTargetData(src->GetSurface9(), dst->GetSurface9());
}
STDMETHODIMP WrapperDirect3DDevice9::GetFrontBufferData(THIS_ UINT iSwapChain,IDirect3DSurface9* pDestSurface) {
	Log::log("WrapperDirect3DDevice9::GetFrontBufferData() TODO\n");
	return m_device->GetFrontBufferData(iSwapChain, pDestSurface);
}
STDMETHODIMP WrapperDirect3DDevice9::StretchRect(THIS_ IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestSurface,CONST RECT* pDestRect,D3DTEXTUREFILTERTYPE Filter) {
	Log::log("WrapperDirect3DDevice9::StretchRect() TODO\n");
	return m_device->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
}

STDMETHODIMP WrapperDirect3DDevice9::ColorFill(THIS_ IDirect3DSurface9* pSurface,CONST RECT* pRect,D3DCOLOR color) {
	Log::log("WrapperDirect3DDevice9::ColorFill() TODO\n");
	return m_device->ColorFill(pSurface, pRect, color);
}

STDMETHODIMP WrapperDirect3DDevice9::CreateOffscreenPlainSurface(THIS_ UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle) {
	Log::log("WrapperDirect3DDevice9::CreateOffscreenPlainSurface() TODO\n");
	return m_device->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface, pSharedHandle);
}

STDMETHODIMP WrapperDirect3DDevice9::SetRenderTarget(THIS_ DWORD RenderTargetIndex,IDirect3DSurface9* pRenderTarget) {
	Log::log("WrapperDirect3DDevice9::SetRenderTarget(), pRenderTarget=%d\n", pRenderTarget);
	WrapperDirect3DSurface9 * sur = (WrapperDirect3DSurface9*)pRenderTarget;
	if(pRenderTarget == NULL) {

		cs.begin_command(SetRenderTarget_Opcode, id);
		cs.write_uint(RenderTargetIndex);
		cs.write_int(-1);
		cs.write_int(-1);
		cs.write_int(-1);
		
		HRESULT hh = m_device->SetRenderTarget(RenderTargetIndex, pRenderTarget);
		cs.end_command();
		return hh;
	}

	/*TODO*/
	cs.begin_command(SetRenderTarget_Opcode, id);
	cs.write_uint(RenderTargetIndex);
	cs.write_int( ((WrapperDirect3DSurface9*)pRenderTarget)->GetID() );
	cs.write_int( sur->GetTexId() );
	cs.write_int( sur->GetLevel() );
	cs.end_command();
	
	Log::log("WrapperDirect3DDevice9::SetRenderTarge(), surface id:%d, paranet texture id: %d, level: %d\n",((WrapperDirect3DSurface9*)pRenderTarget)->GetID(),sur->GetTexId(), sur->GetLevel());
	HRESULT hr = m_device->SetRenderTarget(RenderTargetIndex, ((WrapperDirect3DSurface9*)pRenderTarget)->GetSurface9());
	
	return hr;
}

STDMETHODIMP WrapperDirect3DDevice9::GetRenderTarget(THIS_ DWORD RenderTargetIndex,IDirect3DSurface9** ppRenderTarget) {
	Log::log("WrapperDirect3DDevice9::GetRenderTarget() called!\n");
	
	HRESULT hr;

	IDirect3DSurface9 * ret = NULL;
	hr = m_device->GetRenderTarget(RenderTargetIndex, &ret);
	if(SUCCEEDED(hr)){
		cs.begin_command(D3DDGetRenderTarget_Opcode, id);
		cs.write_int(WrapperDirect3DSurface9::ins_count);
		cs.write_uint(RenderTargetIndex);
		cs.end_command();
		(*ppRenderTarget)=dynamic_cast<IDirect3DSurface9*>(new WrapperDirect3DSurface9(ret, WrapperDirect3DSurface9::ins_count++));
		Log::log("wrapperDirect3DSurface9 new surface with id:%d\n", WrapperDirect3DSurface9::ins_count-1);
	}else{
		Log::log("WrapperDirect3DDevice::GetRenderTarget() failed! ins_count:%d\n", WrapperDirect3DSurface9::ins_count);
	}
	return hr;
}

STDMETHODIMP WrapperDirect3DDevice9::SetDepthStencilSurface(THIS_ IDirect3DSurface9* pNewZStencil) {
	Log::log("WrapperDirect3DDevice9::SetDepthStencilSurface(), pNewZStencil=%d\n", pNewZStencil);
	HRESULT hh;
	if(pNewZStencil == NULL) {
		cs.begin_command(SetDepthStencilSurface_Opcode, id);
		cs.write_int(-1);
		
		hh = m_device->SetDepthStencilSurface(NULL);
		cs.end_command();
		return hh;
	}
	
	cs.begin_command(SetDepthStencilSurface_Opcode, id);
	cs.write_int( ((WrapperDirect3DSurface9*)pNewZStencil)->GetID() );
	cs.end_command();
	
	hh = m_device->SetDepthStencilSurface(((WrapperDirect3DSurface9*)pNewZStencil)->GetSurface9());
	
	return hh;
}

STDMETHODIMP WrapperDirect3DDevice9::GetDepthStencilSurface(THIS_ IDirect3DSurface9** ppZStencilSurface) {
	Log::log("WrapperDirect3DDevice9::GetDepthStencilSurface() called\n");
	IDirect3DSurface9* base_surface = NULL;
	HRESULT hr = m_device->GetDepthStencilSurface(&base_surface);

	WrapperDirect3DSurface9* surface = NULL;
	surface = WrapperDirect3DSurface9::GetWrapperSurface9(base_surface);
	if(surface == NULL) {
		cs.begin_command(GetDepthStencilSurface_Opcode, id);
		cs.write_int(WrapperDirect3DSurface9::ins_count);
		cs.end_command();
		
		surface = new WrapperDirect3DSurface9(base_surface, WrapperDirect3DSurface9::ins_count++);
	}

	*ppZStencilSurface =dynamic_cast<IDirect3DSurface9*>( surface);
	return hr;
}

STDMETHODIMP WrapperDirect3DDevice9::BeginScene(THIS) {
	Log::log("WrapperDirect3DDevice9::BeginScene() invoke\n");

	cs.begin_command(BeginScene_Opcode, id);
	cs.end_command();
	
	HRESULT h = m_device->BeginScene();
	
	return h;
}
STDMETHODIMP WrapperDirect3DDevice9::EndScene(THIS) {
	Log::log("WrapperDirect3DDevice9::EndScene() called\n");

	cs.begin_command(EndScene_Opcode, id);
	cs.end_command();
	
	HRESULT hh = m_device->EndScene();
	
	return hh; 
}
STDMETHODIMP WrapperDirect3DDevice9::Clear(THIS_ DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil) {
	Log::log("WrapperDirect3DDevice9::Clear() called\n");

	cs.begin_command(Clear_Opcode, id);
	cs.write_uint(Count);
	if(pRects == NULL) {
		cs.write_char(1);
	}
	else {
		cs.write_char(0);
		cs.write_byte_arr((char*)pRects, sizeof(D3DRECT));
	}
	cs.write_uint(Flags);
	cs.write_uint(Color);
	cs.write_float(Z);
	cs.write_uint(Stencil);
	
	HRESULT hh =  m_device->Clear(Count, pRects, Flags, Color, Z, Stencil);
	cs.end_command();
	return hh;
}
STDMETHODIMP WrapperDirect3DDevice9::SetTransform(THIS_ D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix) {
	Log::log("WrapperDirect3DDevice9::SetTransform() called\n");
	if(pMatrix == NULL) {
		Log::log("WrapperDirect3DDevice9::SetTransform(), pMatrix is NULL!\n");
	}

	cs.begin_command(SetTransform_Opcode, id);
	cs.write_short(State);
	unsigned short *mask = (unsigned short*)(cs.get_cur_ptr(sizeof(unsigned short)));
	*mask = 0;
	
	for(int i=0; i<4; ++i) {
		for(int j=0; j<4; ++j) {
			//Log::log("matrix[%d][%d] = %f\n", i,j,pMatrix->m[i][j]);
			if(fabs(pMatrix->m[i][j]) > eps) {
				(*mask) ^= (1 << (i * 4 + j));
				cs.write_float(pMatrix->m[i][j]);
			}
		}
	}

	cs.end_command();
	
	HRESULT hh = m_device->SetTransform(State, pMatrix);
	
	return hh;

}
STDMETHODIMP WrapperDirect3DDevice9::GetTransform(THIS_ D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix) {return m_device->GetTransform(State, pMatrix);}
STDMETHODIMP WrapperDirect3DDevice9::MultiplyTransform(THIS_ D3DTRANSFORMSTATETYPE Type,CONST D3DMATRIX* pD3DMatrix) {
	Log::log("WrapperDirect3DDevice9::MultiplyTransform() TODO\n");
	return m_device->MultiplyTransform(Type, pD3DMatrix);}

STDMETHODIMP WrapperDirect3DDevice9::SetViewport(THIS_ CONST D3DVIEWPORT9* pViewport) {
	Log::log("WrapperDirect3DDevice9::SetViewport() called! v.x:%d, v.y:%d, v.Width:%d, v.Height:%d, v.Maxz:%d, v.Minz:%d\n",pViewport->X, pViewport->Y, pViewport->Width, pViewport->Height, pViewport->MaxZ, pViewport->MinZ);

	assert(pViewport);
	if(pViewport == NULL) {
		Log::log("WrapperDirect3DDevice9::SetViewport(), pViewport is NULL\n");
	}

	cs.begin_command(SetViewport_Opcode, id);
	cs.write_byte_arr((char*)pViewport, sizeof(*pViewport));
	cs.end_command();

	HRESULT hh = m_device->SetViewport(pViewport);
	
	return hh;
}

STDMETHODIMP WrapperDirect3DDevice9::GetViewport(THIS_ D3DVIEWPORT9* pViewport) {
	Log::log("WrapperDirect3DDevice9::GetViewport() TODO\n");
	return m_device->GetViewport(pViewport);
}

STDMETHODIMP WrapperDirect3DDevice9::SetMaterial(THIS_ CONST D3DMATERIAL9* pMaterial) {
	Log::log("WrapperDirect3DDevice9::SetMaterial() called\n");

	cs.begin_command(SetMaterial_Opcode, id);
	cs.write_byte_arr((char*)pMaterial, sizeof(D3DMATERIAL9));
	cs.end_command();

	HRESULT hh = m_device->SetMaterial(pMaterial);
	
	return hh;
}

STDMETHODIMP WrapperDirect3DDevice9::GetMaterial(THIS_ D3DMATERIAL9* pMaterial) {return m_device->GetMaterial(pMaterial);}

STDMETHODIMP WrapperDirect3DDevice9::SetLight(THIS_ DWORD Index,CONST D3DLIGHT9* pD3DLight9) {
	Log::log("WrapperDirect3DDevice9::SetLight called\n");

	cs.begin_command(SetLight_Opcode, id);
	cs.write_uint(Index);
	cs.write_byte_arr((char*)pD3DLight9, sizeof(D3DLIGHT9));
	cs.end_command();

	HRESULT hh = m_device->SetLight(Index, pD3DLight9);
	
	return hh;
}

STDMETHODIMP WrapperDirect3DDevice9::GetLight(THIS_ DWORD Index,D3DLIGHT9* pD3DLight9) {return m_device->GetLight(Index, pD3DLight9);}

STDMETHODIMP WrapperDirect3DDevice9::LightEnable(THIS_ DWORD Index,BOOL Enable) {
	Log::log("WrapperDirect3DDevice9::LightEnable() called\n");

	cs.begin_command(LightEnable_Opcode, id);
	cs.write_uint(Index);
	cs.write_int(Enable);
	cs.end_command();
	
	HRESULT hh = m_device->LightEnable(Index, Enable);
	
	return hh;
}

STDMETHODIMP WrapperDirect3DDevice9::GetLightEnable(THIS_ DWORD Index,BOOL* pEnable) {return m_device->GetLightEnable(Index, pEnable);}

STDMETHODIMP WrapperDirect3DDevice9::SetClipPlane(THIS_ DWORD Index,CONST float* pPlane) {
	Log::log("WrapperDirect3DDevice9::SetClipPlane() TODO\n");
	return m_device->SetClipPlane(Index, pPlane);
}

STDMETHODIMP WrapperDirect3DDevice9::GetClipPlane(THIS_ DWORD Index,float* pPlane) {return m_device->GetClipPlane(Index, pPlane);}
STDMETHODIMP WrapperDirect3DDevice9::SetRenderState(THIS_ D3DRENDERSTATETYPE State,DWORD Value) {
	Log::log("WrapperDirect3DDevice9::SetRenderState(), state=%d, value=%d\n", State, Value);

	cs.begin_command(SetRenderState_Opcode, id);
	cs.write_uint(State);
	cs.write_uint(Value);
	cs.end_command();

	HRESULT hh = m_device->SetRenderState(State, Value);
	
	return hh;
}
STDMETHODIMP WrapperDirect3DDevice9::GetRenderState(THIS_ D3DRENDERSTATETYPE State,DWORD* pValue) {return m_device->GetRenderState(State, pValue);}

STDMETHODIMP WrapperDirect3DDevice9::CreateStateBlock(THIS_ D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9** ppSB) {
	Log::log("WrapperDirect3DDevice9::CreateStateBlock() called\n");

	cs.begin_command(CreateStateBlock_Opcode, id);
	cs.write_uint(Type);
	cs.write_int(WrapperDirect3DStateBlock9::ins_count);
	cs.end_command();

	IDirect3DStateBlock9* sb_base = NULL;
	HRESULT hr = m_device->CreateStateBlock(Type, &sb_base);

	WrapperDirect3DStateBlock9::ins_count++;

	if(SUCCEEDED(hr)) {
		*ppSB = dynamic_cast<IDirect3DStateBlock9*>(new WrapperDirect3DStateBlock9(sb_base, WrapperDirect3DStateBlock9::ins_count - 1));
	}
	else {
		Log::log("WrapperDirect3DDevice9::CreateStateBlock() failed\n");
	}

	return hr;
}

STDMETHODIMP WrapperDirect3DDevice9::BeginStateBlock(THIS) {
	Log::log("WrapperDirect3DDevice9::BeginStateBlock() called\n");

	cs.begin_command(BeginStateBlock_Opcode, id);
	cs.end_command();

	HRESULT hh = m_device->BeginStateBlock();
	
	return hh;
}
STDMETHODIMP WrapperDirect3DDevice9::EndStateBlock(THIS_ IDirect3DStateBlock9** ppSB) {
	Log::log("WrapperDirect3DDevice9::EndStateBlock() called\n");
	
	cs.begin_command(EndStateBlock_Opcode, id);
	cs.write_int(WrapperDirect3DStateBlock9::ins_count);
	cs.end_command();

	IDirect3DStateBlock9* sb_base = NULL;
	HRESULT hr = m_device->EndStateBlock(&sb_base);

	WrapperDirect3DStateBlock9::ins_count++;

	if(SUCCEEDED(hr)) {
		*ppSB = dynamic_cast<IDirect3DStateBlock9*>(new WrapperDirect3DStateBlock9(sb_base, WrapperDirect3DStateBlock9::ins_count - 1));
	}
	else {
		Log::log("WrapperDirect3DDevice9::EndStateBlock() failed\n");
	}
	
	return hr;
}

STDMETHODIMP WrapperDirect3DDevice9::SetClipStatus(THIS_ CONST D3DCLIPSTATUS9* pClipStatus) {
	Log::log("WrapperDirect3DDevice9::SetClipStatus() TODO\n");
	return m_device->SetClipStatus(pClipStatus);
}

STDMETHODIMP WrapperDirect3DDevice9::GetClipStatus(THIS_ D3DCLIPSTATUS9* pClipStatus) {return m_device->GetClipStatus(pClipStatus);}

STDMETHODIMP WrapperDirect3DDevice9::GetTexture(THIS_ DWORD Stage,IDirect3DBaseTexture9** ppTexture) {
	Log::log("WrapperDirect3DDevice9::GetTexture() TODO\n");
	/*TODO*/

	/*
	LPDIRECT3DBASETEXTURE9 base_texture = NULL;
	HRESULT hr = m_device->GetTexture(Stage, &base_texture);

	*ppTexture = base_texture;
	*/
	return m_device->GetTexture(Stage, ppTexture);
	//return hr;
}

STDMETHODIMP WrapperDirect3DDevice9::SetTexture(THIS_ DWORD Stage,IDirect3DBaseTexture9* pTexture) {
	Log::log("WrapperDirect3DDevice9::SetTexture() called\n");

	
	if(pTexture == NULL) {
		Log::log("WrapperDirect3DDevice9::SetTexture(), pTexutre is NULL, I got it.\n");

		cs.begin_command(SetTexture_Opcode, this->id);
		cs.write_uint(Stage);
		cs.write_int(-1);
		cs.end_command();
		
		return m_device->SetTexture(Stage, pTexture);
	}
	
	D3DRESOURCETYPE Type = pTexture->GetType();

	if(Type == D3DRTYPE_TEXTURE) {
		int id = ((WrapperDirect3DTexture9*)pTexture)->GetID();
		Log::log("WrapperDirect3DDevice9::SetTexture(), Type is Texture stage=%d, id=%d, hit or not:%d\n", Stage, id, tex_send[id]);

		if(!tex_send[id]) {
			WrapperDirect3DTexture9* wt = (WrapperDirect3DTexture9*)pTexture;
			wt->SendTextureData();
			tex_send[id] = 1;
		}
	
		cs.begin_command(SetTexture_Opcode, this->id);
		cs.write_uint(Stage);
		cs.write_int( ((WrapperDirect3DTexture9*)pTexture)->GetID() );
		cs.end_command();

		HRESULT hh = m_device->SetTexture(Stage, ((WrapperDirect3DTexture9*)pTexture)->GetTex9());
		
		return hh;
	}
	else if(Type == D3DRTYPE_CUBETEXTURE) {
		int id = ((WrapperDirect3DCubeTexture9*)pTexture)->GetID();
		Log::log("WrapperDirect3DDevice9::SetTexture(), Type is CubeTexture id=%d\n", id);

		cs.begin_command(SetCubeTexture_Opcode, this->id);
		cs.write_uint(Stage);
		cs.write_int( ((WrapperDirect3DCubeTexture9*)pTexture)->GetID() );
		cs.end_command();

		HRESULT hh = m_device->SetTexture(Stage, ((WrapperDirect3DCubeTexture9*)pTexture)->GetCubeTex9());
		
		return hh;
	}
	else {
		Log::log("WrapperDirect3DDevice9::SetTexture(), Type is unknown\n");
		return m_device->SetTexture(Stage, pTexture);
	}
	//return m_device->SetTexture(Stage, pTexture);
}

STDMETHODIMP WrapperDirect3DDevice9::GetTextureStageState(THIS_ DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue) {
	Log::log("WrapperDirect3DDevice9::GetTextureStageState() TODO\n");
	return m_device->GetTextureStageState(Stage, Type, pValue);
}

STDMETHODIMP WrapperDirect3DDevice9::SetTextureStageState(THIS_ DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value) {
	Log::log("WrapperDirect3DDevice9::SetTextureStageState() called\n");

	cs.begin_command(SetTextureStageState_Opcode, id);
	cs.write_uint(Stage);
	cs.write_uint(Type);
	cs.write_uint(Value);
	cs.end_command();

	HRESULT hh = m_device->SetTextureStageState(Stage, Type, Value);
	
	return hh;
}

STDMETHODIMP WrapperDirect3DDevice9::GetSamplerState(THIS_ DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD* pValue) {return m_device->GetSamplerState(Sampler, Type, pValue);}
STDMETHODIMP WrapperDirect3DDevice9::SetSamplerState(THIS_ DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value) {
	Log::log("WrapperDirect3DDevice9::SetSamplerState() invoke\n");

	cs.begin_command(SetSamplerState_Opcode, id);
	cs.write_uint(Sampler);
	cs.write_uchar(Type);
	cs.write_uint(Value);
	cs.end_command();

	HRESULT hh = m_device->SetSamplerState(Sampler, Type, Value);
	
	return hh;
}

STDMETHODIMP WrapperDirect3DDevice9::ValidateDevice(THIS_ DWORD* pNumPasses) {
	Log::log("WrapperDirect3DDevice9::ValidateDevice() TODO\n");
	return m_device->ValidateDevice(pNumPasses);
}

STDMETHODIMP WrapperDirect3DDevice9::SetPaletteEntries(THIS_ UINT PaletteNumber,CONST PALETTEENTRY* pEntries) {
	Log::log("WrapperDirect3DDevice9::SetPaletteEntries() TODO\n");
	return m_device->SetPaletteEntries(PaletteNumber, pEntries);
}

STDMETHODIMP WrapperDirect3DDevice9::GetPaletteEntries(THIS_ UINT PaletteNumber,PALETTEENTRY* pEntries) {return m_device->GetPaletteEntries(PaletteNumber, pEntries);}
STDMETHODIMP WrapperDirect3DDevice9::SetCurrentTexturePalette(THIS_ UINT PaletteNumber) {
	Log::log("WrapperDirect3DDevice9::SetCurrentTexturePalette() TODO\n");
	return m_device->SetCurrentTexturePalette(PaletteNumber);
}
STDMETHODIMP WrapperDirect3DDevice9::GetCurrentTexturePalette(THIS_ UINT *PaletteNumber) {
	Log::log("WrapperDirect3DDevice9::GetCurrentTexturePalette() TODO\n");
	return m_device->GetCurrentTexturePalette(PaletteNumber);
}

STDMETHODIMP WrapperDirect3DDevice9::SetScissorRect(THIS_ CONST RECT* pRect) {
	Log::log("WrapperDirect3DDevice9::SetScissorRect() called, left :%d, top :%d, buttom :%d, right :%d\n",pRect->left, pRect->top, pRect->bottom, pRect->right);
	cs.begin_command(D3DDSetScissorRect_Opcode, id);
	cs.write_int(pRect->left);
	cs.write_int(pRect->right);
	cs.write_int(pRect->top);
	cs.write_int(pRect->bottom);

	cs.end_command();
	return m_device->SetScissorRect(pRect);
}

STDMETHODIMP WrapperDirect3DDevice9::GetScissorRect(THIS_ RECT* pRect) {return m_device->GetScissorRect(pRect);}

STDMETHODIMP WrapperDirect3DDevice9::SetSoftwareVertexProcessing(THIS_ BOOL bSoftware) {
	Log::log("WrapperDirect3DDevice9::SetSoftwareVertexProcessing() called\n");

	cs.begin_command(SetSoftwareVertexProcessing_Opcode, id);
	cs.write_int(bSoftware);
	
	HRESULT hh = m_device->SetSoftwareVertexProcessing(bSoftware);
	cs.end_command();
	return hh;
}

STDMETHODIMP_(BOOL) WrapperDirect3DDevice9::GetSoftwareVertexProcessing(THIS) {return m_device->GetSoftwareVertexProcessing();}

STDMETHODIMP WrapperDirect3DDevice9::SetNPatchMode(THIS_ float nSegments) {
	Log::log("WrapperDirect3DDevice9::SetNPatchMode() called\n");

	cs.begin_command(SetNPatchMode_Opcode, id);
	cs.write_float(nSegments);
	
	HRESULT hh = m_device->SetNPatchMode(nSegments);
	cs.end_command();
	return hh;
}

STDMETHODIMP_(float) WrapperDirect3DDevice9::GetNPatchMode(THIS) {return m_device->GetNPatchMode();}


/*
	place for DrawPrimitive functions
*/


STDMETHODIMP WrapperDirect3DDevice9::ProcessVertices(THIS_ UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9* pDestBuffer,IDirect3DVertexDeclaration9* pVertexDecl,DWORD Flags) {
	Log::log("WrapperDirect3DDevice9::ProcessVertices() TODO\n");
	return m_device->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);
}


/*
	place for CreateVertexDeclaration
*/


STDMETHODIMP WrapperDirect3DDevice9::GetVertexDeclaration(THIS_ IDirect3DVertexDeclaration9** ppDecl) {
	Log::log("WrapperDirect3DDevice9::GetVertexDeclaration() TODO\n");
	return m_device->GetVertexDeclaration(ppDecl);
}

STDMETHODIMP WrapperDirect3DDevice9::SetFVF(THIS_ DWORD FVF) {
	Log::log("WrapperDirect3DDevice9::SetFVF() invoke\n");

	cs.begin_command(SetFVF_Opcode, id);
	cs.write_uint(FVF);
	cs.end_command();

	return m_device->SetFVF(FVF);
}
STDMETHODIMP WrapperDirect3DDevice9::GetFVF(THIS_ DWORD* pFVF) {return m_device->GetFVF(pFVF);}

STDMETHODIMP WrapperDirect3DDevice9::CreateVertexShader(THIS_ CONST DWORD* pFunction,IDirect3DVertexShader9** ppShader) {
	Log::log("WrapperDirect3DDevice9::CreateVertexShader() called\n");

	cs.begin_command(CreateVertexShader_Opcode, id);
	int cnt = 0;
	while(true) {
		if(pFunction[cnt++] == D3DVS_END()) break;
	}
	cs.write_int(WrapperDirect3DVertexShader9::ins_count);
	cs.write_int(cnt);
	cs.write_byte_arr((char*)pFunction, sizeof(DWORD) * (cnt));
	

	WrapperDirect3DVertexShader9::ins_count++;
	LPDIRECT3DVERTEXSHADER9 base_vs = NULL;
	HRESULT hr = m_device->CreateVertexShader(pFunction, &base_vs);

	if(SUCCEEDED(hr)) {
		*ppShader = dynamic_cast<IDirect3DVertexShader9*>(new WrapperDirect3DVertexShader9(base_vs, WrapperDirect3DVertexShader9::ins_count - 1));
	}
	else {
		Log::log("WrapperDirect3DDevice9::CreateVertexShader() failed\n");
	}
	cs.end_command();
	return hr;
}

STDMETHODIMP WrapperDirect3DDevice9::SetVertexShader(THIS_ IDirect3DVertexShader9* pShader) {

	Log::log("WrapperDirect3DDevice9::SetVertexShader() called\n");
	HRESULT hh;
	if(pShader == NULL) {
		Log::log("WrapperDirect3DDevice9::SetVertexShader(), pShader is NULL\n");
		
		cs.begin_command(SetVertexShader_Opcode, id);
		cs.write_int(-1);
		cs.end_command();

		hh = m_device->SetVertexShader(NULL);
		

		return hh;
	}

	Log::log("WrapperDirect3DDevice9::SetVertexShader(), id=%d\n", ((WrapperDirect3DVertexShader9*)pShader)->GetID() );

	cs.begin_command(SetVertexShader_Opcode, id);
	cs.write_int(((WrapperDirect3DVertexShader9*)pShader)->GetID());
	
	hh = m_device->SetVertexShader(((WrapperDirect3DVertexShader9*)pShader)->GetVS9());
	cs.end_command();
	return hh;
}

STDMETHODIMP WrapperDirect3DDevice9::GetVertexShader(THIS_ IDirect3DVertexShader9** ppShader) {
	Log::log("WrapperDirect3DDevice9::GetVertexShader() called\n");
	LPDIRECT3DVERTEXSHADER9 base_vertex_shader = NULL;
	HRESULT hr = m_device->GetVertexShader(&base_vertex_shader);

	*ppShader = WrapperDirect3DVertexShader9::GetWrapperVertexShader(base_vertex_shader);
	return hr;
}

float vs_data[10000];

STDMETHODIMP WrapperDirect3DDevice9::SetVertexShaderConstantF(THIS_ UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount) {
	Log::log("WrapperDirect3DDevice9::SetVertexShaderConstantF() called\n");

	cs.begin_command(SetVertexShaderConstantF_Opcode, id);
	cs.write_ushort(StartRegister);
	cs.write_ushort(Vector4fCount);

	memcpy((char*)vs_data, (char*)pConstantData, Vector4fCount * 16);

	int i;
	for(i=0; i<Vector4fCount/2; ++i) {
		//Log::log("%f %f %f %f\n", *(p), *(p + 1), *(p + 2), *(p + 3));

		cs.write_vec(SetVertexShaderConstantF_Opcode, vs_data + (i * 8), 32);
	}

	if(Vector4fCount & 1) {
		cs.write_vec(SetVertexShaderConstantF_Opcode, vs_data + (i * 8), 16);
	}

	
	//cs.write_byte_arr((char*)pConstantData, sizeof(float) * (Vector4fCount * 4));
	cs.end_command();
	
	HRESULT hh = m_device->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
	return hh;

}

STDMETHODIMP WrapperDirect3DDevice9::GetVertexShaderConstantF(THIS_ UINT StartRegister,float* pConstantData,UINT Vector4fCount) {return m_device->GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);}

STDMETHODIMP WrapperDirect3DDevice9::SetVertexShaderConstantI(THIS_ UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount) {
	Log::log("WrapperDirect3DDevice9::SetVertexShaderConstantI() called\n");

	cs.begin_command(SetVertexShaderConstantI_Opcode, id);
	cs.write_uint(StartRegister);
	cs.write_uint(Vector4iCount);
	cs.write_byte_arr((char*)pConstantData, sizeof(int) * (Vector4iCount * 4));
	
	HRESULT hh = m_device->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
	cs.end_command();
	return hh;
}

STDMETHODIMP WrapperDirect3DDevice9::GetVertexShaderConstantI(THIS_ UINT StartRegister,int* pConstantData,UINT Vector4iCount) {return m_device->GetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);}

STDMETHODIMP WrapperDirect3DDevice9::SetVertexShaderConstantB(THIS_ UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount) {
	Log::log("WrapperDirect3DDevice9::SetVertexShaderConstantB() called\n");

	cs.begin_command(SetVertexShaderConstantB_Opcode, id);
	cs.write_uint(StartRegister);
	cs.write_uint(BoolCount);
	cs.write_byte_arr((char*)pConstantData, sizeof(BOOL) * BoolCount);
	
	HRESULT hh = m_device->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
	cs.end_command();
	return hh;
}

STDMETHODIMP WrapperDirect3DDevice9::GetVertexShaderConstantB(THIS_ UINT StartRegister,BOOL* pConstantData,UINT BoolCount) {return m_device->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);}

/*
	place for SetVertexDeclaration
*/


/*
	place for SetStreamSource
*/

STDMETHODIMP WrapperDirect3DDevice9::GetStreamSource(THIS_ UINT StreamNumber,IDirect3DVertexBuffer9** ppStreamData,UINT* pOffsetInBytes,UINT* pStride) {
	Log::log("WrapperDirect3DDevice9::GetStreamSource() called\n");

	LPDIRECT3DVERTEXBUFFER9 base_vertex_buffer = NULL;
	HRESULT hr = m_device->GetStreamSource(StreamNumber, &base_vertex_buffer, pOffsetInBytes, pStride);

	*ppStreamData = WrapperDirect3DVertexBuffer9::GetWrapperVertexBuffer9(base_vertex_buffer);
	return hr;
}

STDMETHODIMP WrapperDirect3DDevice9::SetStreamSourceFreq(THIS_ UINT StreamNumber,UINT Setting) {
	Log::log("WrapperDirect3DDevice9::SetStreamSourceFreq() TODO\n");
	return m_device->SetStreamSourceFreq(StreamNumber, Setting);
}

STDMETHODIMP WrapperDirect3DDevice9::GetStreamSourceFreq(THIS_ UINT StreamNumber,UINT* pSetting) {return m_device->GetStreamSourceFreq(StreamNumber, pSetting);}

/*
	place for SetIndices
*/

STDMETHODIMP WrapperDirect3DDevice9::GetIndices(THIS_ IDirect3DIndexBuffer9** ppIndexData) {
	Log::log("WrapperDirect3DDevice9::GetIndices() TODO\n");

	LPDIRECT3DINDEXBUFFER9 base_indexed_buffer = NULL;
	HRESULT hr =  m_device->GetIndices(&base_indexed_buffer);

	*ppIndexData = WrapperDirect3DIndexBuffer9::GetWrapperIndexedBuffer9(base_indexed_buffer);

	return hr;
}

STDMETHODIMP WrapperDirect3DDevice9::CreatePixelShader(THIS_ CONST DWORD* pFunction,IDirect3DPixelShader9** ppShader) {
	Log::log("WrapperDirect3DDevice9::CreatePixelShader() called\n");

	cs.begin_command(CreatePixelShader_Opcode, id);
	int cnt = 0;
	while(true) {
		if(pFunction[cnt++] == D3DPS_END()) break;
		//cnt++;
	}
	cs.write_int(WrapperDirect3DPixelShader9::ins_count);
	cs.write_int(cnt);
	cs.write_byte_arr((char*)pFunction, sizeof(DWORD) * (cnt));
	

	WrapperDirect3DPixelShader9::ins_count++;
	LPDIRECT3DPIXELSHADER9 base_ps = NULL;
	HRESULT hr = m_device->CreatePixelShader(pFunction, &base_ps);

	if(SUCCEEDED(hr)) {
		*ppShader = dynamic_cast<IDirect3DPixelShader9*>(new WrapperDirect3DPixelShader9(base_ps, WrapperDirect3DPixelShader9::ins_count - 1));
	}
	else {
		Log::log("WrapperDirect3DDevice9::CreatePixelShader() failed\n");
	}
	cs.end_command();
	return hr;
}

STDMETHODIMP WrapperDirect3DDevice9::SetPixelShader(THIS_ IDirect3DPixelShader9* pShader) {
	Log::log("WrapperDirect3DDevice9::SetPixelShader() called\n");

	if(pShader == NULL) {
		Log::log("WrapperDirect3DDevice9::SetPixelShader(), pShader is NULL, I got it\n");
		cs.begin_command(SetPixelShader_Opcode, id);
		cs.write_int(-1);
		cs.end_command();
		return m_device->SetPixelShader(pShader);
	}

	cs.begin_command(SetPixelShader_Opcode, id);
	cs.write_int(((WrapperDirect3DPixelShader9*)pShader)->GetID());
	cs.end_command();

	return m_device->SetPixelShader(((WrapperDirect3DPixelShader9*)pShader)->GetPS9());
}

STDMETHODIMP WrapperDirect3DDevice9::GetPixelShader(THIS_ IDirect3DPixelShader9** ppShader) {
	Log::log("WrapperDirect3DDevice9::GetPixelShader() TODO\n");
	LPDIRECT3DPIXELSHADER9 base_pixel_shader = NULL;
	HRESULT hr = m_device->GetPixelShader(&base_pixel_shader);

	*ppShader = WrapperDirect3DPixelShader9::GetWrapperPixelShader(base_pixel_shader);
	return hr;
}

STDMETHODIMP WrapperDirect3DDevice9::SetPixelShaderConstantF(THIS_ UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount) {
	Log::log("WrapperDirect3DDevice9::SetPixelShaderConstantF() called\n");


	cs.begin_command(SetPixelShaderConstantF_Opcode, id);
	cs.write_uint(StartRegister);
	cs.write_uint(Vector4fCount);

	
	memcpy((char*)vs_data, (char*)pConstantData, Vector4fCount * 16);

	for(int i=0; i<Vector4fCount; ++i) {
		//Log::log("%f %f %f %f\n", *(p), *(p + 1), *(p + 2), *(p + 3));

		cs.write_vec(SetPixelShaderConstantF_Opcode, vs_data + (i * 4));
	}
	

	//cs.write_byte_arr((char*)pConstantData, sizeof(float) * (Vector4fCount * 4));
	cs.end_command();

	static int total_len = 0;
	//total_len += buf_size;
	HRESULT hh = m_device->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
	Log::log("WrapperDirect3DDevice9::SetPixelShaderConstantF(), total_len=%d\n", total_len);
	
	return hh;
}

STDMETHODIMP WrapperDirect3DDevice9::GetPixelShaderConstantF(THIS_ UINT StartRegister,float* pConstantData,UINT Vector4fCount) {return m_device->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);}

STDMETHODIMP WrapperDirect3DDevice9::SetPixelShaderConstantI(THIS_ UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount) {
	Log::log("WrapperDirect3DDevice9::SetPixelShaderConstantI() called\n");

	cs.begin_command(SetPixelShaderConstantI_Opcode, id);
	cs.write_uint(StartRegister);
	cs.write_uint(Vector4iCount);
	cs.write_byte_arr((char*)pConstantData, sizeof(int) * (Vector4iCount * 4));
	
	HRESULT hh = m_device->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
	cs.end_command();
	return hh;
}

STDMETHODIMP WrapperDirect3DDevice9::GetPixelShaderConstantI(THIS_ UINT StartRegister,int* pConstantData,UINT Vector4iCount) {return m_device->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);}

STDMETHODIMP WrapperDirect3DDevice9::SetPixelShaderConstantB(THIS_ UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount) {
	Log::log("WrapperDirect3DDevice9::SetPixelShaderConstantB() called\n");

	cs.begin_command(SetPixelShaderConstantB_Opcode, id);
	cs.write_uint(StartRegister);
	cs.write_uint(BoolCount);
	cs.write_byte_arr((char*)pConstantData, sizeof(BOOL) * BoolCount);
	
	
	HRESULT hh = m_device->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
	cs.end_command();
	return hh;
}

STDMETHODIMP WrapperDirect3DDevice9::GetPixelShaderConstantB(THIS_ UINT StartRegister,BOOL* pConstantData,UINT BoolCount) {return m_device->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);}

STDMETHODIMP WrapperDirect3DDevice9::DrawRectPatch(THIS_ UINT Handle,CONST float* pNumSegs,CONST D3DRECTPATCH_INFO* pRectPatchInfo) {
	Log::log("WrapperDirect3DDevice9::DrawRectPatch() TODO\n");
	return m_device->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
}

STDMETHODIMP WrapperDirect3DDevice9::DrawTriPatch(THIS_ UINT Handle,CONST float* pNumSegs,CONST D3DTRIPATCH_INFO* pTriPatchInfo) {
	Log::log("WrapperDirect3DDevice9::DrawTriPatch() TODO\n");
	return m_device->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
}

STDMETHODIMP WrapperDirect3DDevice9::DeletePatch(THIS_ UINT Handle) {
	Log::log("WrapperDirect3DDevice9::DeletePatch() TODO\n");
	return m_device->DeletePatch(Handle);
}

STDMETHODIMP WrapperDirect3DDevice9::CreateQuery(THIS_ D3DQUERYTYPE Type,IDirect3DQuery9** ppQuery) {
	Log::log("WrapperDirect3DDevice9::CreateQuery() TODO\n");
	return m_device->CreateQuery(Type, ppQuery);
}
