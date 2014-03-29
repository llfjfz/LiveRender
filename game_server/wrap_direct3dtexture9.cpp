#include "wrap_direct3dtexture9.h"
#include "wrap_direct3dsurface9.h"
#include "opcode.h"

//#define SAVE_IMG
//#define LOCAL_IMG

int D3DXTexture = 0;
WrapperDirect3DTexture9::WrapperDirect3DTexture9(IDirect3DTexture9* ptr, int _id): m_tex(ptr), id(_id) {
	Log::log("WrapperDirect3DTexture9::WrapperDirect3DTexture9(), id=%d, base_tex=%d this=%d\n", id, ptr, this);
	if(ptr == NULL) {
		Log::log("WrapperDirect3DTexture9::WrapperDirect3DTexture9(), base_tex is NULL\n");
	}
	m_list.AddMember(ptr, this);
}

void WrapperDirect3DTexture9::SayHi(char* str) {
	Log::log("%s\n", str);
}

void WrapperDirect3DTexture9::SetID(int id) {
	this->id = id;
}

int WrapperDirect3DTexture9::GetID() {
	return this->id;
}

IDirect3DTexture9* WrapperDirect3DTexture9::GetTex9() {
	return this->m_tex;
}

WrapperDirect3DTexture9* WrapperDirect3DTexture9::GetWrapperTexture9(IDirect3DTexture9* ptr) {
	//Log::slog("WrapperDirect3DTexture9::GetWrapperTexture9(), ptr=%u\n", ptr);
	WrapperDirect3DTexture9* ret = (WrapperDirect3DTexture9*)( m_list.GetDataPtr(ptr) );
	if(ret == NULL) {
		Log::log("WrapperDirect3DTexture9::GetWrapperTexture9(), ret is NULL\n");
	}
	return ret;
}

