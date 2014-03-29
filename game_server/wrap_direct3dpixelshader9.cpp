#include "wrap_direct3dpixelshader9.h"

WrapperDirect3DPixelShader9::WrapperDirect3DPixelShader9(IDirect3DPixelShader9* ptr, int _id): m_ps(ptr), id(_id) {

	m_list.AddMember(ptr, this);
}

void WrapperDirect3DPixelShader9::SetID(int id) {
	this->id = id;
}

int WrapperDirect3DPixelShader9::GetID() {
	return this->id;
}

IDirect3DPixelShader9* WrapperDirect3DPixelShader9::GetPS9() {
	return m_ps;
}

WrapperDirect3DPixelShader9* WrapperDirect3DPixelShader9::GetWrapperPixelShader(IDirect3DPixelShader9* ptr) {
	WrapperDirect3DPixelShader9* ret = (WrapperDirect3DPixelShader9*)( m_list.GetDataPtr(ptr) );
	if(ret == NULL) {
		Log::log("WrapperDirect3DPixelShader9::GetWrapperPixelShader(), ret is NULL\n");
	}
	return ret;
}

/*** IUnknown methods ***/
STDMETHODIMP WrapperDirect3DPixelShader9::QueryInterface(THIS_ REFIID riid, void** ppvObj) {
	HRESULT hr = m_ps->QueryInterface(riid, ppvObj);
	*ppvObj = this;
	return hr;
}
STDMETHODIMP_(ULONG) WrapperDirect3DPixelShader9::AddRef(THIS) {
	return m_ps->AddRef();
}
STDMETHODIMP_(ULONG) WrapperDirect3DPixelShader9::Release(THIS) {
	Log::log("WrapperDirect3DPixelShader9::Release() called! id:%d\n", this->id);
	return m_ps->Release();
}

/*** IDirect3DPixelShader9 methods ***/
STDMETHODIMP WrapperDirect3DPixelShader9::GetDevice(THIS_ IDirect3DDevice9** ppDevice) {
	return m_ps->GetDevice(ppDevice);
}

STDMETHODIMP WrapperDirect3DPixelShader9::GetFunction(THIS_ void* ptr,UINT* pSizeOfData) {
	return m_ps->GetFunction(ptr, pSizeOfData);
}

