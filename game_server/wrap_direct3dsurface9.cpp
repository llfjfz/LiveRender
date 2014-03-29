#include "wrap_direct3dsurface9.h"
#include "wrap_direct3ddevice9.h"
#include "opcode.h"
void WrapperDirect3DSurface9::RepalceSurface(IDirect3DSurface9* pnew){
	/*this->m_surface->Release();
	this->m_surface = pnew;*/
}
int WrapperDirect3DSurface9::GetTexId(){
	return this->tex_id;
}
int WrapperDirect3DSurface9::GetLevel(){
	return this->level;
}
void WrapperDirect3DSurface9::SendSurface(){
	//if(isSent == false){
		// send data to client
		D3DLOCKED_RECT rect;
		D3DSURFACE_DESC desc;
		this->LockRect(&rect, 0, 0);
		this->GetDesc(&desc);
		int byte_per_pixel = rect.Pitch / desc.Width;

		
		

		/*WRITE_DATA(DWORD, rect.Pitch);
		WRITE_DATA(int, desc.Height * desc.Width * byte_per_pixel);
		Log::log("WrapperDirect3DTexture9::SendSurface(), id=%d, height=%d, width=%d, pitch=%d, size=%d\n", this->id, desc.Height, desc.Width, rect.Pitch,  desc.Height * desc.Width * byte_per_pixel);

		WRITE_BYTE_ARR(desc.Height * desc.Width * byte_per_pixel, rect.pBits);*/
		LPD3DXBUFFER tbuf = NULL;
		D3DXSaveSurfaceToFileInMemory(&tbuf,D3DXIFF_PNG,this,NULL,NULL);
		int size = tbuf->GetBufferSize();
		/*
		GET_BUFFER(TransmitSurfaceData_Opcode, id);
		WRITE_DATA(int, size);
		WRITE_BYTE_ARR(size, tbuf->GetBufferPointer());

		END_BUFFER();
		//client.SendPacket(msg, buf_size);
		*/
		char  fname[50];
		sprintf(fname,"surface\\face_%d.png",this->id);
		D3DXSaveSurfaceToFile(fname, D3DXIFF_PNG,this, NULL, NULL);
		isSent = true;
	//}
	//else{
		// the data has been sent to client
	//}
}
WrapperDirect3DSurface9::WrapperDirect3DSurface9(IDirect3DSurface9* ptr, int _id): m_surface(ptr), id(_id) {

	this->tex_id = -1;
	this->level = -1;
	Log::log("WrapperDirect3DSurface9 constructor called! ptr:%d, id:%d, this:%d\n",ptr, id,this);
	m_list.AddMember(ptr, this);
	this->isSent = false;
}

int WrapperDirect3DSurface9::GetID() {
	return this->id;
}

void WrapperDirect3DSurface9::SetID(int id) {
	this->id = id;
}

WrapperDirect3DSurface9* WrapperDirect3DSurface9::GetWrapperSurface9(IDirect3DSurface9* ptr) {
	//Log::slog("WrapperDirect3DSurface9::GetWrapperSurface9(), ptr=%u\n", ptr);
	WrapperDirect3DSurface9* ret = (WrapperDirect3DSurface9*)(m_list.GetDataPtr(ptr));
	if(ret == NULL) {
		Log::log("WrapperDirect3DSurface9::GetWrapperSurface9(), ret is NULL, IDSurface:%d, ins_count:%d\n",ptr,WrapperDirect3DSurface9::ins_count);
		// add the following line , 2013/7/23 21:12
	//	ret = new WrapperDirect3DSurface9(ptr, WrapperDirect3DSurface9::ins_count++);
	}
	return ret;
}

IDirect3DSurface9* WrapperDirect3DSurface9::GetSurface9() {
	return this->m_surface;
}

/*** IUnknown methods ***/
STDMETHODIMP WrapperDirect3DSurface9::QueryInterface(THIS_ REFIID riid, void** ppvObj) {
	Log::log("WrapperDirect3DSurface9::QueryInterface() called\n");
	HRESULT hr = m_surface->QueryInterface(riid, ppvObj);
	*ppvObj = this;
	return hr;
}

