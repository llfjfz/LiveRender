#include "game_server.h"
#include "wrap_direct3dvertexdeclaration9.h"

WrapperDirect3DVertexDeclaration9::WrapperDirect3DVertexDeclaration9(IDirect3DVertexDeclaration9* ptr, int _id): m_vd(ptr), id(_id) {}

void WrapperDirect3DVertexDeclaration9::SetID(int id) {
	this->id = id;
}

int WrapperDirect3DVertexDeclaration9::GetID() {
	return this->id;
}

IDirect3DVertexDeclaration9* WrapperDirect3DVertexDeclaration9::GetVD9() {
	return m_vd;
}

/*** IUnknown methods ***/
STDMETHODIMP WrapperDirect3DVertexDeclaration9::QueryInterface(THIS_ REFIID riid, void** ppvObj) {
	HRESULT hr = m_vd->QueryInterface(riid, ppvObj);
	*ppvObj = this;
	return hr;
}

STDMETHODIMP_(ULONG) WrapperDirect3DVertexDeclaration9::AddRef(THIS) {
	return m_vd->AddRef();
}

STDMETHODIMP_(ULONG) WrapperDirect3DVertexDeclaration9::Release(THIS) {
	Log::log("WrapperDirect3DVertexDeclaration9::Release() called! id:%d\n", this->id);
	return m_vd->Release();
}

/*** IDirect3DVertexDeclaration9 methods ***/
STDMETHODIMP WrapperDirect3DVertexDeclaration9::GetDevice(THIS_ IDirect3DDevice9** ppDevice) {
	Log::log("WrapperDirect3DVertexDeclaration9::GetDevice called!\n");
	return m_vd->GetDevice(ppDevice);
}

STDMETHODIMP WrapperDirect3DVertexDeclaration9::GetDeclaration(THIS_ D3DVERTEXELEMENT9* pElement,UINT* pNumElements) {
	return m_vd->GetDeclaration(pElement, pNumElements);
}