HRESULT WrapperDirect3DTexture9::SendTextureData() {
	Log::log("WrapperDirect3DTexture9::SendTextureData(), id=%d, threadid=%d\n", this->id, GetCurrentThreadId());

	Log::log("trying to call GetLevelCount()\n");
	int levelCount = m_tex->GetLevelCount();

	Log::log("WrapperDirect3DTexture9::SendTextureData(), id=%d, LevelCount=%d\n", this->id, levelCount);

#ifdef LOCAL_IMG
	for(int level=0; level<levelCount; ++level) {
	//int level = 0;
		LPDIRECT3DSURFACE9 top_surface = NULL;
		m_tex->GetSurfaceLevel(level, &top_surface);

		D3DLOCKED_RECT rect;
		Log::log("WrapperDirect3DTexture9::SendTextureData(), start lock %d level\n", level);
		//top_surface->
		top_surface->LockRect(&rect, 0, 0);

		cs.begin_command(TransmitTextureData_Opcode, id);
		cs.write_int(level);
		cs.end_command();
	
		//Log::log("WrapperDirect3DTexture9::SendTextureData(), id=%d, height=%d, width=%d, pitch=%d, size=%d\n", this->id, desc.Height, desc.Width, rect.Pitch,  desc.Height * desc.Width * byte_per_pixel);
		char  fname[50];
		sprintf(fname,"surface\\face_%d_leve_%d.png",this->id,level);
		
		top_surface->UnlockRect();

		Log::log("WrapperDirect3DTexture9::SendTextureData(), quit %d level\n", level);
	}
#else

	
	for(int level=0; level<levelCount; ++level) {
	//int level = 0;
		cs.begin_command(TransmitTextureData_Opcode, id);
		LPDIRECT3DSURFACE9 top_surface = NULL;
		m_tex->GetSurfaceLevel(level, &top_surface);

		D3DLOCKED_RECT rect;
		Log::log("WrapperDirect3DTexture9::SendTextureData(), start lock %d level\n", level);
		//top_surface->
		// here lock failed when debug d3d, the flag and texture creation may error
		

		
		//cs.write_int(levelCount);
		cs.write_int(level);
		
		//Log::log("WrapperDirect3DTexture9::SendTextureData(), id=%d, height=%d, width=%d, pitch=%d, size=%d\n", this->id, desc.Height, desc.Width, rect.Pitch,  desc.Height * desc.Width * byte_per_pixel);
		//WRITE_DATA(int, desc.Height * desc.Width * byte_per_pixel);
#ifdef MEM_FILE_TEX

		top_surface->LockRect(&rect, 0, D3DLOCK_READONLY);

		D3DSURFACE_DESC desc;
		top_surface->GetDesc(&desc);

		int byte_per_pixel = rect.Pitch / desc.Width;

		cs.write_uint(rect.Pitch);
		cs.write_int(desc.Height * desc.Width * byte_per_pixel);
		cs.write_byte_arr((char *)(rect.pBits), desc.Height * desc.Width * byte_per_pixel);

		top_surface->UnlockRect();
#else

		
		//char* surface_data = (char*)(rect.pBits);
		double tick_s= 0.0;
		tick_s = GetTickCount();
		
		LPD3DXBUFFER DestBuf = NULL;
		HRESULT hhr = D3DXSaveSurfaceToFileInMemory(&DestBuf,D3DXIFF_PNG,top_surface,NULL,NULL);
		double tick_e = GetTickCount();
		int size = DestBuf->GetBufferSize();
		cs.write_int(size);
		cs.write_byte_arr((char*)(DestBuf->GetBufferPointer()), size);
		char  fname[50];

		Log::log("send surface id:%d size:%d\n",this->id, size);
		sprintf(fname,"surface\\face_%d_leve_%d.png",this->id,level);
		if(DestBuf){
			DestBuf->Release();
			DestBuf = NULL;
		}
		double tick_a = GetTickCount();
		Log::log("\tSave to memo:%f, write to buffer:%f\n", tick_e -tick_s, tick_a - tick_e);
#endif
		cs.end_command();
		
		
#ifdef SAVE_IMG
		D3DXSaveSurfaceToFile(fname,D3DXIFF_PNG,top_surface,NULL,NULL);
#endif
		//D3DXSaveSurfaceToFile(fname, D3DXIFF_JPG, top_surface,NULL,NULL);
		Log::log("WrapperDirect3DTexture9::SendTextureData(), quit %d level\n", level);
	}
#endif
	return D3D_OK;
}

/*** IUnknown methods ***/
STDMETHODIMP WrapperDirect3DTexture9::QueryInterface(THIS_ REFIID riid, void** ppvObj) {
	Log::log("WrapperDirect3DTexture9::QueryInterface(), ppvObj=%d\n", *ppvObj);
	HRESULT hr = m_tex->QueryInterface(riid, ppvObj);
	*ppvObj = this;
	Log::log("WrapperDirect3DTexture9::QueryInterface() end, ppvObj=%d\n", *ppvObj);
	return hr;
}
STDMETHODIMP_(ULONG) WrapperDirect3DTexture9::AddRef(THIS) {
	Log::log("WrapperDirect3DTexture9::AddRef() called\n");
	return m_tex->AddRef();
}
STDMETHODIMP_(ULONG) WrapperDirect3DTexture9::Release(THIS) {
	Log::log("WrapperDirect3DTexture9::Release(), id=%d, threadid=%d\n", this->id, GetCurrentThreadId());
	HRESULT hr = m_tex->Release();
	Log::log("WrapperDirect3DTexture9::Release() succeeded\n");
	return hr;
	//return D3D_OK;
}

/*** IDirect3DBaseTexture9 methods ***/
STDMETHODIMP WrapperDirect3DTexture9::GetDevice(THIS_ IDirect3DDevice9** ppDevice) {
	Log::log("WrapperDirect3DTexture9::GetDevice() TODO\n");
	return m_tex->GetDevice(ppDevice);
}
STDMETHODIMP WrapperDirect3DTexture9::SetPrivateData(THIS_ REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags) {
	Log::log("WrapperDirect3DTexture9::SetPrivateData() TODO\n");
	return m_tex->SetPrivateData(refguid, pData, SizeOfData, Flags);
}