STDMETHODIMP_(ULONG) WrapperDirect3DSurface9::AddRef(THIS) {
	Log::log("WrapperDirect3DSurface9::AddRef() called\n");
	return m_surface->AddRef();
}
STDMETHODIMP_(ULONG) WrapperDirect3DSurface9::Release(THIS) {
	Log::log("WrapperDirect3DSurface9::Release() called! id:%d\n",this->id);
	return m_surface->Release();
}

/*** IDirect3DResource9 methods ***/
STDMETHODIMP WrapperDirect3DSurface9::GetDevice(THIS_ IDirect3DDevice9** ppDevice) {
	Log::log("WrapperDirect3DSurface9::GetDevice() TODO\n");
	IDirect3DDevice9* base_device = NULL;
	HRESULT hr = m_surface->GetDevice(&base_device);

	WrapperDirect3DDevice9* device = WrapperDirect3DDevice9::GetWrapperDevice9(base_device);
	*ppDevice = device;

	Log::log("WrapperDirect3DSurface9::GetDevice(), base_device=%d, device=%d\n", base_device, device);

	return hr;
}

STDMETHODIMP WrapperDirect3DSurface9::SetPrivateData(THIS_ REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags){
	Log::log("WrapperDirect3DSurface9::SetPrivateData() TODO\n");
	return m_surface->SetPrivateData(refguid, pData, SizeOfData, Flags);
}

STDMETHODIMP WrapperDirect3DSurface9::GetPrivateData(THIS_ REFGUID refguid,void* pData,DWORD* pSizeOfData) {
	Log::log("WrapperDirect3DSurface9::GetPrivateData() called\n");
	return m_surface->GetPrivateData(refguid, pData, pSizeOfData);
}

STDMETHODIMP WrapperDirect3DSurface9::FreePrivateData(THIS_ REFGUID refguid) {
	Log::log("WrapperDirect3DSurface9::FreePrivateData() called\n");
	return m_surface->FreePrivateData(refguid);
}

STDMETHODIMP_(DWORD) WrapperDirect3DSurface9::SetPriority(THIS_ DWORD PriorityNew) {
	Log::log("WrapperDirect3DSurface9::SetPriority() TODO\n");
	return m_surface->SetPriority(PriorityNew);
}

STDMETHODIMP_(DWORD) WrapperDirect3DSurface9::GetPriority(THIS) {
	Log::log("WrapperDirect3DSurface9::GetPriority() called\n");
	return m_surface->GetPriority();
}

STDMETHODIMP_(void) WrapperDirect3DSurface9::PreLoad(THIS) {
	Log::log("WrapperDirect3DSurface9::PreLoad() called\n");
	return m_surface->PreLoad();
}

STDMETHODIMP_(D3DRESOURCETYPE) WrapperDirect3DSurface9::GetType(THIS) {
	Log::log("WrapperDirect3DSurface9::GetType() called\n");
	return m_surface->GetType();
}

STDMETHODIMP WrapperDirect3DSurface9::GetContainer(THIS_ REFIID riid,void** ppContainer) {
	Log::log("WrapperDirect3DSurface9::GetContainer() TODO\n");
	return m_surface->GetContainer(riid, ppContainer);
}

STDMETHODIMP WrapperDirect3DSurface9::GetDesc(THIS_ D3DSURFACE_DESC *pDesc) {
	Log::log("WrapperDirect3DSurface9::GetDesc() called\n");
	return m_surface->GetDesc(pDesc);
}

STDMETHODIMP WrapperDirect3DSurface9::LockRect(THIS_ D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags) {
	Log::log("WrapperDirect3DSurface9::LockRect() called\n");
	return m_surface->LockRect(pLockedRect, pRect, Flags);
}

STDMETHODIMP WrapperDirect3DSurface9::UnlockRect(THIS) {
	Log::log("WrapperDirect3DSurface9::UnlockRect() called\n");
	return m_surface->UnlockRect();
}

STDMETHODIMP WrapperDirect3DSurface9::GetDC(THIS_ HDC *phdc) {
	Log::log("WrapperDirect3DSurface9::GetDC() called\n");
	return m_surface->GetDC(phdc);
}

STDMETHODIMP WrapperDirect3DSurface9::ReleaseDC(THIS_ HDC hdc) {
	Log::log("WrapperDirect3DSurface9::ReleaseDC() called\n");
	return m_surface->ReleaseDC(hdc);
}
