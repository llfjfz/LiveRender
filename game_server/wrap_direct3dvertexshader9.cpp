#include "game_server.h"
#include "wrap_direct3dvertexshader9.h"

WrapperDirect3DVertexShader9::WrapperDirect3DVertexShader9(IDirect3DVertexShader9* ptr, int _id): m_vs(ptr), id(_id) {
	Log::log("WrapperDirect3DVertexShader9 constructor called!\n");
	m_list.AddMember(ptr, this);
}

int WrapperDirect3DVertexShader9::GetID() {
	return this->id;
}

void WrapperDirect3DVertexShader9::SetID(int id) {
	this->id = id;
}

IDirect3DVertexShader9* WrapperDirect3DVertexShader9::GetVS9() {
	return m_vs;
}


WrapperDirect3DVertexShader9* WrapperDirect3DVertexShader9::GetWrapperVertexShader(IDirect3DVertexShader9* ptr) {
	WrapperDirect3DVertexShader9* ret = (WrapperDirect3DVertexShader9*)( m_list.GetDataPtr(ptr) );
	if(ret == NULL) {
		Log::log("WrapperDirect3DPixelShader9::GetWrapperPixelShader(), ret is NULL\n");
	}
	return ret;
}

/*** IUnknown methods ***/
STDMETHODIMP WrapperDirect3DVertexShader9::QueryInterface(THIS_ REFIID riid, void** ppvObj) {
	HRESULT hr = m_vs->QueryInterface(riid, ppvObj);
	*ppvObj = this;
	return hr;
}
STDMETHODIMP_(ULONG) WrapperDirect3DVertexShader9::AddRef(THIS) {
	return m_vs->AddRef();
}
STDMETHODIMP_(ULONG) WrapperDirect3DVertexShader9::Release(THIS) {
	Log::log("WrapperDirect3DVertexShader9::Release() called! id:%d\n", id);
	return m_vs->Release();
}

/*** IDirect3DVertexShader9 methods ***/
STDMETHODIMP WrapperDirect3DVertexShader9::GetDevice(THIS_ IDirect3DDevice9** ppDevice) {
	Log::log("WrapperDirect3DVertexShader9::GetDevice called!\n");
	return m_vs->GetDevice(ppDevice);
}
STDMETHODIMP WrapperDirect3DVertexShader9::GetFunction(THIS_ void* ptr,UINT* pSizeOfData) {
	return m_vs->GetFunction(ptr, pSizeOfData);
}