STDMETHODIMP WrapperDirect3DTexture9::GetPrivateData(THIS_ REFGUID refguid,void* pData,DWORD* pSizeOfData) {
	Log::log("WrapperDirect3DTexture9::GetPrivateData() TODO\n");
	return m_tex->GetPrivateData(refguid, pData, pSizeOfData);
}
STDMETHODIMP WrapperDirect3DTexture9::FreePrivateData(THIS_ REFGUID refguid) {
	Log::log("WrapperDirect3DTexture9::FreePrivateData() TODO\n");
	return m_tex->FreePrivateData(refguid);
}
STDMETHODIMP_(DWORD) WrapperDirect3DTexture9::SetPriority(THIS_ DWORD PriorityNew) {
	Log::log("WrapperDirect3DTexture9::SetPriority() TODO\n");
	return m_tex->SetPriority(PriorityNew);
}

STDMETHODIMP_(DWORD) WrapperDirect3DTexture9::GetPriority(THIS) {
	Log::log("WrapperDirect3DTexture9::GetPriority() called\n");
	return m_tex->GetPriority();
}

STDMETHODIMP_(void) WrapperDirect3DTexture9::PreLoad(THIS) {
	Log::log("WrapperDirect3DTexture9::PreLoad() TODO\n");
	return m_tex->PreLoad();
}

STDMETHODIMP_(D3DRESOURCETYPE) WrapperDirect3DTexture9::GetType(THIS) {
	//Log::log("WrapperDirect3DTexture9::GetType() TODO\n");
	return m_tex->GetType();
}

STDMETHODIMP_(DWORD) WrapperDirect3DTexture9::SetLOD(THIS_ DWORD LODNew) {
	Log::log("WrapperDirect3DTexture9::SetLOD() TODO\n");
	return m_tex->SetLOD(LODNew);
}

STDMETHODIMP_(DWORD) WrapperDirect3DTexture9::GetLOD(THIS) {
	Log::log("WrapperDirect3DTexture9::GetLOD() called\n");
	return m_tex->GetLOD();
}

STDMETHODIMP_(DWORD) WrapperDirect3DTexture9::GetLevelCount(THIS) {
	Log::log("WrapperDirect3DTexture9::GetLevelCount() called\n");
	return m_tex->GetLevelCount();
}

STDMETHODIMP WrapperDirect3DTexture9::SetAutoGenFilterType(THIS_ D3DTEXTUREFILTERTYPE FilterType) {
	Log::log("WrapperDirect3DTexture9::SetAutoGenFilterType() called\n");
	/*
	GET_BUFFER(TextureSetAutoGenFilterType_Opcode, id);
	WRITE_DATA(D3DTEXTUREFILTERTYPE, FilterType);
	END_BUFFER();
	client.SendPacket(msg, buf_size);
	*/
	cs.begin_command(TextureSetAutoGenFilterType_Opcode, id);
	cs.write_uint(FilterType);
	cs.end_command();

	return m_tex->SetAutoGenFilterType(FilterType);
}

STDMETHODIMP_(D3DTEXTUREFILTERTYPE) WrapperDirect3DTexture9::GetAutoGenFilterType(THIS) {
	Log::log("WrapperDirect3DTexture9::GetAutoGenFilterType() TODO\n");
	return m_tex->GetAutoGenFilterType();
}

STDMETHODIMP_(void) WrapperDirect3DTexture9::GenerateMipSubLevels(THIS) {
	Log::log("WrapperDirect3DTexture9::GenerateMipSubLevels() called\n");
	/*
	GET_BUFFER(TextureGenerateMipSubLevels_Opcode, id);
	END_BUFFER();
	client.SendPacket(msg, buf_size);
	*/
	cs.begin_command(TextureGenerateMipSubLevels_Opcode, id);
	cs.end_command();

	return m_tex->GenerateMipSubLevels();
}

