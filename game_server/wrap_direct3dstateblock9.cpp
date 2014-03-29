#include "wrap_direct3dstateblock9.h"
#include "wrap_direct3ddevice9.h"
#include "opcode.h"

WrapperDirect3DStateBlock9::WrapperDirect3DStateBlock9(IDirect3DStateBlock9* _sb, int _id): m_sb(_sb), id(_id){

}

int WrapperDirect3DStateBlock9::GetID() {
	return this->id;
}

void WrapperDirect3DStateBlock9::SetID(int id) {
	this->id = id;
}

IDirect3DStateBlock9* WrapperDirect3DStateBlock9::GetSB9() {
	return this->m_sb;
}


/*** IUnknown methods ***/
STDMETHODIMP WrapperDirect3DStateBlock9::QueryInterface(THIS_ REFIID riid, void** ppvObj) {
	Log::log("WrapperDirect3DStateBlock9::QueryInterface() called\n");
	HRESULT hr = m_sb->QueryInterface(riid, ppvObj);
	*ppvObj = this;
	return hr;
}

STDMETHODIMP_(ULONG) WrapperDirect3DStateBlock9::AddRef(THIS) {
	Log::log("WrapperDirect3DStateBlock9::AddRef() called\n");
	return m_sb->AddRef();
}

STDMETHODIMP_(ULONG) WrapperDirect3DStateBlock9::Release(THIS) {
	Log::log("WrapperDirect3DStateBlock9::Release() called\n");
	return m_sb->Release();
}

/*** IDirect3DStateBlock9 methods ***/
STDMETHODIMP WrapperDirect3DStateBlock9::GetDevice(THIS_ IDirect3DDevice9** ppDevice) {
	Log::log("WrapperDirect3DStateBlock9::GetDevice() called\n");
	IDirect3DDevice9* base = NULL;
	HRESULT hr = this->m_sb->GetDevice(&base);
	WrapperDirect3DDevice9 * ret = WrapperDirect3DDevice9::GetWrapperDevice9(base);
	if(ret == NULL){
		Log::log("WrapperDirect3DStateBlock9::GetDevice return NULL!\n");
	}
	*ppDevice = ret;
	return hr;
	//return m_sb->GetDevice(ppDevice);
}

STDMETHODIMP WrapperDirect3DStateBlock9::Capture(THIS) {
	Log::log("WrapperDirect3DStateBlock9::Capture() called\n");
	/*
	GET_BUFFER(StateBlockCapture_Opcode, id);
	END_BUFFER();
	client.SendPacket(msg, buf_size);
	*/
	cs.begin_command(StateBlockCapture_Opcode, id);
	cs.end_command();
	return m_sb->Capture();
}

STDMETHODIMP WrapperDirect3DStateBlock9::Apply(THIS) {
	Log::log("WrapperDirect3DStateBlock9::Apply() called\n");
	/*
	GET_BUFFER(StateBlockApply_Opcode, id);
	END_BUFFER();
	client.SendPacket(msg, buf_size);
	*/
	cs.begin_command(StateBlockApply_Opcode, id);
	cs.end_command();
	return m_sb->Apply();
}
