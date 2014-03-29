#include "wrap_direct3dcubetexture9.h"
#include "wrap_direct3dsurface9.h"
#include "wrap_direct3ddevice9.h"
#include "opcode.h"

WrapperDirect3DCubeTexture9::WrapperDirect3DCubeTexture9(IDirect3DCubeTexture9* ptr, int _id): m_cube_tex(ptr), id(_id) {
	Log::log("WrapperDirect3DCubeTexture9::WrapperDirect3DCubeTexture9() called\n");

	m_list.AddMember(ptr, this);
}

IDirect3DCubeTexture9* WrapperDirect3DCubeTexture9::GetCubeTex9() {
	return this->m_cube_tex;
}

int WrapperDirect3DCubeTexture9::GetID() {
	return this->id;
}

void WrapperDirect3DCubeTexture9::SetID(int id) {
	this->id = id;
}

WrapperDirect3DCubeTexture9* WrapperDirect3DCubeTexture9::GetWrapperCubeTexture9(IDirect3DCubeTexture9* ptr) {
	Log::log("WrapperDirect3DCubeTexture9::GetWrapperCubeTexture9() called\n");
	WrapperDirect3DCubeTexture9* ret = (WrapperDirect3DCubeTexture9*)( m_list.GetDataPtr(ptr) );
	if(ret == NULL) {
		Log::log("WrapperDirect3DCubeTexture9::GetWrapperCubeTexture9(), ret is NULL\n");
	}
	return ret;
}

/*** IUnknown methods ***/
STDMETHODIMP WrapperDirect3DCubeTexture9::QueryInterface(THIS_ REFIID riid, void** ppvObj) {
	Log::log("WrapperDirect3DCubeTexture9::QueryInterface(), ppvObj=%d\n", *ppvObj);
	HRESULT hr = m_cube_tex->QueryInterface(riid, ppvObj);
	*ppvObj = this;
	Log::log("WrapperDirect3DCubeTexture9::QueryInterface() end, ppvObj=%d\n", *ppvObj);
	return hr;
}
STDMETHODIMP_(ULONG) WrapperDirect3DCubeTexture9::AddRef(THIS) {
	Log::log("WrapperDirect3DCubeTexture9::AddRef() called\n");
	return m_cube_tex->AddRef();
}
STDMETHODIMP_(ULONG) WrapperDirect3DCubeTexture9::Release(THIS) {
	Log::log("WrapperDirect3DCubeTexture9::Release(), id=%d, threadid=%d\n", this->id, GetCurrentThreadId());
	HRESULT hr = m_cube_tex->Release();
	Log::log("WrapperDirect3DCubeTexture9::Release() succeeded\n");
	return hr;
	//return D3D_OK;
}

/*** IDirect3DBaseTexture9 methods ***/
STDMETHODIMP WrapperDirect3DCubeTexture9::GetDevice(THIS_ IDirect3DDevice9** ppDevice) {
	Log::log("WrapperDirect3DCubeTexture9::GetDevice() TODO\n");
	//return m_cube_tex->GetDevice(ppDevice);
	IDirect3DDevice9* base = NULL;
	HRESULT hr = this->m_cube_tex->GetDevice(&base);
	WrapperDirect3DDevice9 * ret = WrapperDirect3DDevice9::GetWrapperDevice9(base);
	if(ret == NULL){
		Log::log("WrapperDirect3DCubeTexture9::GetDevice() return NULL\n");
	}
	else{

	}
	//*ppDevice = ret;
	*ppDevice = base;
	return hr;
}
STDMETHODIMP WrapperDirect3DCubeTexture9::SetPrivateData(THIS_ REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags) {
	Log::log("WrapperDirect3DCubeTexture9::SetPrivateData() TODO\n");
	return m_cube_tex->SetPrivateData(refguid, pData, SizeOfData, Flags);
}

STDMETHODIMP WrapperDirect3DCubeTexture9::GetPrivateData(THIS_ REFGUID refguid,void* pData,DWORD* pSizeOfData) {
	Log::log("WrapperDirect3DCubeTexture9::GetPrivateData() TODO\n");
	return m_cube_tex->GetPrivateData(refguid, pData, pSizeOfData);
}
STDMETHODIMP WrapperDirect3DCubeTexture9::FreePrivateData(THIS_ REFGUID refguid) {
	Log::log("WrapperDirect3DCubeTexture9::FreePrivateData() TODO\n");
	return m_cube_tex->FreePrivateData(refguid);
}
STDMETHODIMP_(DWORD) WrapperDirect3DCubeTexture9::SetPriority(THIS_ DWORD PriorityNew) {
	Log::log("WrapperDirect3DCubeTexture9::SetPriority() TODO\n");
	return m_cube_tex->SetPriority(PriorityNew);
}

STDMETHODIMP_(DWORD) WrapperDirect3DCubeTexture9::GetPriority(THIS) {
	Log::log("WrapperDirect3DCubeTexture9::GetPriority() called\n");
	return m_cube_tex->GetPriority();
}

STDMETHODIMP_(void) WrapperDirect3DCubeTexture9::PreLoad(THIS) {
	Log::log("WrapperDirect3DCubeTexture9::PreLoad() TODO\n");
	return m_cube_tex->PreLoad();
}