STDMETHODIMP WrapperDirect3DTexture9::GetLevelDesc(THIS_ UINT Level,D3DSURFACE_DESC *pDesc) {
	Log::log("WrapperDirect3DTexture9::GetLevelDesc() called\n");
	return m_tex->GetLevelDesc(Level, pDesc);
}

STDMETHODIMP WrapperDirect3DTexture9::GetSurfaceLevel(THIS_ UINT Level,IDirect3DSurface9** ppSurfaceLevel) {
	Log::log("WrapperDirect3DTexture9::GetSurfaceLevel() called, Level:%d\n",Level);
	IDirect3DSurface9* base_surface = NULL;
	HRESULT hr = m_tex->GetSurfaceLevel(Level, &base_surface);//ppSurfaceLevel);

	WrapperDirect3DSurface9* surface = WrapperDirect3DSurface9::GetWrapperSurface9(base_surface);
	if(surface == NULL) {

		/*
		GET_BUFFER(TextureGetSurfaceLevel_Opcode, id);
		WRITE_DATA(int, WrapperDirect3DSurface9::ins_count);
		WRITE_DATA(UINT, Level);
		END_BUFFER();
		client.SendPacket(msg, buf_size);
		*/
		cs.begin_command(TextureGetSurfaceLevel_Opcode, id);
		cs.write_int(this->id);
		cs.write_int(WrapperDirect3DSurface9::ins_count);
		cs.write_uint(Level);
		cs.end_command();


		surface = new WrapperDirect3DSurface9(base_surface, WrapperDirect3DSurface9::ins_count++);
		surface->SetTexId(this->id);
		surface->SetLevel(Level);
	}
	*ppSurfaceLevel = surface;
	Log::log("WrapperDirect3DTexture9::GetSurfaceLevel(), base_surface=%d, ppSurfaceLevel=%d\n", base_surface, *ppSurfaceLevel);
	return hr;
}

STDMETHODIMP WrapperDirect3DTexture9::LockRect(THIS_ UINT Level,D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags) {
	if(pRect == NULL) {
		Log::log("WrapperDirect3DTexture9::LockRect(), pRect is NULL!\n");
	}
	Log::log("WrapperDirect3DTexture9::LockRect() called\n");
	tex_send[this->id] = false;
	//Log::log("WrapperDirect3DTexture9::LockRect(), Level=%d, left=%d, right=%d, bottom=%d, top=%d\n", Level, pRect->left, pRect->right, pRect->bottom, pRect->top);

	return m_tex->LockRect(Level, pLockedRect, pRect, Flags);
}

STDMETHODIMP WrapperDirect3DTexture9::UnlockRect(THIS_ UINT Level) {
	Log::log("WrapperDirect3DTexture9::UnlockRect(), Level=%d\n", Level);

	//this->SendTextureData();
	//tex_send[this->id] = true;

	HRESULT hr = m_tex->UnlockRect(Level);

	//tex_send[this->id] = true;
	/****************************this->SendTextureData();*/

	/*
	GET_BUFFER_2(TextureUnlock_Opcode, id);

	D3DSURFACE_DESC desc;
	m_tex->GetLevelDesc(Level, &desc);

	D3DLOCKED_RECT rect;
	m_tex->LockRect(Level, &rect, 0, 0);
	WRITE_DATA(int, Level);
	WRITE_DATA(int, rect.Pitch);
	WRITE_DATA(int, desc.Height * desc.Width * 4);
	WRITE_BYTE_ARR(desc.Height * desc.Width * 4, rect.pBits);
	END_BUFFER();
	client.SendPacket(msg, buf_size);
	m_tex->UnlockRect(Level);
	*/
	return hr;
}

STDMETHODIMP WrapperDirect3DTexture9::AddDirtyRect(THIS_ CONST RECT* pDirtyRect) {
	Log::log("WrapperDirect3DTexture9::AddDirtyRect() TODO\n");
	return m_tex->AddDirtyRect(pDirtyRect);
}