STDMETHODIMP_(D3DRESOURCETYPE) WrapperDirect3DCubeTexture9::GetType(THIS) {
	Log::log("WrapperDirect3DCubeTexture9::GetType() called\n");
	return m_cube_tex->GetType();
}

STDMETHODIMP_(DWORD) WrapperDirect3DCubeTexture9::SetLOD(THIS_ DWORD LODNew) {
	Log::log("WrapperDirect3DCubeTexture9::SetLOD() TODO\n");
	return m_cube_tex->SetLOD(LODNew);
}

STDMETHODIMP_(DWORD) WrapperDirect3DCubeTexture9::GetLOD(THIS) {
	Log::log("WrapperDirect3DCubeTexture9::GetLOD() called\n");
	return m_cube_tex->GetLOD();
}

STDMETHODIMP_(DWORD) WrapperDirect3DCubeTexture9::GetLevelCount(THIS) {
	Log::log("WrapperDirect3DCubeTexture9::GetLevelCount() TODO\n");
	return m_cube_tex->GetLevelCount();
}

STDMETHODIMP WrapperDirect3DCubeTexture9::SetAutoGenFilterType(THIS_ D3DTEXTUREFILTERTYPE FilterType) {
	Log::log("WrapperDirect3DCubeTexture9::SetAutoGenFilterType() TODO\n");
	return m_cube_tex->SetAutoGenFilterType(FilterType);
}

STDMETHODIMP_(D3DTEXTUREFILTERTYPE) WrapperDirect3DCubeTexture9::GetAutoGenFilterType(THIS) {
	Log::log("WrapperDirect3DCubeTexture9::GetAutoGenFilterType() called\n");
	return m_cube_tex->GetAutoGenFilterType();
}

STDMETHODIMP_(void) WrapperDirect3DCubeTexture9::GenerateMipSubLevels(THIS) {
	Log::log("WrapperDirect3DCubeTexture9::GenerateMipSubLevels() TODO\n");
	return m_cube_tex->GenerateMipSubLevels();
}

STDMETHODIMP WrapperDirect3DCubeTexture9::GetLevelDesc(THIS_ UINT Level,D3DSURFACE_DESC *pDesc) {
	Log::log("WrapperDirect3DCubeTexture9::GetLevelDesc() TODO\n");
	return m_cube_tex->GetLevelDesc(Level, pDesc);
}

STDMETHODIMP WrapperDirect3DCubeTexture9::GetCubeMapSurface(THIS_ D3DCUBEMAP_FACES FaceType,UINT Level,IDirect3DSurface9** ppCubeMapSurface) {
	Log::log("WrapperDirect3DCubeTexture9::GetCubeMapSurface() called\n");
	IDirect3DSurface9* base_surface = NULL;
	HRESULT hr = m_cube_tex->GetCubeMapSurface(FaceType, Level, &base_surface);

	WrapperDirect3DSurface9* surface = WrapperDirect3DSurface9::GetWrapperSurface9(base_surface);
	if(surface == NULL) {
		/*
		GET_BUFFER(CubeGetCubeMapSurface_Opcode, id);
		WRITE_DATA(int, WrapperDirect3DSurface9::ins_count);
		WRITE_DATA(D3DCUBEMAP_FACES, FaceType);
		WRITE_DATA(UINT, Level);
		END_BUFFER();
		client.SendPacket(msg, buf_size);
		*/
		cs.begin_command(CubeGetCubeMapSurface_Opcode, id);
		cs.write_int(WrapperDirect3DSurface9::ins_count);
		cs.write_uint(FaceType);
		cs.write_uint(Level);
		cs.end_command();

		surface = new WrapperDirect3DSurface9(base_surface, WrapperDirect3DSurface9::ins_count++);
	}

	*ppCubeMapSurface = surface;

	return hr;
}

STDMETHODIMP WrapperDirect3DCubeTexture9::LockRect(THIS_ D3DCUBEMAP_FACES FaceType,UINT Level,D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags) {
	if(pRect == NULL) {
		Log::log("WrapperDirect3DCubeTexture9::LockRect(), pRect is NULL!\n");
	}
	Log::log("WrapperDirect3DCubeTexture9::LockRect() called\n");
	return m_cube_tex->LockRect(FaceType, Level, pLockedRect, pRect, Flags);
}

STDMETHODIMP WrapperDirect3DCubeTexture9::UnlockRect(THIS_ D3DCUBEMAP_FACES FaceType,UINT Level) {
	Log::log("WrapperDirect3DCubeTexture9::UnlockRect(), Level=%d\n", Level);

	//this->SendTextureData();
	//tex_send[this->id] = true;

	return m_cube_tex->UnlockRect(FaceType, Level);
}

STDMETHODIMP WrapperDirect3DCubeTexture9::AddDirtyRect(THIS_ D3DCUBEMAP_FACES FaceType,CONST RECT* pDirtyRect) {
	Log::log("WrapperDirect3DCubeTexture9::AddDirtyRect() TODO\n");
	
	return m_cube_tex->AddDirtyRect(FaceType, pDirtyRect);
}
